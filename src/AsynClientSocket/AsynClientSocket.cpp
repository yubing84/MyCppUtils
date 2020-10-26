#include "AsynClientSocket.h"

#include <sstream>
#include <iostream>

std::string ASCII2UTF8(const char* cont)
{
	if (NULL == cont)
	{
		return std::string("");
	}

	int num = MultiByteToWideChar(CP_ACP, NULL, cont, -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_ACP, NULL, cont, -1, buffw, num);

	int len = WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;

	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}


void intToByte(int i, BYTE abyte[], int size = 4)
{
	memset(abyte, 0, sizeof(byte) * size);

	abyte[3] = (byte)(0xff & i);

	abyte[2] = (byte)((0xff00 & i) >> 8);

	abyte[1] = (byte)((0xff0000 & i) >> 16);

	abyte[0] = (byte)((0xff000000 & i) >> 24);

}


void AsynClientSocket::SetRemoteServerSocketIPAndPort(const char* ip, const char* port)
{
	// 设置远程服务器地址
	SOCKADDR_IN		remoteServerAddress;

	remoteServerAddress.sin_family = AF_INET; // 需要连接的IPV4协议
	inet_pton(AF_INET, ip, (void*)&remoteServerAddress.sin_addr.S_un.S_addr);	// 需要连接的远程服务器的IP地址

	// 端口字符型转int型
	std::istringstream intStr(port);	// 需要连接的远程服务器的端口
	int tempPort;
	intStr >> tempPort;

	remoteServerAddress.sin_port = htons(tempPort);

	m_RemoteServerAddress = remoteServerAddress;
}

bool AsynClientSocket::CreateAndConnectToRemoteServer()
{
	int result = -1;

	// 初始化Windows套接字
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return false;
	}
	std::cout << "初始化Windows套接字成功" << std::endl;

	// 创建套接字
	m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_Socket)
	{
		WSACleanup();
		return false;
	}

	// 设置套接字非阻塞模式
	//unsigned long ul = 1;
	//result = ioctlsocket(m_Socket, FIONBIO, (unsigned long*)&ul);
	//if (SOCKET_ERROR == result)
	//{
	//	WSACleanup();
	//	return false;
	//}

	// 连接服务器
	while (true)
	{
		result = connect(m_Socket, (LPSOCKADDR)&m_RemoteServerAddress, sizeof(m_RemoteServerAddress));
		if (SOCKET_ERROR == result)
		{
			int nErrCode = WSAGetLastError();
			if (WSAEWOULDBLOCK == nErrCode || WSAEINVAL == nErrCode)    //连接还没有完成
			{
				continue;
			}
			else if (WSAEISCONN == nErrCode)//连接已经完成
			{
				break;
			}
			else//其它原因，连接失败
			{
				//关闭套接字
				closesocket(m_Socket);
				//释放套接字资源
				WSACleanup();

				std::cout << "远程服务器连接失败" << std::endl;

				return false;
			}
		}
		else if(result == 0)
		{
			break;
		}
	}

	std::cout << "连接远程服务器成功" << std::endl;

	return true;
}

bool AsynClientSocket::SendDataToRemoteServer(const std::string& str)
{
	//std::string utf8Str = ASCII2UTF8(str.c_str());

	std::string utf8Str = str;

	char* messageByte = new char[utf8Str.length() + 4];

	byte dataLengthByteArray[4];

	intToByte(utf8Str.length(), dataLengthByteArray);

	memcpy(messageByte, dataLengthByteArray, 4);

	memcpy(messageByte + 4, utf8Str.c_str(), utf8Str.length());

	int result = send(m_Socket, (char*)messageByte, sizeof(char) * utf8Str.length() + 4, 0);

	if (SOCKET_ERROR == result)
	{
		//关闭套接字
		closesocket(m_Socket);
		//释放套接字资源
		WSACleanup();

		return false;
	}

	delete[] messageByte;

	return true;

}

void AsynClientSocket::CloseClient()
{
	//关闭套接字
	closesocket(m_Socket);
	//释放套接字资源
	WSACleanup();
}
