#include "base-request.h"
#include "download-request.h"
NetRequest::NetRequest(QObject *parent)
	: QObject(parent), m_pReply(nullptr), m_pManager(nullptr)
{

}

NetRequest::~NetRequest() {

}

bool NetRequest::Start() {
	return true;
}

void NetRequest::Abort() {
	if (m_pReply != nullptr) {
		m_pReply->abort();
		m_pReply->deleteLater();
		m_pReply = nullptr;
	}
}

void NetRequest::OnError(QNetworkReply::NetworkError code) {
	if (m_pReply != nullptr) {
		m_strError = m_pReply->errorString();
	}
}


void NetRequest::SetTask(const RequestTask &task) {
	m_task = task;
}




std::unique_ptr<NetRequest> NetworkRequestFactory::create(const RequestType &type)
{
	std::unique_ptr<NetRequest> pRequest;
	switch (type) {
	case TYPE_NORMAL:
		//pRequest = std::make_unique<>();
		break;
	case TYPE_DOWNLOAD:
		pRequest = std::make_unique<DownloadRequest>();
		break;
	}
	return pRequest;
}
