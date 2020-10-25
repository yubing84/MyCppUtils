#ifndef _TWOWAY_SOCKET_H
#define _TWOWAY_SOCKET_H

#include "AsynClientSocket.h"
#include "AsynServerSocket.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>

typedef std::function<bool(const std::string& message)> ServerMessageCallbackFunc;

class TwowaySocket
{
public:
	TwowaySocket();
	virtual~TwowaySocket() = default;

public:
	// 初始化
	void Init(
		const char* currentServerPort, // 当前服务器的端口
		const char* remoteServer1IP,  // 远程服务器1的IP
		const char* remoteServer1Port, // 远程服务器1的端口
		const char* remoteServer2IP, // 远程服务器2的IP
		const char* remoteServer2Port // 远程服务器2的端口
	);

	// 关闭
	void Close();

	// 发送数据到两个客户端
	void SendDataToRemoteServer(const char* data, int length);

	// 注册服务器收到客户端消息回调函数
	void RegisterServerMessageCallbackFunc(ServerMessageCallbackFunc func);

private:
	std::shared_ptr<AsynServerSocket> m_ServerPtr;
	std::vector<AsynClientSocket::ptr> m_ClientPtrVector;
};

#endif // !_TWOWAY_SOCKET_H
