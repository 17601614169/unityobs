/*****************************************************************************
*                                                                            *
*  @file     translucen-widget.h                                             *
*  @brief    基类:半透明不规则底层窗口                                       *
*  Details.                                                                  *
*                                                                            *
*  @author   huchenzhi                                                       *
*  @version  1.0.0.1(版本号)                                                 *
*  @license  GNU General Public License (GPL)                                *
*----------------------------------------------------------------------------*
*  Remark         : Description                                              *
*  2020/10/27								     *
*----------------------------------------------------------------------------*
*****************************************************************************/

#ifndef TranslucenWidget_h
#define TranslucenWidget_h

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include "qtcustom_global.h"

class QTCUSTOM_EXPORT TranslucenWidget : public QDialog {
	Q_OBJECT
public:
	explicit TranslucenWidget(QWidget *parent = nullptr);
	virtual ~TranslucenWidget(){};

	TranslucenWidget(const TranslucenWidget &) = delete;
	TranslucenWidget &operator=(const TranslucenWidget &) = delete;
	void SetPixmap(const QString &strPixmap);
	void SetColor(const QString &color,int radius,bool border=false,int w=0,QString strColor="");
	void SetNinePixmap(const QString& picName,int iHorzSplit,int iVertSplit, int DstWidth, int DstHeight);

private:
	QPixmap* NinePatch(const QString& picName,int iHorzSplit,int iVertSplit, int DstWidth, int DstHeight);

protected:
	virtual void paintEvent(QPaintEvent *event);

public:
	QPixmap pixmap;
	int m_nRadius;
};

#endif
