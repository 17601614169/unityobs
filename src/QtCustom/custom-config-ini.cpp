#include "custom-config-ini.h"
#include "characters-convert.h"

#include <Shlwapi.h>
#include <windows.h>
#include <shellapi.h>
#include <ShlObj.h>
#include <QDir>
#include <QDebug>
#pragma comment(lib, "shlwapi.lib")

const std::string INI_FILE = "yl-custom-service/custom-global.ini";

namespace CustomConfig
{
	std::string GetGlobalConfigPath()
	{
		wchar_t path_utf16[MAX_PATH] = {0};
		SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT,
			path_utf16);
		::PathAppendW(path_utf16, L"yl-custom-service/custom-global.ini");
		std::string path = CharatersConvert::WStringToString(
			std::wstring(path_utf16));
		return path;
	}

	std::string GetImagePath() {
		wchar_t path_utf16[MAX_PATH] = {0};
		SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT,
				 path_utf16);
		::PathAppendW(path_utf16, L"yl-custom-service/images");
		std::string path = CharatersConvert::WStringToString(
			std::wstring(path_utf16));
		QDir dir;
		if (!dir.exists(QString::fromStdString(path))) {
			qDebug() << "path not exist";
			bool bmake=dir.mkpath(QString::fromStdString(path));
			if (bmake) {
				qDebug()<< "path make :" <<QString::fromStdString(path);
			} else {
				qDebug() << "path make failed:" << GetLastError();
			}
		}
		return path;
	}

	QTCUSTOM_EXPORT bool SetRoomTitle(const std::string &value)
	{
		return (WritePrivateProfileStringA("login", "room_title", value.c_str(), GetGlobalConfigPath().c_str())==TRUE);
	}

	QTCUSTOM_EXPORT std::string GetRoomTitle()
	{
		char retData[128] = {0};
		GetPrivateProfileStringA("login", "room_title", "", retData, _countof(retData), GetGlobalConfigPath().c_str());
		return std::string(retData);
	}

	std::string GetLoginData(const std::string& element) {
		char retData[4096] = {0};
		GetPrivateProfileStringA("login",
			element.c_str(),
			"", retData,
			_countof(retData),
			GetGlobalConfigPath().c_str());
		return std::string(retData);
	}

	bool SetLoginData(const std::string &element,
		const std::string &value)
	{
		return (WritePrivateProfileStringA("login",
			element.c_str(),
			value.c_str(),
			GetGlobalConfigPath().c_str())==TRUE);
	}

	void SetOpen(bool open) {
		char data[16] = {0};
		if (open) {
			sprintf(data, "1");
		} else {
			sprintf(data, "0");
		}
		WritePrivateProfileStringA("Secret", "open", data,
					   GetGlobalConfigPath().c_str());
	}
	int GetOpen() {
		int open = GetPrivateProfileIntA("Secret", "open", 1,
						 GetGlobalConfigPath().c_str());
		return open;
	}
	void SetMaskPath(const std::string &path)
	{
		WritePrivateProfileStringA("Path", "mask_path", path.c_str(), GetGlobalConfigPath().c_str());
	}
	std::string GetMaskPath()
	{
		char retData[1024] = {0};
		GetPrivateProfileStringA("Path", "mask_path", "", retData, _countof(retData), GetGlobalConfigPath().c_str());
		return std::string(retData);
	}
	std::string GetDownloadPath()
	{
		char retData[1024] = {0};
		GetPrivateProfileStringA("Path", "download", "", retData, _countof(retData), GetGlobalConfigPath().c_str());
		return std::string(retData);
	}

	void SetDownloadPath(const std::string &path)
	{
		WritePrivateProfileStringA("Path", "download", path.c_str(), GetGlobalConfigPath().c_str());
	}

	void SetBackgroundPath(const std::string &path)
	{
		WritePrivateProfileStringA("Path", "background_path", path.c_str(), GetGlobalConfigPath().c_str());
	}
	std::string GetBackgroundPath()
	{
		char retData[1024] = {0};
		GetPrivateProfileStringA("Path", "background_path", "", retData, _countof(retData), GetGlobalConfigPath().c_str());
		return std::string(retData);
	}
	void SetMask3DType(int type) {
		char data[16] = {0};
		sprintf(data, "%d", type);
		WritePrivateProfileStringA("Mask3d", "type", data, GetGlobalConfigPath().c_str());
	}
	int GetMask3DType() {
		int type = GetPrivateProfileIntA("Mask3d", "type", -2, GetGlobalConfigPath().c_str());
		return type;
	}
	};
