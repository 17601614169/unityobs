#include "cefmodule-export.h"
#include "QCefViewSDK/include/QCefSetting.h"
namespace CefModule {
void CEFMODULE_EXPORT initializeCef()
{
	QCefSetting::initializeCef();
}
void CEFMODULE_EXPORT uninitializeCef()
{
	QCefSetting::uninitializeCef();
}
bool CEFMODULE_EXPORT setWindowlessRenderingEnabled(bool enabled)
{
	return QCefSetting::setWindowlessRenderingEnabled(enabled);
}
bool CEFMODULE_EXPORT setBrowserSubProcessPath(const QString &path)
{
	return QCefSetting::setBrowserSubProcessPath(path);
}
bool CEFMODULE_EXPORT setLogFilePath(const QString &path)
{
	return QCefSetting::setLogFilePath(path);
}
bool CEFMODULE_EXPORT setResourceDirectoryPath(const QString &path)
{
	return QCefSetting::setResourceDirectoryPath(path);
}
bool CEFMODULE_EXPORT setLocalesDirectoryPath(const QString &path)
{
	return QCefSetting::setLocalesDirectoryPath(path);
}
bool CEFMODULE_EXPORT setUserAgent(const QString &agent)
{
	return QCefSetting::setUserAgent(agent);
}
const CEFMODULE_EXPORT bool windowlessRenderingEnabled()
{
	return QCefSetting::windowlessRenderingEnabled();
}
const CEFMODULE_EXPORT QString browserSubProcessPath()
{
	return QCefSetting::browserSubProcessPath();
}
const CEFMODULE_EXPORT QString logFilePath()
{
	return QCefSetting::logFilePath();
}
const CEFMODULE_EXPORT QString resourceDirectoryPath()
{
	return QCefSetting::resourceDirectoryPath();
}
const CEFMODULE_EXPORT QString localesDirectoryPath()
{
	return QCefSetting::localesDirectoryPath();
}
const CEFMODULE_EXPORT QString userAgent()
{
	return QCefSetting::userAgent();
}
}
