#ifndef CHARACTERS_CONVERT_H
#define CHARACTERS_CONVERT_H
#include "qtcustom_global.h"
#include <string>

namespace CharatersConvert {
	QTCUSTOM_EXPORT std::string UTF8ToGBK(const std::string &strUtf8);
	QTCUSTOM_EXPORT std::string ACPToUTF8(const std::string &str);
	QTCUSTOM_EXPORT std::wstring StringToWString(std::string str);
	QTCUSTOM_EXPORT std::string WStringToString(std::wstring wstr);
};

#endif // !characters_convert_h
