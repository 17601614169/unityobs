#ifndef NAMEDBTN_H
#define NAMEDBTN_H
#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include "qtcustom_global.h"

enum BtnTypes {
	TYPE_MASK = 0,		// 面具
	TYPE_BKGROUND = 1,	// 背景
	TYPE_DEVICE = 2,	// 设备
	TYPE_QUAILTY = 3,	// 画质
	/**
	* 麦克风
	*/
	TYPE_MKF = 4,
	TYPE_3DMASK = 5,	// 3d面具
};

class QTCUSTOM_EXPORT NamedBtn : public QPushButton {
	Q_OBJECT
public:
	NamedBtn(const QString &name, QWidget *parent);
	~NamedBtn();
	void SetButtonType(BtnTypes type);
	void SetButtonStyle(const QString &style);
	void SetName(const QString & name);
signals:
	void buttonClicked(int);

private:
	BtnTypes btnType;
	QPushButton *m_pButton;
	QLabel *m_pName;
};

#endif // !NAMEDBTN_H
