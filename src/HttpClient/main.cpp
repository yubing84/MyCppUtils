#include <iostream>

#include "HttpClient.h"
#include "AsynHttpClient.h"


using namespace std;

int main()
{
	// 同步请求
	HttpClient client;

	std::string result;

	client.Gets("https://api.ipify.org", result);

	std::cout << "Http Get同步请求结果为："<<result << std::endl;
	
	// 异步请求
	AsynHttpClient asynClient;

	asynClient.RegisterRequestCallbackFunc([](bool isSuccess, std::string response) {
		std::cout <<"Http Get异步请求结果为：" <<response << std::endl;
		});

	asynClient.AsynGet("https://api.ipify.org",true);


	getchar();

	return 0;
}