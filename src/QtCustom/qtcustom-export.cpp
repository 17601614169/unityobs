#include "qtcustom-export.h"
#include <QDebug>
#include <QMessageBox>
#include <QApplication>
#include "base-define.h"
#include "custom-event.h"

void VedioDeviceNotEnabled(const char* id, const char *name, bool enable)
{
	VedioDeviceDisabledEvent *event = new VedioDeviceDisabledEvent;
	event->m_bEnable = enable;
	event->m_strId = std::string(id);
	event->m_strName = std::string(name);
	QApplication::instance()->postEvent(QApplication::instance(), event);
}
