#include <iostream>

#include "UUIDUtils.h"

using namespace std;

int main()
{

	for (int i = 0; i < 10; ++i)
	{
		std::cout << UUIDUtils::GenerateUUID() << std::endl;
	}


	getchar();

	return 0;
}