#ifndef NETWORK_UTILITY_H
#define NETWORK_UTILITY_H
#include <memory>
#include <QString>
#include <QFile>
struct RequestTask;
class QUrl;
#include "base-request.h"
namespace NetworkUtility {

	bool FileExists(QFile *pFile);
	bool RemoveFile(const QString &strFilePath, QString &errMessage);
	std::unique_ptr<QFile> CreateAndOpenFile(const RequestTask &, QString &errMessage);
	QString GetDownloadFileSaveName(const RequestTask &);
	bool FileOpened(QFile *pFile);

	inline bool isHttpProxy(const QString& strScheme) { return (strScheme.compare(QLatin1String("http"), Qt::CaseInsensitive) == 0); }
	inline bool isHttpsProxy(const QString& strScheme) { return (strScheme.compare(QLatin1String("https"), Qt::CaseInsensitive) == 0); }
	inline bool isFtpProxy(const QString& strScheme) { return (strScheme.compare(QLatin1String("ftp"), Qt::CaseInsensitive) == 0); }
}

#endif // !network_utility_h
