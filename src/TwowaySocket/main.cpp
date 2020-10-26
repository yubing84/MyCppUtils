#include <iostream>

#include "TwowaySocket.h"

#include "conio.h"

using namespace std;

int main()
{
	TwowaySocket twowaySocket;

	twowaySocket.Init("1994", "192.168.0.210", "8661", "127.0.0.1", "1996");

	while (true)
	{
		if (_kbhit()) // 如果有按键被按下
		{
			if (_getch() == 's')
			{
				std::string str = "你好";

				twowaySocket.SendDataToRemoteServer(str.c_str(), str.length());

				break;
			}

		}
	}


	getchar();
	
	return 0;
}