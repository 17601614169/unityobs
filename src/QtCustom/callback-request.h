#pragma once
#include <QObject>
#include <QThread>
#include <functional>
#include <QtNetwork/QNetworkReply>
#include "asyn-request.h"
#include "qtcustom_global.h"
using ReqCallback = std::function<void(const QByteArray&,const QNetworkReply::NetworkError &)>;
using ReqCallbackFill = std::function<void(const QByteArray&,const QNetworkReply::NetworkError &,const QVariant&)>;
class QTCUSTOM_EXPORT CallbackThread : public QThread {
	Q_OBJECT
public:
	CallbackThread(const STReqData& reqData,const ReqCallback &call_back,
		       QObject *parent = 0);
	CallbackThread(const ReqCallbackFill &call_back,
		       const STReqData &reqData,
		       QObject *parent = 0);
	~CallbackThread();

	void SetReqData(const STReqData &reqData);

protected:
	void run();

private:
	STReqData m_stReqData;
	ReqCallback m_callback;
	ReqCallbackFill m_callbackFill;
};
