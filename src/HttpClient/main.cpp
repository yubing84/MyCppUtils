#include <iostream>

#include "HttpClient.h"

using namespace std;



int main()
{
	HttpClient client;

	std::string result;

	client.Gets("https://api.ipify.org", result);

	std::cout << "Http Get请求结果为："<<result << std::endl;

	getchar();

	return 0;
}