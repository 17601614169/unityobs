#pragma once

#include <QDialog>
#include "qtcustom_global.h"
#include "translucent-widget.h"

class QCheckBox;
class QButtonGroup;

enum SET_TYPE {
	TYPE_MIN_TASKBAR = 0,
	TYPE_MIN_TRAY,
	TYPE_CLOSE_EXIT,
	TYPE_CLOSE_TRAY,
	TYPE_UPDATE,
	TYPE_CLOSE_WINDOW
};

class QTCUSTOM_EXPORT SettingDialog : public TranslucenWidget {
	Q_OBJECT
public:
	explicit SettingDialog(QWidget *parent = nullptr);
	virtual ~SettingDialog() override;

	void SetStatus(bool bAuto, bool bMinToTray, bool bCloseToTray);

signals:
	void SettingButtonClick(int id);
	void AutoStart(bool bstart);

private:
	QCheckBox *m_pStartup;
	QButtonGroup *m_pMinGroup;
	QButtonGroup *m_pCloseGroup;
};
