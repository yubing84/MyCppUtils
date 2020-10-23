#ifndef ASYN_SERVER_SOCKET_H
#define ASYN_SERVER_SOCKET_H

// Windows套接字需要的
#include "winsock2.h"
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <thread>
#include <memory>
#include <vector>
#include <mutex>
#include <iostream>

class tcpthread
{
public:
	void Main()
	{
		char buf[1024] = { 0 };//接收信息的最大长度，记位buf
		while (true)
		{
			int recvlen = recv(client, buf, sizeof(buf) - 1, 0);//windows没有read函数，linux才有
			if (recvlen <= 0)break;//没有收到
			if (strstr(buf, "q") != NULL)//按q退出
			{
				char re[] = "quit success!!!\n";
				send(client, re, strlen(re) + 1, 0);//加1是因为还有\0
				break;
			}
			int sendlen = send(client, "ok\n", 3, 0);//linux可以使用write
			std::cout << "receive:" << buf << std::endl;
			//len是接收数据的实际大小，len<=buf长度（这里是1024）
		}


		closesocket(client);//关闭连接

	}
	int client = 0;
};


class AsynServerSocket
{
public:
	AsynServerSocket() = default;
	virtual~AsynServerSocket() = default;

public:
	// 设置服务端socket的ip和端口
	void SetServerSocketIPAndPort(const char* port);

	// 创建并连接服务端Socket
	bool CreateServerSocket();

	// 关闭套接字
	void CloseSocket();
private:
	void ServerProcessThreadFunction();

	void SingleSocketReciveThreadFunction(int socket);
private:
	SOCKADDR_IN m_ServerAddress;
	SOCKET m_ServerSocket;

	std::shared_ptr<std::thread> m_ServerRunThreadPtr;
	std::vector<std::shared_ptr<std::thread>> m_SingleSocketRecvThreadVector;

	std::mutex m_SingleSocketMutex;
};

#endif // !ASYN_SERVER_SOCKET_H
