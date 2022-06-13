#ifndef DOWNLOAD_REQUEST_H
#define DOWNLOAD_REQUEST_H
#include "base-request.h"
#include <memory>
class QFile;
class DownloadRequest : public NetRequest {
	Q_OBJECT
public:
	DownloadRequest(QObject *parent = 0);
	~DownloadRequest();
public slots:
	virtual bool Start() override;
	virtual void OnFinished() override final;
	virtual void OnReadyRead() final;
private:
	std::unique_ptr<QFile> m_pFile;
};

#endif // !DOWNLOAD_REQUEST_H
