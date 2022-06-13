#ifndef CEFMODULE_EXPORT_H
#define CEFMODULE_EXPORT_H
#include "cefmodule_global.h"
namespace CefModule {
void CEFMODULE_EXPORT initializeCef();
void CEFMODULE_EXPORT uninitializeCef();
bool CEFMODULE_EXPORT setWindowlessRenderingEnabled(bool);
bool CEFMODULE_EXPORT setBrowserSubProcessPath(const QString &path);
bool CEFMODULE_EXPORT setLogFilePath(const QString &path);
bool CEFMODULE_EXPORT setResourceDirectoryPath(const QString &path);
bool CEFMODULE_EXPORT setLocalesDirectoryPath(const QString &path);
bool CEFMODULE_EXPORT setUserAgent(const QString &agent);
const CEFMODULE_EXPORT bool windowlessRenderingEnabled();
const CEFMODULE_EXPORT QString browserSubProcessPath();
const CEFMODULE_EXPORT QString logFilePath();
const CEFMODULE_EXPORT QString resourceDirectoryPath();
const CEFMODULE_EXPORT QString localesDirectoryPath();
const CEFMODULE_EXPORT QString userAgent();
}
#endif // !cefmodule_export_h
