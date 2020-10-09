#include <iostream>
#include "CJsonObject.hpp"

using namespace std;

int main()
{
	neb::CJsonObject jsonObject;

	jsonObject.Add("test", "helloWorld");

	std::cout << "Æ´´ÕµÄjsonÎª" << jsonObject.ToString() << std::endl;

	getchar();

	return 0;
}