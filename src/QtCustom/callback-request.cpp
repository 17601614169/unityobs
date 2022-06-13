#include "callback-request.h"
#include <QEventLoop>
#include <QTimer>
#include "util/base.h"

CallbackThread::CallbackThread(const STReqData &reqData,
			       const ReqCallback &call_back, QObject *parent)
	: QThread(parent)
{
	m_stReqData = reqData;
	m_callback = call_back;
	this->start();
}

CallbackThread::CallbackThread(const ReqCallbackFill &call_back,
			       const STReqData &reqData,
			       QObject *parent)
	: QThread(parent)
{
	m_stReqData = reqData;
	m_callbackFill = call_back;
	this->start();
}

void CallbackThread::SetReqData(const STReqData &reqData) {
	m_stReqData = reqData;
	if (!isRunning()) {
		start();
	}
}

CallbackThread::~CallbackThread() {

}

void CallbackThread::run() {
	if (m_stReqData.func == FUNC_NONE) {
		qDebug() << "operate data type error";
		return;
	}
	QNetworkRequest networkReq;
	networkReq.setUrl(QString::fromLocal8Bit(m_stReqData.url));
	QNetworkAccessManager *pManager = new QNetworkAccessManager;
	HeaderList::iterator iter = m_stReqData.list.begin();
	while (iter != m_stReqData.list.end()) {
		networkReq.setRawHeader((*iter).first, (*iter).second);
		iter++;
	}
	QNetworkReply *pNetworkReply = nullptr;
	QString strUrl = networkReq.url().toString();
	if (strUrl.contains("https")) {
#ifndef QT_NO_SSL
		QSslConfiguration conf = networkReq.sslConfiguration();
		conf.setPeerVerifyMode(QSslSocket::VerifyNone);
		conf.setProtocol(QSsl::TlsV1SslV3);
		networkReq.setSslConfiguration(conf);
#endif
	}
	if (m_stReqData.method == GET) {
		pNetworkReply = pManager->get(networkReq);
	} else {
		pNetworkReply = pManager->post(networkReq, m_stReqData.body);
	}
	// 阻塞请求等待返回
	QEventLoop eventLoop;
	connect(pNetworkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
	QTimer::singleShot(10000, &eventLoop, SLOT(quit()));
	eventLoop.exec();
	STRspData stRspData;
	QByteArray strValue;
	QNetworkReply::NetworkError error = pNetworkReply->error();
	if (error != QNetworkReply::NoError) {
		qDebug() << "step:" << stRspData.func
			 << ",error:" << pNetworkReply->error();
		qDebug()
			<< "url:"
			<< pNetworkReply->url().toString().toLocal8Bit().data();
	} else {
		strValue = pNetworkReply->readAll();
	}
	if (m_callback != nullptr) {
		blog(LOG_INFO, "response %s", pNetworkReply->url().toString().toLocal8Bit().data());
		m_callback(strValue, error);
	}
	if (m_callbackFill != nullptr) {
		QVariant var;
		var.setValue(m_stReqData);
		m_callbackFill(strValue, error, var);
	}
	pNetworkReply->deleteLater();
	pManager->deleteLater();
	quit();
}
