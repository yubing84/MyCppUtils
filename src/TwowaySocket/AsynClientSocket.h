#ifndef ASYN_CLIENT_SOCKET_H
#define ASYN_CLIENT_SOCKET_H

// Windows套接字需要的
#include "winsock2.h"
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <memory>

class AsynClientSocket
{
public:
	typedef std::shared_ptr<AsynClientSocket> ptr;
	AsynClientSocket() = default;
	virtual~AsynClientSocket() = default;

public:
	// 设置远程服务端socket的ip和端口
	void SetRemoteServerSocketIPAndPort(const char* ip, const char* port);

	// 创建并连接到远程服务器
	bool CreateAndConnectToRemoteServer();

	// 发送数据
	bool SendDataToRemoteServer(const char* data,int length);

	// 关闭套接字
	void CloseClient();
private:
	SOCKADDR_IN m_RemoteServerAddress;
	SOCKET m_Socket;
};

#endif // !ASYN_CLIENT_SOCKET_H
