#ifndef GLOBAL_CONFIG_INI_H
#define GLOBAL_CONFIG_INI_H
#include <string>
#include "qtcustom_global.h"

namespace CustomConfig
{
	QTCUSTOM_EXPORT std::string GetGlobalConfigPath();
	QTCUSTOM_EXPORT std::string GetLoginData(const std::string &element);
	QTCUSTOM_EXPORT bool SetLoginData(const std::string &element, const std::string &value);
	QTCUSTOM_EXPORT std::string GetImagePath();
	QTCUSTOM_EXPORT bool SetRoomTitle(const std::string &value);
	QTCUSTOM_EXPORT std::string GetRoomTitle();
	QTCUSTOM_EXPORT void SetOpen(bool open);
	QTCUSTOM_EXPORT int GetOpen();
	QTCUSTOM_EXPORT void SetMaskPath(const std::string &path);
	QTCUSTOM_EXPORT std::string GetMaskPath();
	QTCUSTOM_EXPORT void SetBackgroundPath(const std::string &path);
	QTCUSTOM_EXPORT std::string GetBackgroundPath();
	QTCUSTOM_EXPORT std::string GetDownloadPath();
	QTCUSTOM_EXPORT void SetDownloadPath(const std::string &path);
	QTCUSTOM_EXPORT void SetMask3DType(int type);
	QTCUSTOM_EXPORT int GetMask3DType();
	};

#endif // !GLOBAL_CONFIG_INI_H
