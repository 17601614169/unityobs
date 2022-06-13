#ifndef BASE_REQUEST_H
#define BASE_REQUEST_H

#include <QObject>
#include <memory>
#include <QNetworkReply>
#include "base-define.h"

enum RequestType {
	TYPE_NORMAL = 1,
	TYPE_DOWNLOAD = 2,
};

struct RequestTask {
	RequestType type;
	QString url;
	QString redirectUrl;
	QString file;
};

class QNetworkAccessManager;

class NetRequest : public QObject {
	Q_OBJECT
public:
	NetRequest(QObject *parent = 0);
	~NetRequest();
	NetRequest(const NetRequest &) = delete;
	NetRequest &operator=(const NetRequest &) = delete;
	void SetTask(const RequestTask & task);
Q_SIGNALS:
	void requestFinished(bool, const QByteArray &, const QString &);
public slots:
	virtual bool Start() ;
	virtual void Abort();
	virtual void OnFinished()=0;
	virtual void OnError(QNetworkReply::NetworkError code);

protected:
	RequestTask m_task;
	QString m_strError;
	QNetworkReply* m_pReply;
	QNetworkAccessManager *m_pManager;
};


class NetworkRequestFactory {
public:
	static std::unique_ptr<NetRequest> create(const RequestType &type);
};

#endif // !base_request_h
