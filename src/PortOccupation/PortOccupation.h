#ifndef _PORT_OCCUPATION_H_
#define _PORT_OCCUPATION_H_

#include <string>

enum CmdExecuteType
{
	CmdExecuteType_Popen,
	CmdExecuteType_CreateProcess
};


class PortOccupation
{
public:
	//! 
	//! @brief 通过_popen的方式返回命令行的运行结果，缺点是运行时有黑框一闪而过，优点是可传值所有的cmd命令
	//!
	//! @param[in] strCmd - 命令行字符串
	//!
	//! @return 运行结果字符串
	//!
	static std::string GetCmdResultByPopen(const std::string& strCmd);

	//! 
	//! @brief 通过CreateProcess的方式返回命令行的运行结果，缺点是可以运行部分cmd命令，优点是运行时没有黑框
	//!
	//! @param[in] strCmd - 命令行字符串
	//!
	//! @return 运行结果字符串
	//!
	static std::string GetCmdResultByCreateProcess(const std::string& strCmd);


	//! 
	//! @brief 检测本机某个端口是否被占用
	//!
	//! @param[in] portStr - 端口字符串
	//!
	//! @return 如果端口被占用，返回true;否则返回false
	//!
	static bool PortIsOccupied(const std::string& portStr);


	//! 
	//! @brief 根据进程PID杀掉进程
	//!
	//! @param[in] pid - 进程pid
	//!
	//! @return 杀掉进程成功返回true；否则返回false
	//!
	static bool KillProcessByPid(const std::string& pid);

	//! 
	//! @brief 杀掉所有占据指定端口的进程
	//!
	//! @param[in] portStr - 端口号
	//!
	//! @return 
	//!
	static void KillAllOccupyPortProcess(const std::string& portStr);
};

#endif // !_PORT_OCCUPATION_H_
