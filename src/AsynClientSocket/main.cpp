#include <iostream>
#include "AsynClientSocket.h"

#include "conio.h"
#include <thread>
#include<chrono>

using namespace std;


int main()
{
	AsynClientSocket clientSocket;

	//clientSocket.SetRemoteServerSocketIPAndPort("192.168.0.210", "8661");
	clientSocket.SetRemoteServerSocketIPAndPort("127.0.0.1", "1994");

	if (clientSocket.CreateAndConnectToRemoteServer())
	{
		std::cout << "连接服务器成功" << std::endl;
	}
	else
	{
		std::cout << "连接服务器失败" << std::endl;

		return 0;
	}

	int A = 0;

	while (true)
	{
		std::string str = "HelloWorldHelloWorld"+ std::to_string(A);

		if (clientSocket.SendDataToRemoteServer(str))
		{
			std::cout << "向服务器发送" << str << "成功" << std::endl;
		}
		else
		{
			std::cout << "向服务器发送" << str << "失败" << std::endl;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(3000));

		A++;

		if (_kbhit()) // 如果有按键被按下
		{
			if (_getch() == 's') //如果按下了q键则跳出循环
			{
				std::string str = "HelloWorldHelloWorld";

				//for (int i = 0; i < 20; ++i)
				//{
				//	if (clientSocket.SendDataToRemoteServer(str))
				//	{
				//		std::cout << "向服务器发送" << str << "成功" << std::endl;
				//	}
				//	else
				//	{
				//		std::cout << "向服务器发送" << str << "失败" << std::endl;
				//	}
				//}

				//for (int i = 0; i < 100; ++i)
				//{
				//	str += "HelloWorld";
				//}
				//if (clientSocket.SendDataToRemoteServer(str))
				//{
				//	std::cout << "向服务器发送" << str << "成功" << std::endl;
				//}
				//else
				//{
				//	std::cout << "向服务器发送" << str << "失败" << std::endl;
				//}



				if (clientSocket.SendDataToRemoteServer(str))
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
				clientSocket.CloseClient();

				break;
			}

		}
	}


	getchar();

	return 0;
}