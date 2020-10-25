#include <iostream>

#include "TwowaySocket.h"

using namespace std;

int main()
{
	TwowaySocket twowaySocket;

	twowaySocket.Init("1994", "127.0.0.1", "1995", "127.0.0.1", "1996");

	getchar();
	
	return 0;
}