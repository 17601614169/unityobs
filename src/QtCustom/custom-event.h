#ifndef OBS_EVENT_H
#define OBS_EVENT_H
#include <QEvent>
#include <map>
#include <QDebug>
#include "qtcustom_global.h"
#define VedioEnabledEventType (QEvent::User+ 1)
#define UserInfoEventType (QEvent::User + 2)
#define UserTitleEventType (QEvent::User + 3)
#define UserMoveSceneItemType (QEvent::User + 4)
class QTCUSTOM_EXPORT VedioDeviceDisabledEvent : public QEvent {
public:
	VedioDeviceDisabledEvent() : QEvent((QEvent::Type)VedioEnabledEventType)
	{
		m_bEnable = false;
	}
	bool m_bEnable;
	std::string m_strId;
	std::string m_strName;
};

class QTCUSTOM_EXPORT UserInfoEvent : public QEvent {
public:
	UserInfoEvent() : QEvent((QEvent::Type)UserInfoEventType)
	{
		
	}
	std::string head;
};

class QTCUSTOM_EXPORT UserTitleEvent : public QEvent {
public:
	UserTitleEvent() : QEvent((QEvent::Type)UserTitleEventType) {}
	std::string title;
};

class QTCUSTOM_EXPORT UserMoveSceneItemEvent : public QEvent {
public:
	UserMoveSceneItemEvent() : QEvent((QEvent::Type)UserMoveSceneItemType) {
		x = 0.0;
		y = 0.0;
		hide = false;
	}
	float x;
	float y;
	std::string id;
	std::string name;
	bool hide;
};
#endif // !OBS_EVENT_H
