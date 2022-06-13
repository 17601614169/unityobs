/**===========================================================================
  Copyright (C) yl tech All rights reserved.

  @file     bottomCtrls.cpp
  @brief    底部按钮功能以及带命名的按钮
  @author   huchenzhi
  @version  r0.1
  @date     2020-10-24
  @license  
============================================================================*/
#ifndef bottomCtrls_h
#define bottomCtrls_h

#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include "qtcustom_global.h"
#include "named-btn.h"
#include "custom-slider.h"
#include "base-define.h"
#include "base-widget.h"
#include <QTimer>
class QTCUSTOM_EXPORT BottomCtrls : public QWidget {
	Q_OBJECT
public:
	BottomCtrls(QWidget *parent = Q_NULLPTR);
	~BottomCtrls();
	QSlider* GetMicSlider();
	void HideMicSlider();
	void ResetMicSliderPosition();
signals:
	void buttonSignal(int);
	void AddSourceClicked(QString type);
	void BeautyClicked();
protected:
	bool eventFilter(QObject *obj, QEvent *event);
public slots:
	void OnButtonGroupSlot(int);

private:
	QButtonGroup *m_pButtonGroup;
	NamedBtn *m_pMaskBtn;
	NamedBtn *m_p3dMaskBtn;
	NamedBtn *m_pBkGroundBtn;
	NamedBtn *m_pDeviceBtn;
	NamedBtn *m_pQualityBtn;

	NamedBtn *m_pMkfBtn;

	bool m_bEnterMic;
	QTimer *m_pPosTimer;
	CustomSlider *m_pMicSliderContext;
};
#endif
