#include <iostream>
#include "AsynClientSocket.h"

#include "conio.h"

using namespace std;

int main()
{
	AsynClientSocket clientSocket;

	clientSocket.SetRemoteServerSocketIPAndPort("127.0.0.1", "1994");

	clientSocket.CreateAndConnectToRemoteServer();

	while (true)
	{
		if (_kbhit()) // 如果有按键被按下
		{
			if (_getch() == 's') //如果按下了q键则跳出循环
			{
				std::string str = "你好";

				if (clientSocket.SendDataToRemoteServer(str.c_str(), str.length()))
				{
					std::cout << "向服务器发送" << str << "成功" << std::endl;
				}
				else
				{
					std::cout << "向服务器发送" << str << "失败" << std::endl;
				}			
			}
			else if (_getch() == 'e')
			{
				clientSocket.CloseSocket();

				break;
			}

		}
	}


	getchar();

	return 0;
}