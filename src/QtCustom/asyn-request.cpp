#include "asyn-request.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QJsonObject>
#include "network/base-request.h"
AsynThread::AsynThread(QObject *parent) : QThread(parent), m_bStop(false) {

}

AsynThread::~AsynThread() {

}

void AsynThread::SetStop(bool stop) {
	m_bStop = stop;
}

void AsynThread::InsertReq(const STReqData &stReqData)
{
	m_mutex.lock();
	m_vecReq.push_back(stReqData);
	m_mutex.unlock();
	if(!isRunning()){
		start();
	}
}

void AsynThread::run()
{
	while ((m_vecReq.size() > 0) && !m_bStop)
	{
		m_mutex.lock();
		STReqData stReqData = m_vecReq[0];
		m_vecReq.pop_front();
		m_mutex.unlock();
		if (stReqData.func == FUNC_NONE)
		{
		    qDebug()<<"operate data type error";
		    return;
		}
		if (stReqData.method==GET || stReqData.method==POST) {
			QNetworkRequest networkReq;
			networkReq.setUrl(QString::fromLocal8Bit(stReqData.url));
			QNetworkAccessManager *pManager = new QNetworkAccessManager;
			HeaderList::iterator iter = stReqData.list.begin();
			while (iter != stReqData.list.end()) {
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
			if (stReqData.func==FUNC_SIGNIN_HEART_BEAT)
				qDebug() << "1";
			if (stReqData.method == GET)
			{
				pNetworkReply = pManager->get(networkReq);
			}
			else
			{
				pNetworkReply =
					pManager->post(networkReq, stReqData.body);
			}
			// 阻塞请求等待返回
			QEventLoop eventLoop;
			connect(pNetworkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
			QTimer::singleShot(10000, &eventLoop, SLOT(quit()));
			eventLoop.exec();
			STRspData stRspData;
			QNetworkReply::NetworkError error = pNetworkReply->error();
			
			if (error != QNetworkReply::NoError)
			{
				if (stReqData.func == FUNC_HEART_BEAT) {
					qDebug() << "heart beat error";
				}
				qDebug() << "step:" << stRspData.func
					<< ",error:" << pNetworkReply->error();
				qDebug()<<"url:"<<pNetworkReply->url().toString().toLocal8Bit().data();
				stRspData.error = pNetworkReply->error();
			}
			else
			{
				if (stReqData.func == FUNC_HEART_BEAT) {
					qDebug() << "heart beat";
				}
				QByteArray strValue = pNetworkReply->readAll();
				stRspData.func = stReqData.func;
				stRspData.len = strValue.size();
				stRspData.recvBytes = strValue;
			}
			emit readyData(stRspData);
			pNetworkReply->deleteLater();
			pManager->deleteLater();
		} else if(stReqData.method==DOWNLOAD){
			std::unique_ptr<NetRequest> pRequest = nullptr;
			pRequest = std::move(NetworkRequestFactory::create(stReqData.task.type));
			QEventLoop eventLoop;
			connect(this, &AsynThread::exitEventLoop, &eventLoop, [&eventLoop]() {
				eventLoop.quit();
			}, Qt::QueuedConnection);
			if (pRequest.get())
			{
				connect(pRequest.get(), &NetRequest::requestFinished, this, &AsynThread::RequestFinishd);
				pRequest->SetTask(stReqData.task);
				pRequest->Start();
			}
			eventLoop.exec();
		}
	}
	quit();
}

void AsynThread::RequestFinishd(bool bSuccess, const QByteArray &bytesContent,
		    const QString &strError)
{
	emit exitEventLoop();
}

AsynReqManager::AsynReqManager(QObject *parent)
	: QObject(parent), m_ptrAsynThread(nullptr)
{
	m_ptrAsynThread = new AsynThread();
	qRegisterMetaType<STRspData>("STRspData");
	QObject::connect(m_ptrAsynThread,
			 SIGNAL(readyData(const STRspData &)), this,
			 SLOT(OnReadyData(const STRspData &)));
}

AsynReqManager::~AsynReqManager()
{
	if (m_ptrAsynThread != nullptr) {
		m_ptrAsynThread->SetStop(true);
		m_ptrAsynThread->wait();
		m_ptrAsynThread->deleteLater();
		m_ptrAsynThread = nullptr;
	}
}

void AsynReqManager::InsertReq(const STReqData &stReqData)
{
	if (m_ptrAsynThread!=nullptr){
		m_ptrAsynThread->InsertReq(stReqData);
	}
}

void AsynReqManager::OnReadyData(const STRspData &stRspData)
{
	bool verify = (stRspData.len == strlen(stRspData.recvBytes));
	if (stRspData.error != 0 || stRspData.len == 0){
		return;
	}
	if (verify) {
		qDebug() << "data:" << stRspData.recvBytes;
		emit readyData(stRspData);
	}
}
