#ifndef ASYN_HTTP_CLIENT_H
#define ASYN_HTTP_CLIENT_H

#include <memory>
#include <functional>
#include <string>

typedef std::function<void(bool,std::string)> RequestFunc;

class AsynHttpClient
{
public:
	typedef std::shared_ptr<AsynHttpClient> ptr;
	AsynHttpClient();
	virtual~AsynHttpClient();

public:
	/** 
	* @brief 注册请求回调函数
	* @param func 输入参数，需要回调的可调用对象
	*/
	void RegisterRequestCallbackFunc(RequestFunc func);

	/**
	* @brief 异步HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:https://api.ipify.org
	* @param isUseSsl 输入参数，是否使用ssl加密链接， 适用于https类型的网址
	* @param pCaPath 输入参数，需要使用的https证书加密路径
	* @return 返回是否Post成功
	*/
	int AsynGet(const std::string & strUrl,bool isUseSsl = false, const char * pCaPath = NULL);


	/**
	* @brief 异步HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:https://api.ipify.org
	* @param strPost 输入参数，向服务器发送的字段数据
	* @param isUseSsl 输入参数，是否使用ssl加密链接， 适用于https类型的网址
	* @param pCaPath 输入参数，需要使用的https证书加密路径
	* @return 返回是否Post成功
	*/
	int AsynPost(const std::string & strUrl, const std::string & strPost, bool isUseSsl = false,const char * pCaPath = NULL);

	/**
	* @brief 异步HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:https://api.ipify.org
	* @param strJsonData 输入参数，向服务器发送的Json数据
	* @param isUseSsl 输入参数，是否使用ssl加密链接， 适用于https类型的网址
	* @param pCaPath 输入参数，需要使用的https证书加密路径
	* @return 返回是否Post成功
	*/
	int AsynPostJsonDataToServer(const std::string& strUrl, const std::string strJsonData, bool isUseSsl = false, const char* pCaPath = NULL);
private:
	RequestFunc m_RequestFunc;
	bool m_bDebug;
};


#endif
