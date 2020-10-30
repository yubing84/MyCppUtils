#include "TwowaySocket.h"

TwowaySocket::TwowaySocket()
{

}

void TwowaySocket::Init(
	const char* currentServerPort,
	const char* remoteServer1IP, 
	const char* remoteServer1Port, 
	const char* remoteServer2IP, 
	const char* remoteServer2Port)
{
	/*----- 开启本机socket服务器 -----*/
	m_ServerPtr = std::make_shared<AsynServerSocket>();
	m_ServerPtr->SetServerPort(currentServerPort);
	m_ServerPtr->CreateServerSocket();

	/*----- 开启两个客户端 -----*/
	// 开启第一个客户端并连接到服务器
	std::shared_ptr<AsynClientSocket> client1 = std::make_shared<AsynClientSocket>();
	client1->SetRemoteServerSocketIPAndPort(remoteServer1IP, remoteServer1Port);
	client1->CreateAndConnectToRemoteServer();

	m_ClientPtrVector.push_back(client1);

	// 开启第二个客户端并连接到服务器
	std::shared_ptr<AsynClientSocket> client2 = std::make_shared<AsynClientSocket>();
	client2->SetRemoteServerSocketIPAndPort(remoteServer2IP, remoteServer2Port);
	client2->CreateAndConnectToRemoteServer();

	m_ClientPtrVector.push_back(client2);

}

void TwowaySocket::Close()
{
	// 关闭服务器
	if (m_ServerPtr != nullptr)
	{
		m_ServerPtr->CloseServer();
	}

	// 关闭所有客户端
	for (int i = 0; i < m_ClientPtrVector.size(); ++i)
	{
		if (m_ClientPtrVector[i] != nullptr)
		{
			m_ClientPtrVector[i]->CloseClient();
		}
	}
}

void TwowaySocket::SendDataToRemoteServer(const char* data, int length)
{
	for (int i = 0; i < m_ClientPtrVector.size(); ++i)
	{
		if (m_ClientPtrVector[i] != nullptr)
		{
			m_ClientPtrVector[i]->SendDataToRemoteServer(data, length);
		}
	}
}
