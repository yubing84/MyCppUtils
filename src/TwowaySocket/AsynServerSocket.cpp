#include "AsynServerSocket.h"

#include <sstream>

#include <iostream>

std::string UTF82ASCII(const char* cont)
{
	if (NULL == cont)
	{
		return std::string("");
	}
	int num = MultiByteToWideChar(CP_UTF8, NULL, cont, -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_UTF8, NULL, cont, -1, buffw, num);
	int len = WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;
	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}

//int 转 4字节 BYTE[],
void intToByte(int i, BYTE abyte[],int size= 4)
{
	memset(abyte, 0, sizeof(byte) * size);

	abyte[3] = (byte)(0xff & i);

	abyte[2] = (byte)((0xff00 & i) >> 8);

	abyte[1] = (byte)((0xff0000 & i) >> 16);

	abyte[0] = (byte)((0xff000000 & i) >> 24);

}

//4字节 BYTE[] 转 int 
int bytesToInt(BYTE bytes[])
{

	int addr = bytes[3] & 0xFF;

	addr |= ((bytes[2] << 8) & 0xFF00);

	addr |= ((bytes[1] << 16) & 0xFF0000);

	addr |= ((bytes[0] << 24) & 0xFF000000);

	//long int result = (x[0] << 24) + (x[1] << 16) + (x[2] << 8) + x[3];   

	return addr;
}



AsynServerSocket::AsynServerSocket()
{
	m_IsCloseServer = false;
}

void AsynServerSocket::SetServerPort(const char* port)
{
	// 设置当前服务器地址
	SOCKADDR_IN		serverAddress;

	serverAddress.sin_family = AF_INET;

	inet_pton(AF_INET, "0.0.0.0", (void*)&serverAddress.sin_addr.S_un.S_addr);

	// 端口字符型转int型
	std::istringstream intStr(port);
	int tempPort;
	intStr >> tempPort;

	serverAddress.sin_port = htons(tempPort);

	m_ServerAddress = serverAddress;
}

bool AsynServerSocket::CreateServerSocket()
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
	m_ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_ServerSocket)
	{
		WSACleanup();
		return false;
	}
	std::cout << "创建服务器套接字成功" << std::endl;

	// 设置套接字为非阻塞模式
	//unsigned long ul = 1;
	//result = ioctlsocket(m_ServerSocket, FIONBIO, (unsigned long*)&ul);
	//if (SOCKET_ERROR == result)
	//{
	//	WSACleanup();
	//	return false;
	//}


	// 绑定套接字
	result = bind(m_ServerSocket, (LPSOCKADDR)&m_ServerAddress, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == result)
	{
		closesocket(m_ServerSocket);
		WSACleanup();
		return false;
	}

	std::cout << "绑定服务器套接字成功" << std::endl;

	// 开始监听
	result = listen(m_ServerSocket, 5);
	if (SOCKET_ERROR == result)
	{
		closesocket(m_ServerSocket);
		WSACleanup();
		return false;
	}

	std::cout << "服务器套接字开始监听" << std::endl;

	// 开始接受客户端请求
	m_ServerMainThreadPtr = std::make_shared<std::thread>(&AsynServerSocket::ServerProcessThreadFunction,this);

	return true;
}

void AsynServerSocket::CloseServer()
{
	// 关闭服务器，让分离子线程退出
	m_IsCloseServer = true;

	// 关闭套接字和清理资源
	closesocket(m_ServerSocket);
	WSACleanup();

	m_ServerMainThreadPtr->join();
}

void AsynServerSocket::RegisterServerMessageCallbackFunc(ServerMessageCallbackFunc func)
{
	m_ServerMessageCallbackFunc = func;
}

void AsynServerSocket::ServerProcessThreadFunction()
{
	while (true)
	{
		if (m_IsCloseServer)
		{
			break;
		}

		SOCKET clientSocket;
		SOCKADDR_IN addrClient;
		int len = sizeof(SOCKADDR);

		clientSocket = accept(m_ServerSocket, (SOCKADDR*)&addrClient, &len);

		std::cout << "一个客户端已连接到服务器，socket是：" << clientSocket << std::endl;

		std::shared_ptr<std::thread> singleSocketRecvThread = std::make_shared<std::thread>(&AsynServerSocket::SingleSocketReciveThreadFunction, this, clientSocket);
		singleSocketRecvThread->detach();
	}

	std::cout << "服务器运行子线程正确退出" << std::endl;
}

void AsynServerSocket::SingleSocketReciveThreadFunction(int socket)
{
	int clientSocket = socket;

	while (true)
	{
		char recvBuf[1024] = {0};

		int recvlen = recv(clientSocket, recvBuf, sizeof(recvBuf) , 0);	

		// 客户端已关闭连接
		if (recvlen == 0)
		{
			std::cout << "socket连接关闭" << std::endl;

			closesocket(socket);

			break;
		}
		// 出错
		else if (recvlen == -1)
		{

			break;
		}
		else if (recvlen > 0)
		{
			//recvBuf[recvlen] = '\0';

			// 得到协议格式中数据的长度
			char dataLengthStr[4];
			memcpy(dataLengthStr, recvBuf, 4);
			int dataLength = bytesToInt((byte*)dataLengthStr);

			// 得到当前消息体的数据的长度
			int messageLength = recvlen - 4;

			char* dataStr = new char[dataLength+1];

			memset(dataStr, 0, dataLength);

			memcpy(dataStr, recvBuf + 4, messageLength);

			int writeDataLength = messageLength;

			while (writeDataLength < dataLength)
			{
				char buffer[1024] = { 0 };

				int recvbufferLen = recv(clientSocket, buffer, sizeof(buffer), 0);

				// 客户端已关闭连接
				if (recvbufferLen == 0)
				{
					std::cout << "socket连接关闭" << std::endl;

					closesocket(socket);

					break;
				}
				// 出错
				else if (recvbufferLen == -1)
				{

					break;
				}
				else if (recvbufferLen > 0)
				{
					int tempWriteDataLength = writeDataLength + recvbufferLen;

					if (tempWriteDataLength >= dataLength)
					{
						memcpy(dataStr + writeDataLength -1, buffer, dataLength - writeDataLength);

						//break;
					}
					else
					{
						memcpy(dataStr + writeDataLength -1 , buffer, recvbufferLen);
					}

					writeDataLength = tempWriteDataLength;

				}
			}

			dataStr[dataLength] = '\0';

			std::cout << "收到客户端的数据：" << dataStr << std::endl;
			//for (int i = 0; i < dataLength; ++i)
			//{
			//	std::cout << dataStr[i];
			//}
			//std::cout << std::endl;

			delete[] dataStr;

		}	
	}

	std::cout << "服务器单个接收数据子线程正确退出" << std::endl;
}
