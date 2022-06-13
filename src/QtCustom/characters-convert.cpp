#include "characters-convert.h"
#include <wchar.h>
#include <windows.h>

namespace CharatersConvert {
	std::string UTF8ToGBK(const std::string &strUtf8)
	{
		std::string strOutGBK = "";
		int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
		WCHAR *wszGBK = new WCHAR[len + 1];
		memset(wszGBK, 0, len * 2 + 2);
		MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		char *pszGBK = new char[len + 1];
		memset(pszGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, pszGBK, len, NULL, NULL);
		strOutGBK = pszGBK;
		delete[] pszGBK;
		delete[] wszGBK;
		return strOutGBK;
	}

	std::string ACPToUTF8(const std::string &str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1,
						  NULL, 0);
		wchar_t *pwBuf =
			new wchar_t[nwLen + 1]; 
		ZeroMemory(pwBuf, nwLen * 2 + 2);

		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(),
				      pwBuf, nwLen);

		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL,
						 NULL, NULL, NULL);
		char *pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen,
				      NULL, NULL);

		std::string retStr(pBuf);
		delete[] pwBuf;
		delete[] pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}

	std::wstring StringToWString(std::string str)
	{
		std::wstring result;
		int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(),
					      str.size(), NULL, 0);
		TCHAR *buffer = new TCHAR[len + 1];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer,
				    len);
		buffer[len] = '\0';
		result.append(buffer);
		delete[] buffer;
		return result;
	}

	std::string WStringToString(std::wstring wstr)
	{
		std::string result;
		int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(),
					      wstr.size(), NULL, 0, NULL, NULL);
		char *buffer = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(),
				    buffer, len, NULL, NULL);
		buffer[len] = '\0';
		result.append(buffer);
		delete[] buffer;
		return result;
	}
	};
