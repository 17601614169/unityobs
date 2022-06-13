// Copyright Dr. Alex. Gouaillard (2015, 2020)

#include "WebsocketClient.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

//Use http://think-async.com/ insted of boost
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_
#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_FUNCTIONAL_
#define _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_MEMORY_

#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/client.hpp"
#include <mutex>
typedef websocketpp::client<websocketpp::config::asio_tls_client> Client;

class MediasoupWebsocketClientImpl : public WebsocketClient {
public:
	MediasoupWebsocketClientImpl();
	~MediasoupWebsocketClientImpl();

	// WebsocketClient::Listener implementation
	bool Connect(const std::string &url, const std::string &room,
		     const std::string &userid,
		     WebsocketClient::Listener *listener) override;
	bool Disconnect(bool wait) override;

	void keepConnectionAlive(WebsocketClient::Listener *listener);
	void destroy();

	AsyncWebsocketResponse
	Request(const std::string &method, const nlohmann::json &data) override;

public:
	//packet
	void InsertPacket(const WebsocketRequest & req);
	void DoPacket();

	void Response(const nlohmann::json &body) override;

	virtual bool SendMsgNoResponse(const std::string &method, const nlohmann::json &data);
	void OnPongMessage(websocketpp::connection_hdl hdl, std::string msg);
	void OnTimeout(websocketpp::connection_hdl hdl, std::string msg);
	void handlePongMsgTimeout(const asio::error_code &ec);

protected:
	std::chrono::time_point<std::chrono::system_clock>
		last_message_recd_time;

	void handleDisconnect(websocketpp::connection_hdl connectionHdl,
			      WebsocketClient::Listener *listener);
	void handleFail(websocketpp::connection_hdl connectionHdl,
			WebsocketClient::Listener *listener);
	void handleRequest(const json &msg);
	void handleResponse(const json &msg);
	void handleNotification(const json &msg);
	bool hasTimedOut();

private:
	void doMessage(const json &msg);

private:
	bool logged;
	long long session_id;
	long long handle_id;

	Client client;
	Client::connection_ptr connection;
	std::thread msg_thread;
	std::thread msg_thread_keepAlive;
	std::atomic<bool> is_running;

	WebsocketClient::Listener *listener;

	std::string sanitizeString(const std::string &s);
	bool sendMessage(json msg, const char *name);

	typedef std::unordered_map<int, WebsocketRequestPtr> RequestList;
	RequestList sents;
	typedef std::queue<nlohmann::json> MsgQueue;
	MsgQueue recvs;

	std::mutex m_mutex;
	std::thread logic_thread;
	asio::io_service ios;
	asio::io_service::work work;
	asio::io_service::strand strand_;
	asio::steady_timer *m_pSteady_timer;
};
