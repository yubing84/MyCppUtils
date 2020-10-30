#include "PortOccupation.h"
#include <vector>

#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#include <iostream>


std::wstring Str2Wstr(std::string str)
{
	unsigned len = str.size() * 2;// 预留字节数
	setlocale(LC_CTYPE, "");     //必须调用此函数
	wchar_t* p = new wchar_t[len];// 申请一段内存存放转换后的字符串
	mbstowcs(p, str.c_str(), len);// 转换
	std::wstring str1(p);
	delete[] p;// 释放申请的内存
	return str1;
}

std::vector<std::string> StringSplitBySymbol(const  std::string& s, const std::string& symbol = ",")
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = symbol.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(symbol, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

std::string PortOccupation::GetCmdResultByPopen(const std::string& strCmd)
{
	char buf[10240] = { 0 };
	FILE* pf = NULL;

	if ((pf = _popen(strCmd.c_str(), "r")) == NULL)
	{
		return "";
	}

	std::string strResult;
	while (fgets(buf, sizeof buf, pf))
	{
		strResult += buf;
	}

	_pclose(pf);

	unsigned int iSize = strResult.size();
	if (iSize > 0 && strResult[iSize - 1] == '\n')  // linux
	{
		strResult = strResult.substr(0, iSize - 1);
	}

	return strResult;
}

std::string PortOccupation::GetCmdResultByCreateProcess(const std::string& strCmd)
{
	std::wstring pszCmd_w = Str2Wstr(strCmd);
	// 创建匿名管道,write->read;
	SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	HANDLE hRead, hWrite;
	if (!CreatePipe(&hRead, &hWrite, &sa, 0))
	{
		return (" ");
	}

	// 设置命令行进程启动信息(以隐藏方式启动命令并定位其输出到hWrite
	STARTUPINFO si = { sizeof(STARTUPINFO) }; // Pointer to STARTUPINFO structure;
	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	//si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE; //隐藏窗口；
	si.hStdError = hWrite;
	si.hStdError = hWrite;
	si.hStdOutput = hWrite; //管道的输入端口连接命令行的输出；
	// 启动命令行
	PROCESS_INFORMATION pi;// Pointer to PROCESS_INFORMATION structure;
	if (!CreateProcess(NULL,
		(LPWSTR)pszCmd_w.c_str(),
		NULL,
		NULL,
		TRUE,
		//FALSE,          // Set handle inheritance to FALSE
		NULL,
		//0,              // No creation flags
		NULL,
		NULL,
		&si,
		&pi))
	{
		return "";
	}
	CloseHandle(hWrite);//关闭管道的输入端口；
	// 读取命令行返回值
	std::string strRetTmp;
	char buff[1024] = { 0 };
	DWORD dwRead = 0;
	strRetTmp = buff;
	while (ReadFile(hRead, buff, 1024, &dwRead, NULL))//从管道的输出端获取命令行写入的数据；
	{
		strRetTmp += buff;
	}
	CloseHandle(hRead);//关闭管道的输出端口；
	return strRetTmp;
}


bool PortOccupation::PortIsOccupied(const std::string& portStr)
{
	std::string cmdStrPopen = "netstat -aon|findstr ";

	std::string cmdStrCreateProcess = "cmd.exe /c netstat -aon|findstr ";

	std::string tempPortStr = "\"" + portStr + "\"";

	cmdStrCreateProcess += tempPortStr;

	std::string cmdResult = GetCmdResultByCreateProcess(cmdStrCreateProcess);

	// 如果命令行运行结果没有任何返回结果，说明该端口没有被占用，如果有结果说明被占用
	if (!cmdResult.empty())
	{
		std::vector<std::string> split = StringSplitBySymbol(cmdResult, "\n");

		for (int i = 0; i < split.size(); ++i)
		{
			std::string tempStr = split[i];

			// 如果端口在监听或者已经建立连接，则表示端口被占用
			if (tempStr.find("LISTENING") != std::string::npos || tempStr.find("ESTABLISHED") != std::string::npos)
			{
				// 判断端口是不是当前端口
				std::vector<std::string> tempSplitVector = StringSplitBySymbol(tempStr," ");

				for (int j = 0; j < tempSplitVector.size(); ++j)
				{
					if (tempSplitVector[j].find(":") != std::string::npos)
					{
						std::vector<std::string> tempVector = StringSplitBySymbol(tempSplitVector[j], ":");
						if (tempVector.size() > 1)
						{
							std::string tempPortString = tempVector[1];

							if (tempPortString.compare(portStr) == 0)
							{
								return true;
							}
						}
					}
				}

			}
		}
	}


	return false;
}

bool PortOccupation::KillProcessByPid(const std::string& pid)
{
	std::string cmdStrCreatePorcess = "cmd.exe /c taskkill /f /pid ";

	std::string tempPidStr = "\"" + pid + "\"";

	cmdStrCreatePorcess += tempPidStr;

	std::string cmdResult = GetCmdResultByCreateProcess(cmdStrCreatePorcess);

	if (!cmdResult.empty())
	{
		if (cmdResult.find("成功") != std::string::npos)
		{
			return true;
		}
	}

	return false;
}

void PortOccupation::KillAllOccupyPortProcess(const std::string& portStr)
{
	std::string cmdStrCreatePorcess = "cmd.exe /c netstat -aon|findstr ";

	std::string tempPortStr = "\"" + portStr + "\"";

	cmdStrCreatePorcess += tempPortStr;

	std::string cmdResult = GetCmdResultByCreateProcess(cmdStrCreatePorcess);

	if (cmdResult.empty())
	{
		return ;
	}

	std::vector<std::string> split = StringSplitBySymbol(cmdResult, "\n");

	for (int i = 0; i < split.size(); ++i)
	{
		std::vector<std::string> tempVector = StringSplitBySymbol(split[i], " ");

		std::string port = tempVector[tempVector.size() - 1];

		KillProcessByPid(port);
	}
}
