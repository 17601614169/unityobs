#include "network-utility.h"
#include "../md5.h"
#include "../custom-config-ini.h"

namespace NetworkUtility {
bool FileExists(QFile *pFile)
{
	return (pFile != nullptr && pFile->exists());
}

bool RemoveFile(const QString &strFilePath, QString &errMessage)
{
	QFile file(strFilePath);
	if (FileExists(&file)) {
		file.close();
		if (!file.remove()) {
			errMessage = file.errorString();
			return false;
		}
	}
	return true;
}

bool FileOpened(QFile *pFile)
{
	return (nullptr != pFile && pFile->exists() && pFile->isOpen());
}

std::unique_ptr<QFile> CreateAndOpenFile(const RequestTask &task,
					 QString &errMessage)
{
	MD5 md5(task.url.toStdString().c_str());
	std::string name = md5.toString();
	std::string imagePath = CustomConfig::GetImagePath();
	std::string path ;
	if (task.file.isEmpty())
		path= imagePath + "/" + name;
	else
		path = task.file.toStdString();
	std::unique_ptr<QFile> pFile = std::make_unique<QFile>(path.c_str());
	if (pFile.get()) {
		if (!pFile->open(QIODevice::ReadWrite)) {
			pFile->reset();
		}
	}
	return pFile;
}

QString GetDownloadFileSaveName(const RequestTask &)
{
	return QString("");
}
};
