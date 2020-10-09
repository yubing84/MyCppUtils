#include "WebsocketClient.h"
#include "Base64.h"

WebsocketClient::WebsocketClient()
{
	m_WebsocketClient.clear_access_channels(websocketpp::log::alevel::all);  // 开启全部接入日志级别
	m_WebsocketClient.clear_error_channels(websocketpp::log::elevel::all);   // 开启全部错误日志级别

	m_WebsocketClient.init_asio();       // 初始化asio
	m_WebsocketClient.start_perpetual(); // 避免请求为空时退出，实际上，也是避免asio退出

	// 独立运行client::run()的线程，主要是避免阻塞
	m_Thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_WebsocketClient);

	m_OnOpenFunc = nullptr;
	m_OnFailFunc = nullptr;
	m_OnCloseFunc = nullptr;
	m_OnMessageFunc = nullptr;
	m_OnReconnectFunc = nullptr;

	m_WebsocketStatus = WebsocketStatus::Connecting;
	m_WebsocketConnectMode = WebsocketConnectMode::NoHttpAuthorization;

	m_bIsNeedCloseConnection = false;
}

WebsocketClient::~WebsocketClient()
{
	m_WebsocketClient.stop_perpetual();

	// 如果当前连接还处于连接状态则关闭
	if (m_ConnectionMetadataPtr != nullptr && m_ConnectionMetadataPtr->get_status() == "Open")
	{
		websocketpp::lib::error_code ec;
		m_WebsocketClient.close(m_ConnectionMetadataPtr->get_hdl(), websocketpp::close::status::going_away, "", ec); // 关闭连接

		if (ec) {
			std::string errorMessage = ec.message();
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
		}
	}

	//m_Thread->join();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	try
	{
		if (m_Thread && m_Thread->joinable())
		{
			m_Thread->join();
		}
	}
	catch (const std::exception& e)
	{
		std::string errorMessage = e.what();
	}


	//m_Thread.reset();
}

bool WebsocketClient::Connect(std::string const & url)
{
	websocketpp::lib::error_code ec;

	// 创建connect的共享指针，注意，此时创建并没有实际建立
	client::connection_ptr con = m_WebsocketClient.get_connection(url, ec);  

	if (ec) {
		std::cout << "> Connect initialization error: " << ec.message() << std::endl;
		return false;
	}

	// 创建连接的metadata信息，并保存
	connection_metadata::ptr metadata_ptr = websocketpp::lib::make_shared<connection_metadata>(con->get_handle(), url);
	m_ConnectionMetadataPtr = metadata_ptr;

	m_WebsocketConnectMode = WebsocketConnectMode::NoHttpAuthorization;

	// 注册连接打开的Handler
	//con->set_open_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_open,
	//	metadata_ptr,
	//	&m_WebsocketClient,
	//	websocketpp::lib::placeholders::_1
	//));
	con->set_open_handler(websocketpp::lib::bind(
		&WebsocketClient::OnOpen,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接失败的Handler
	//con->set_fail_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_fail,
	//	metadata_ptr,
	//	&m_WebsocketClient,
	//	websocketpp::lib::placeholders::_1
	//));
	con->set_fail_handler(websocketpp::lib::bind(
		&WebsocketClient::OnFail,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接关闭的Handler
	//con->set_close_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_close,
	//	metadata_ptr,
	//	&m_WebsocketClient,
	//	websocketpp::lib::placeholders::_1
	//));
	con->set_close_handler(websocketpp::lib::bind(
		&WebsocketClient::OnClose,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接接收消息的Handler
	//con->set_message_handler(websocketpp::lib::bind(
	//	&connection_metadata::on_message,
	//	metadata_ptr,
	//	websocketpp::lib::placeholders::_1,
	//	websocketpp::lib::placeholders::_2
	//));
	con->set_message_handler(websocketpp::lib::bind(
		&WebsocketClient::OnMessage,
		this,
		websocketpp::lib::placeholders::_1,
		websocketpp::lib::placeholders::_2
	));

	// 进行连接
	m_WebsocketClient.connect(con);

	m_WebsocketStatus = WebsocketStatus::Connecting;

	std::cout << "Websocket连接成功" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return true;
}

bool WebsocketClient::Connect(std::string const & url, const std::string& httpAuthKey,const std::string& httpAuthValue)
{
	websocketpp::lib::error_code ec;

	// 创建connect的共享指针，注意，此时创建并没有实际建立
	client::connection_ptr con = m_WebsocketClient.get_connection(url, ec);

	// 生成Request Header
	if (!httpAuthKey.empty() && !httpAuthValue.empty())
	{
		std::string result = httpAuthKey + ":" + httpAuthValue;
		con->append_header("Authorization", GetHttpAuthenticationStr(result));
	}

	if (ec) {
		std::cout << "> Connect initialization error: " << ec.message() << std::endl;
		return false;
	}
	// 赋值成员变量
	m_WebsocketUrl = url;
	m_HttpAuthKey = httpAuthKey;
	m_HttpAuthValue = httpAuthValue;

	// 创建连接的metadata信息，并保存
	connection_metadata::ptr metadata_ptr = websocketpp::lib::make_shared<connection_metadata>(con->get_handle(), url);
	m_ConnectionMetadataPtr = metadata_ptr;

	m_WebsocketConnectMode = WebsocketConnectMode::WithHttpAuthorization;

	// 注册连接打开的Handler
	con->set_open_handler(websocketpp::lib::bind(
		&WebsocketClient::OnOpen,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接失败的Handler
	con->set_fail_handler(websocketpp::lib::bind(
		&WebsocketClient::OnFail,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接关闭的Handler
	con->set_close_handler(websocketpp::lib::bind(
		&WebsocketClient::OnClose,
		this,
		&m_WebsocketClient,
		websocketpp::lib::placeholders::_1
	));

	// 注册连接接收消息的Handler
	con->set_message_handler(websocketpp::lib::bind(
		&WebsocketClient::OnMessage,
		this,
		websocketpp::lib::placeholders::_1,
		websocketpp::lib::placeholders::_2
	));

	// 进行连接
	m_WebsocketClient.connect(con);

	m_WebsocketStatus = WebsocketStatus::Connecting;

	// 不需要断开websocket连接
	m_bIsNeedCloseConnection = false;

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	return true;
}

bool WebsocketClient::ReConnect()
{
	if (m_WebsocketConnectMode == WebsocketConnectMode::NoHttpAuthorization)
	{
		return Connect(m_WebsocketUrl);
	}
	else if (m_WebsocketConnectMode == WebsocketConnectMode::WithHttpAuthorization)
	{
		Close();
		return Connect(m_WebsocketUrl, m_HttpAuthKey, m_HttpAuthValue);
	}

	return false;
}

bool WebsocketClient::Close(std::string reason)
{
	m_bIsNeedCloseConnection = true;

	websocketpp::lib::error_code ec;

	if (m_ConnectionMetadataPtr != nullptr)
	{
		int close_code = websocketpp::close::status::normal;
		// 关闭连接
		m_WebsocketClient.close(m_ConnectionMetadataPtr->get_hdl(), close_code, reason, ec);
		if (ec) {
			std::string errorMessage = ec.message();
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
			return false;
		}
		
		std::cout << "关闭Websocket连接成功" << std::endl;
		
		m_ConnectionMetadataPtr = nullptr;
	}

	return true;
}

bool WebsocketClient::SendTextToServer(std::string message, bool isNeedConvertToUtf8)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	websocketpp::lib::error_code ec;

	if (m_ConnectionMetadataPtr != nullptr)
	{
		// 连接发送数据
		if (isNeedConvertToUtf8)
		{
			m_WebsocketClient.send(m_ConnectionMetadataPtr->get_hdl(), ansi_to_utf8(message), websocketpp::frame::opcode::text, ec);
		}
		else
		{
			m_WebsocketClient.send(m_ConnectionMetadataPtr->get_hdl(), message, websocketpp::frame::opcode::text, ec);
		}
		
		if (ec) 
		{
			std::string errorMessage = ec.message();
			std::cout << "> Error sending message: " << ec.message() << std::endl;		
			return false;
		}
		std::cout << "发送数据成功" << std::endl;
	}

	return true;
}

bool WebsocketClient::SendBinartToServer(void * message,int length)
{
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));

	websocketpp::lib::error_code ec;

	if (m_ConnectionMetadataPtr != nullptr)
	{
		// 连接发送数据
		m_WebsocketClient.send(m_ConnectionMetadataPtr->get_hdl(), message,length,websocketpp::frame::opcode::binary, ec);

		if (ec)
		{
			std::string errorMessage = ec.message();
			std::cout << "> Error sending message: " << ec.message() << std::endl;
			return false;
		}
		std::cout << "发送数据成功" << std::endl;
	}

	return true;
}

connection_metadata::ptr WebsocketClient::GetConnectionMetadataPtr()
{
	return m_ConnectionMetadataPtr;
}

void WebsocketClient::OnOpen(client * c, websocketpp::connection_hdl hdl)
{
	m_WebsocketStatus = WebsocketStatus::Open;
	if (m_OnOpenFunc != nullptr)
	{
		m_OnOpenFunc();
	}
}

void WebsocketClient::OnFail(client * c, websocketpp::connection_hdl hdl)
{
	m_WebsocketStatus = WebsocketStatus::Failed;
	if (m_OnFailFunc != nullptr)
	{
		client::connection_ptr con = c->get_con_from_hdl(hdl);
		std::string errorMessage = con->get_ec().message();
		int errorCode = con->get_ec().value();
		m_OnFailFunc(errorCode,errorMessage);
	}
}

void WebsocketClient::OnClose(client * c, websocketpp::connection_hdl hdl)
{
	m_WebsocketStatus = WebsocketStatus::Closed;

	client::connection_ptr con = c->get_con_from_hdl(hdl);
	int closeCode = con->get_remote_close_code();
	std::string closeMessage = websocketpp::close::status::get_string(con->get_remote_close_code());
	std::string closeReson = con->get_remote_close_reason();

	if (m_OnCloseFunc != nullptr)
	{
		m_OnCloseFunc(closeCode,closeMessage);
	}
}

void WebsocketClient::OnMessage(websocketpp::connection_hdl, client::message_ptr msg)
{
	if (msg->get_opcode() == websocketpp::frame::opcode::text)
	{
		std::string message = utf8_to_ansi(msg->get_payload());
		//std::cout << "收到来自服务器的消息：" << message << std::endl;

		if (m_OnMessageFunc != nullptr)
		{
			m_OnMessageFunc(message);
		}	
	}
	else
	{
		std::string message = websocketpp::utility::to_hex(msg->get_payload());
	}
}


std::string WebsocketClient::GetHttpAuthenticationStr(const std::string& strSrc)
{
	size_t length = strSrc.length();
	int block_num = length / 16 + 1;
	//明文
	char* szDataIn = new char[block_num * 16 + 1];
	memset(szDataIn, 0x00, block_num * 16 + 1);
	strcpy(szDataIn, strSrc.c_str());

	//进行进行AES的CBC模式加密
	std::string str = base64_encode((unsigned char*)szDataIn,
		block_num * 16);
	delete[] szDataIn;

	std::string result = "Basic " + str;

	return result;
}

void WebsocketClient::SetOnOpenFunc(OnOpenFunc func)
{
	m_OnOpenFunc = func;
}

void WebsocketClient::SetOnFailFunc(OnFailFunc func)
{
	m_OnFailFunc = func;
}

void WebsocketClient::SetOnCloseFunc(OnCloseFunc func)
{
	m_OnCloseFunc = func;
}

void WebsocketClient::SetMessageFunc(OnMessageFunc func)
{
	m_OnMessageFunc = func;
}

void WebsocketClient::SetReconnectFunc(OnReconnectFunc func)
{
	m_OnReconnectFunc = func;
}

bool WebsocketClient::IsConnect()
{
	if (m_WebsocketStatus == WebsocketStatus::Closed || m_WebsocketStatus == WebsocketStatus::Connecting || m_WebsocketStatus== WebsocketStatus::Failed)
	{
		return false;
	}
	return true;
}

bool WebsocketClient::IsNeedCloseConnection()
{
	return m_bIsNeedCloseConnection;
}

