#include <iostream>

using namespace std;

#include "PortOccupation.h"

int main()
{
	if (PortOccupation::PortIsOccupied("866"))
	{
		std::cout << "端口被占用" << std::endl;
		//PortOccupation::KillAllOccupyPortProcess("1994");
	}
	else
	{
		std::cout << "端口未被占用" << std::endl;
	}
	
	getchar();

	return 0;
}