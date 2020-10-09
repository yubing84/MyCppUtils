#include <iostream>
#include "TinyTimer.h"

using namespace std;

int main()
{
	TinyTimer timer;
	timer.AsyncOnceExecute(1000, []() {
		std::cout << "延迟一秒打印一次" << std::endl; 
		});

	getchar();

	return 0;
}