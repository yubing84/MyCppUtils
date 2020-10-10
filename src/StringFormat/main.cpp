#include <iostream>
#include "StringFormatUtils.h"

using namespace std;

int main()
{
	std::string exampleStr = StringFormat("Hello {0}{1}{2}", "Wo", "rl", "d");

	std::cout << "字符串格式化的结果为：" << exampleStr << std::endl;

	getchar();

	return 0;
}