#ifndef StringConvert_h
#define StringConvert_h

#include "qtcustom_global.h"
#include "base-define.h"
#include <QString>
#include <QObject>
class QTCUSTOM_EXPORT StringConvert : public QObject {
	Q_OBJECT
public:
	~StringConvert();

	SINGLETON_DEFINE(StringConvert);

	QString GetString(STRINDEX index);

private:
	StringConvert();
};

#endif // !StringConvert_h
