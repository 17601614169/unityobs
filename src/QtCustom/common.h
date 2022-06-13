#pragma once

#include <QString>
#include <QIcon>
#include <QColor>
#include "qtcustom_global.h"
#include "base-define.h"

enum SourceType {
	SOURCE_CAMERA = 0,
	SOURCE_PICTURE,
	SOURCE_TEXT,
	SOURCE_WINODW,
	SOURCE_SCREEN,
	SOURCE_MEDIA
};

enum OBSPixmap {
	PIXMAP_TRAY_ICON,
};

class QTCUSTOM_EXPORT common {
public:
	static QString GetSourceIcon(const char *id);
	static QString GetSourceId(SourceType sourcetype);
	static const char *GetSourceDisplayNameFromId(const char *id);
	static QPixmap GetPixmap(OBSPixmap pix);
	static long long color_to_int(QColor color);
	static QColor color_from_int(long long val);
	static QString GetFilterPath(Filter_Type type);
};
