/* Copyright Dr. Alex. Gouaillard (2015, 2020) */

#ifndef _WEBSOCKETCLIENT_H_
#define _WEBSOCKETCLIENT_H_

#ifdef _MSC_VER
    #ifdef WEBSOCKETCLIENT_EXPORTS
        #define WEBSOCKETCLIENT_API __declspec(dllexport)
    #else
        #define WEBSOCKETCLIENT_API __declspec(dllimport)
    #endif
#else
    #ifndef  __APPLE__
        #define WEBSOCKETCLIENT_API
    #else
        #define WEBSOCKETCLIENT_API __attribute__((visibility("default")))
    #endif
#endif

#include <string>
#include <functional>
#include <future>
#include "nlohmann/json.hpp"
/*
enum ReqStep {
	STEP_NONE = 0,
	STEP_GETROUTERRTPCAPABILITIES = 1,
	STEP_JOIN = 2,
	STEP_CONNECTWEBRTCTRANSPORT = 3,
	STEP_RESTART_ICE = 4,
	STEP_REQUESTCONSUMERKEYFRAME = 5,
	STEP_CHANGEDISPLAYNAME = 6,
	STEP_CLOSEPRODUCER = 7,
	STEP_PAUSEPRODUCER = 8,
	STEP_RESUMEPRODUCER = 9,
	STEP_CREATEWEBRTCTRANSPORT = 10,
	STEP_PRODUCE = 11,
	STEP_PAUSECONSUMER = 12,
	STEP_RESUMECONSUMER = 13,
};
*/

using AsyncWebsocketResponse = std::future<nlohmann::json>;

class WebsocketResponseObserver {
public:
	WebsocketResponseObserver() = default;
	~WebsocketResponseObserver() = default;

	std::future<nlohmann::json> GetFuture() { return promise.get_future(); }

	void OnSuccess(const nlohmann::json &v) { this->promise.set_value(v); }
	void OnFailure(const std::string &error)
	{
		this->promise.set_exception(
			std::make_exception_ptr(error.c_str()));
	}

private:
	std::promise<nlohmann::json> promise;
};

typedef std::unique_ptr<WebsocketResponseObserver> WebsocketResponseObserverPtr;

struct WebsocketRequest {
	nlohmann::json body;
	std::string reqName;
	WebsocketResponseObserverPtr observer;
	WebsocketRequest()
	{
	}	
};

typedef std::shared_ptr<WebsocketRequest> WebsocketRequestPtr;


class WEBSOCKETCLIENT_API WebsocketClient {
public:
    class Listener {
    public:
        virtual ~Listener() = default;
        virtual void onConnected() = 0;
        virtual void onDisconnected() = 0;
	virtual void onConnectedFail(int code) = 0;
	virtual void onRequest(int id, const std::string &method,
			       const nlohmann::json &body) = 0;
	virtual void onNotification(const std::string &method,
				    const nlohmann::json &body) = 0;
    };
public:
    virtual ~WebsocketClient() = default;
    virtual bool Connect(
            const std::string & url,
            const std::string & room,
            const std::string & userid,
            WebsocketClient::Listener * listener) = 0;
    
    virtual bool Disconnect(bool wait) = 0;

    virtual AsyncWebsocketResponse
    Request(const std::string &method, const nlohmann::json &data) = 0;
    virtual bool SendMsgNoResponse(const std::string &method, const nlohmann::json &data)=0;
    virtual void Response( const nlohmann::json &body ) = 0;
};

WEBSOCKETCLIENT_API WebsocketClient * createWebsocketClient(/*int type*/);

#endif
