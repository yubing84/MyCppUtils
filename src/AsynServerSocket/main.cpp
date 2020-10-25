#include <iostream>
#include "AsynServerSocket.h"

#include "conio.h"

using namespace std;

int main()
{
	AsynServerSocket serverSocket;
	serverSocket.SetServerPort("1994");

	serverSocket.CreateServerSocket();

	std::cout << "创建服务器完成" << std::endl;

	while (true)
	{
		if (_kbhit()) // 如果有按键被按下
		{
			if (_getch() == 'e')
			{
				std::cout << "关闭服务器" << std::endl;

				serverSocket.CloseServer();

				break;
			}

		}
	}

	getchar();

	return 0;
}


//#include <iostream>
//#include<thread>
//#include<ws2tcpip.h>
//#include<Windows.h>
//#include<string>
//#include<string.h>
//
//using namespace std;
//
//class tcpthread
//{
//public:
//	void Main()
//	{
//		char buf[1024] = { 0 };//接收信息的最大长度，记位buf
//		while (true)
//		{
//			int recvlen = recv(client, buf, sizeof(buf) - 1, 0);//windows没有read函数，linux才有
//			if (recvlen <= 0)break;//没有收到
//			if (strstr(buf, "q") != NULL)//按q退出
//			{
//				char re[] = "quit success!!!\n";
//				send(client, re, strlen(re) + 1, 0);//加1是因为还有\0
//				break;
//			}
//			int sendlen = send(client, "ok\n", 3, 0);//linux可以使用write
//			cout << "receive:" << buf << endl;
//			//len是接收数据的实际大小，len<=buf长度（这里是1024）
//		}
//
//
//		closesocket(client);//关闭连接
//
//	}
//	int client = 0;
//};
//
//int main(int argc, char* argv[])
//{
//	//初始化动态链接库
//	WSADATA ws;
//	WSAStartup(MAKEWORD(2, 2), &ws);//22是版本号,加载动态链接库
//	int sock = socket(AF_INET, SOCK_STREAM, 0);//AF_INET指明调用TCP/IP协议,SOCK_STREAM是TCP的协议（相对于UDP来讲）
//	cout << sock << endl;//打印句柄id，失败返回负值
//	//失败提示
//	if (sock == -1)
//	{
//		cout << "create socket failed!" << endl;
//		return -1;
//	}
//
//	//测试端口号
//	unsigned short port = 1994;
//	if (argc > 1)
//	{
//		port = atoi(argv[1]);
//	}
//
//	//创建TCP相关的结构体
//	sockaddr_in saddr;
//	saddr.sin_family = AF_INET;//使用TCP
//	saddr.sin_port = htons(port);//本地字节序转网络字节序
//	//X86架构是小端的而网络字节流是大端的，
//	//Linux不一定，小型linux使用的也是和网路字节序一样的话转换也只是一个空的宏，
//	//这时候会可有可无，但考虑兼容性要求建议加上
//	saddr.sin_addr.s_addr = htonl(0);//这里可以指定网卡，0是任意的意思
//
//	if (bind(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0)//绑定端口号到上面创建的socket,并判断是否成功
//	{
//		cout << "bind port " << port << " failed!" << endl;
//		return -2;
//	}
//	else
//	{
//		cout << "bind port " << port << " success!" << endl;
//	}
//
//	listen(sock, 10);//监听，接受连接；10是列表大小，套接字接收队列的最大大小 
//	//accept每调用一次队列就会减少一个
//
//	while (true)
//	{
//		sockaddr_in caddr;
//		socklen_t len = sizeof(caddr);
//		int client = accept(sock, (sockaddr*)&caddr, &len);//取信息
//		if (client <= 0)break;
//
//		//cout << client << endl;
//		//char* ip = inet_ntoa(caddr.sin_addr);
//		//unsigned short cport = ntohs(caddr.sin_port);//网络字节序转本地字节序
//		//cout << "client ip is " << ip << " port is " << cport << endl;
//		tcpthread* th = new tcpthread();
//		th->client = client;
//		thread sth(&tcpthread::Main, th);
//		sth.detach();//释放主线程拥有的子线程的资源
//	}
//	closesocket(sock);
//
//	return 0;
//}