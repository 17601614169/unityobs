// Copyright Dr. Alex. Gouaillard (2015, 2020)

#include "MediasoupWebsocketClientImpl.h"

#include <util/base.h>

#include <iostream>
#include <string>

#define warn(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define info(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define debug(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)
#define error(format, ...) blog(LOG_ERROR, format, ##__VA_ARGS__)

typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

extern bool g_received;

static int ping_count = 0;


MediasoupWebsocketClientImpl::MediasoupWebsocketClientImpl()
	: ios(), work(ios), strand_(ios), m_pSteady_timer(nullptr)
{
	// Set logging to be pretty verbose (everything except message payloads)
	client.set_access_channels(websocketpp::log::alevel::all);
	client.clear_access_channels(websocketpp::log::alevel::frame_payload);
	client.set_error_channels(websocketpp::log::elevel::all);
	// Initialize ASIO
	//client.init_asio(&ios);
	client.init_asio();
	srand((unsigned)time(NULL));

	m_pSteady_timer = new asio::steady_timer(client.get_io_service());
}

MediasoupWebsocketClientImpl::~MediasoupWebsocketClientImpl()
{
	// Disconnect just in case
	Disconnect(false);
}

bool MediasoupWebsocketClientImpl::Connect(const std::string &raw_url,
					   const std::string &raw_room,
					   const std::string &raw_userid,
					   WebsocketClient::Listener *listener)
{
	websocketpp::lib::error_code ec;
	std::string url = sanitizeString(raw_url);
	std::string room = sanitizeString(raw_room);
	std::string userid = sanitizeString(raw_userid);

	this->listener = listener;

	// Reset login flag
	logged = false;
	try {
		// --- Message handler
		client.set_message_handler([=](websocketpp::
						       connection_hdl /* con */,
					       message_ptr frame) {
			std::lock_guard<std::mutex> lock(m_mutex);
			const char *x = frame->get_payload().c_str();		
			last_message_recd_time =
				std::chrono::system_clock::now();
			json msg = json::parse(frame->get_payload());
			
			//bool isRequest = msg.value("request", false);
			bool isResponse = msg.value("response", false);
			//bool isNotification = msg.value("notification", false);

			if (isResponse) { //优先处理response
				handleResponse(msg);
				int count = 0;
				while (!recvs.empty() && count < 100) {
					//处理逻辑线程等待response阻塞时 消息线程积压的request及notification
					//为防止有大量消息堆积时雪崩,暂时每次处理100个堆积消息
					auto &m = recvs.front();
					doMessage(m);
					count++;
					recvs.pop();
				}
			} else { //reqeust 及 notification 先放入接收消息队列
				if (sents.size() > 0) {
					//如果有request在等待response 则先将收到的request或者notifiction消息放入队列
					recvs.push(msg);
				} else {
					doMessage(msg);
				}
			}
			
		});

		// --- Open handler
		client.set_open_handler(
			[=](websocketpp::connection_hdl /* con */) {
				// Logic thread
				try {
					logic_thread = std::thread([&]() {
					info("ios.run thread id: %d, ios: %p",
					     std::this_thread::get_id(), &ios);
						ios.run();	
					});
				} catch (std::exception e) {
				}
				
				// Launch event3
				strand_.post([listener]() {
					listener->onConnected();
				});
				//sendLoginMessage(username, token, room);
				m_pSteady_timer->async_wait(std::bind(
					&MediasoupWebsocketClientImpl::
						handlePongMsgTimeout,
					this, std::placeholders::_1));
				m_pSteady_timer->expires_from_now(
					std::chrono::milliseconds(5000));

			});

		// --- Close handler
		client.set_close_handler(std::bind(
			&MediasoupWebsocketClientImpl::handleDisconnect, this,
			std::placeholders::_1, listener));

		// --- Failure handler
		client.set_fail_handler(
			std::bind(&MediasoupWebsocketClientImpl::handleFail,
				  this, std::placeholders::_1, listener));

		// --- TLS handler
		client.set_tls_init_handler([&](websocketpp::
							connection_hdl /* con */) {
			// Create context
			auto ctx = websocketpp::lib::make_shared<
				asio::ssl::context>(asio::ssl::context::sslv23);
			try {
				// Remove support for undesired TLS versions
				ctx->set_options(
					asio::ssl::context::default_workarounds |
					asio::ssl::context::no_sslv2 |
					asio::ssl::context::no_sslv3 |
					asio::ssl::context::single_dh_use);
			} catch (std::exception &e) {
				warn("TLS exception: %s", e.what());
			}
			return ctx;
		});

		client.set_pong_handler(std::bind(
			&MediasoupWebsocketClientImpl::OnPongMessage, this,
			std::placeholders::_1, std::placeholders::_2));

		client.set_pong_timeout_handler(std::bind(
			&MediasoupWebsocketClientImpl::OnTimeout, this,
			std::placeholders::_1, std::placeholders::_2));
#if _DEBUG
		//peng chong 192.168.1.226:4443
		//guo ying jie 192.168.1.228
		std::string uri2 =
			"wss://192.168.1.208:4443/?roomId=" + room + "&peerId=" + userid;
		std::string uri1 =
			url + "/?roomId=" +"534255"+ "&peerId=" + userid;
		std::string uri =
			url + "/?roomId=" +room+ "&peerId=" + userid;
#else
		std::string uri =
			url + "/?roomId=" +room+ "&peerId=" + userid;
#endif
		
		info("CONNECT PROTO URL: %s", uri.c_str());
		connection = client.get_connection(uri, ec);

		if (ec) {
			error("Error establishing websocket connection: %s|%s", uri.c_str(),
			      ec.message().c_str());
			return 0;
		}
		connection->add_subprotocol("protoo", ec);

		// Note that connect here only requests a connection. No network messages
		// exchanged until the event loop starts running in the next line.
		client.connect(connection);
		// Async
		msg_thread = std::thread([&]() {
			// Start ASIO io_service run loop
			// (single connection will be made to the server)			
			info("client.run thread id: %d, client: %p",
				std::this_thread::get_id(), &client);
			client.run(); // will exit when this connection is closed			
		});
	} catch (const websocketpp::exception &e) {
		warn("connect exception: %s", e.what());
		return false;
	}
	// OK
	return true;
}

void MediasoupWebsocketClientImpl::OnTimeout(websocketpp::connection_hdl hdl,
					     std::string msg)
{
	std::cout << "timeout message: " << msg << std::endl;
}

void MediasoupWebsocketClientImpl::handlePongMsgTimeout(
	const asio::error_code &ec)
{
	strand_.post([this](){
		this->client.ping(connection, "ping");
	});
	ping_count++;
	/**
	  * 只发ping 不做处理
	  */
	m_pSteady_timer->expires_from_now(std::chrono::milliseconds(5000));
	m_pSteady_timer->async_wait(
		std::bind(&MediasoupWebsocketClientImpl::handlePongMsgTimeout,
			  this, std::placeholders::_1));

}


void MediasoupWebsocketClientImpl::keepConnectionAlive(
	WebsocketClient::Listener *listener)
{
	while (is_running.load()) {
		if (connection) {
			if (hasTimedOut()) {
				warn("Connection timed Out.");
				listener->onDisconnected();
				break;
			}
			try {
				//sendKeepAliveMessage();
			} catch (const websocketpp::exception &e) {
				warn("keepConnectionAlive exception: %s",
				     e.what());
			}
		}
	}
}

void MediasoupWebsocketClientImpl::destroy()
{
	if (connection) {
		try {
			//sendDestroyMessage();
		} catch (const websocketpp::exception &e) {
			warn("destroy exception: %s", e.what());
		}
	}
}

bool MediasoupWebsocketClientImpl::Disconnect(bool wait)
{
	if (!connection)
		return true;
	websocketpp::lib::error_code ec;
	try {
		// Stop keepAlive
		if (msg_thread_keepAlive.joinable()) {
			is_running.store(false);
			msg_thread_keepAlive.join();
		}
		// Stop client
		if (connection->get_state() ==
		    websocketpp::session::state::open)
			client.close(connection,
				     websocketpp::close::status::normal,
				     std::string("disconnect"), ec);
		if (ec)
			warn("> Error on disconnect close: %s",
			     ec.message().c_str());

		ios.stop();
		// Don't wait for connection close
		client.stop();
		if (wait && msg_thread.joinable()) {
			msg_thread.join();
		} else {
			// Remove handlers
			client.set_open_handler([](...) {});
			client.set_close_handler([](...) {});
			client.set_fail_handler([](...) {});
			client.reset();			
		}
		if (msg_thread.joinable())
			msg_thread.detach();
		if (logic_thread.joinable())
			logic_thread.detach();
		ios.reset();

	} catch (const websocketpp::exception &e) {
		warn("disconnect exception: %s", e.what());
		return false;
	}
	// OK
	return true;
}

//---------------------------------------------------------
//  WebSocket API Handlers / Callbacks
//---------------------------------------------------------

void MediasoupWebsocketClientImpl::handleDisconnect(
	websocketpp::connection_hdl connectionHdl,
	WebsocketClient::Listener *listener)
{
	UNUSED_PARAMETER(connectionHdl);

	info("> set_close_handler > handleDisconnect called");
	strand_.post([listener]() {
		if (listener) {
			listener->onDisconnected();
		} else {
			info("handleDisconnect listener is null");
		}
	});
	
}

void MediasoupWebsocketClientImpl::handleFail(
	websocketpp::connection_hdl connectionHdl,
	WebsocketClient::Listener *listener)
{
	UNUSED_PARAMETER(connectionHdl);

	info("> set_fail_handler > handleFail called ");
	strand_.post([this, listener]() {
		if (listener) {
			if (this->hasTimedOut()) {
				warn("Connection Timed out");
				listener->onDisconnected();
			} else {
				//listener->onLoggedError(-1);
				listener->onConnectedFail(-1);
			}
		}
	});
}

void MediasoupWebsocketClientImpl::handleRequest(const json &msg)
{
	strand_.post([this, msg]() {
		int id = msg.value("id", 0);
		std::string method = msg.value("method", "");
		this->listener->onRequest(id, method, msg["data"]);
	});
}

void MediasoupWebsocketClientImpl::handleResponse(const json &msg)
{	
	int id = msg.value("id", 0);

	RequestList::iterator reqIt = sents.find(id);
	if (reqIt == sents.end()) {
		error("received response does not match any sent request [id:%d] ", id);
		return;
	}

	bool isOk = msg.value("ok", false);

	auto &req = reqIt->second;
	if (isOk) {
		const nlohmann::json& data = msg["data"];
		req->observer->OnSuccess(data);
	} else {
		auto errorCode = msg.value("errorCode", 0);
		error("received response is not ok for sent request [id:%d] [errorCode:%d]",
		      id, errorCode);
		req->observer->OnFailure("response of the request is not ok.");
	}

	sents.erase(id);
}

void MediasoupWebsocketClientImpl::doMessage(const json& msg)
{
	bool isRequest = msg.value("request", false);
	if (isRequest)
		handleRequest(msg);
	else
		handleNotification(msg);	
}

void MediasoupWebsocketClientImpl::handleNotification(const json &msg)
{
	strand_.post([this, msg]() {
		std::string method = msg.value("method", "");
		this->listener->onNotification( method, msg["data"]);
	});
}

AsyncWebsocketResponse
MediasoupWebsocketClientImpl::Request(const std::string &method,
					   const json &data)
{
	int msgId = rand();
	json body = {{"request", true},
		    {"method", method},
		    {"id", msgId},
		    {"data", data}};

	WebsocketRequestPtr request = std::make_shared<WebsocketRequest>();
	//request.step = STEP_GETROUTERRTPCAPABILITIES;
	request->reqName = method;
	request->body = body;
	request->observer = std::make_unique<WebsocketResponseObserver>();
	sents[msgId] = request;

	sendMessage(request->body, request->reqName.c_str());
	//InsertPacket(request);
	return request->observer->GetFuture();
}

//----------------------------------------
// Helpers
//----------------------------------------

// Helper - Sanitize the streams coming from UI
std::string MediasoupWebsocketClientImpl::sanitizeString(const std::string &s)
{
	std::string _my_s = s;
	size_t p = _my_s.find_first_not_of(" \n\r\t");
	_my_s.erase(0, p);
	p = _my_s.find_last_not_of(" \n\r\t");
	if (p != std::string::npos)
		_my_s.erase(p + 1);
	return _my_s;
}

// Helper - Serialize and send message, with appropriate logging.
bool MediasoupWebsocketClientImpl::sendMessage(json msg, const char *name)
{
	info("MESSAGE SEND:%s\n", msg.dump().c_str());

	// Serialize and send
	if (connection->send(msg.dump())) {
		// this is a console log
		// do we want to double up with a UI error modal window?
		warn("Error sending %s message", name);
		return false;
	}

	return true;
}

// Helper - Websocket layer - Check for timeout
bool MediasoupWebsocketClientImpl::hasTimedOut()
{
	auto current_time = std::chrono::system_clock::now();
	std::chrono::duration<double> gap =
		current_time - last_message_recd_time;
	return gap.count() > 5;
}

void MediasoupWebsocketClientImpl::Response(const nlohmann::json &body)
{
	sendMessage(body, "response");
}

bool MediasoupWebsocketClientImpl::SendMsgNoResponse(const std::string& method, const nlohmann::json& data) {
	bool success = true;

	int msgId = rand();
	json body = {{"request", true},
		    {"method", method},
		    {"id", msgId},
		    {"data", data}};

	success = sendMessage(body,method.c_str());

	return success;
}

void MediasoupWebsocketClientImpl::OnPongMessage(websocketpp::connection_hdl hdl, std::string msg) {
	std::cout << "pong message: " << msg << std::endl;
	if (ping_count % 100 == 0) {
		blog(LOG_INFO,"pong message per 500s:%s",msg.c_str());
	}
}
