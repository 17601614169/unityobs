#include "download-request.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include "network-utility.h"
DownloadRequest::DownloadRequest(QObject *parent)
	: NetRequest(parent), m_pFile(nullptr)
{
}

DownloadRequest::~DownloadRequest() {

}

bool DownloadRequest::Start() {
	QString strError;
	m_pFile = std::move(NetworkUtility::CreateAndOpenFile(m_task, strError));
	if (!m_pFile.get()) {
		return false;
	}
	if (m_pManager == nullptr)
		m_pManager = new QNetworkAccessManager();
	QNetworkRequest request;
	request.setUrl(m_task.redirectUrl.isEmpty() ? QUrl(m_task.url) : QUrl(m_task.redirectUrl));
	m_pReply = m_pManager->get(request);
	connect(m_pReply, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
	connect(m_pReply, SIGNAL(finished()), this, SLOT(OnFinished()));
	connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)), this,
		SLOT(OnError(QNetworkReply::NetworkError)));
	return true;
}

void DownloadRequest::OnFinished() {
	bool bSuccess = (m_pReply->error() == QNetworkReply::NoError);
	int statusCode =
		m_pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute)
			.toInt();
	QUrl url(m_task.url);
	Q_ASSERT(url.isValid());

	if (NetworkUtility::isHttpProxy(url.scheme()) ||
	    NetworkUtility::isHttpsProxy(url.scheme())) {
		bSuccess = bSuccess && (statusCode >= 200 && statusCode < 300);
	}
	if (!bSuccess) {
		if (statusCode == 301 || statusCode == 302) { //301,302重定向
			const QVariant &redirectionTarget = m_pReply->attribute(
					QNetworkRequest::
						RedirectionTargetAttribute);
			const QUrl &redirectUrl =
				url.resolved(redirectionTarget.toUrl());
			if (redirectUrl.isValid() && url != redirectUrl) {
				m_task.redirectUrl = redirectUrl.toString();
				qDebug() << "[QMultiThreadNetwork] url:"
					 << url.toString() << "redirectUrl:" << m_task.redirectUrl;

				m_pReply->deleteLater();
				m_pReply = nullptr;

				//重定向需要关闭之前打开的文件
				if (NetworkUtility::FileExists(m_pFile.get())) {
					m_pFile->close();
					m_pFile->remove();
				}
				m_pFile.reset();
				Start();
				return;
			}
		} else if ((statusCode >= 300 || statusCode < 200) &&
			   statusCode != 0) {
			qDebug() << "[QMultiThreadNetwork] HttpStatusCode:"
				 << statusCode;
		}
	}

	if (NetworkUtility::FileExists(m_pFile.get())) {
		m_pFile->close();
		if (!bSuccess) {
			m_pFile->remove();
		}
	}
	m_pFile->close();
	m_pFile.reset();
	emit requestFinished(bSuccess, QByteArray(), m_strError);
	m_pReply->deleteLater();
	m_pReply = nullptr;
}

void DownloadRequest::OnReadyRead() {
	if (m_pReply && m_pReply->error() == QNetworkReply::NoError &&
	    m_pReply->isOpen()) {
		if (NetworkUtility::FileOpened(m_pFile.get())) {
			const QByteArray &bytesRev = m_pReply->readAll();
			if (!bytesRev.isEmpty()) {
				int n = m_pFile->write(bytesRev);
				qDebug() << "[DownloadRequest]"
					 << m_pFile->errorString();
			}
		}
	}
}
