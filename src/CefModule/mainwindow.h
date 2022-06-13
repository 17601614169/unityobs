#ifndef QCEFVIEWTEST_H
#define QCEFVIEWTEST_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "cefwebview.h"
#include <QDialog>
#include <QPainter>
#include "cefmodule_global.h"
class CEFMODULE_EXPORT QCefViewMainWidget : public QMainWindow {
	Q_OBJECT

public:
	QCefViewMainWidget(QWidget *parent = 0);
	~QCefViewMainWidget();

	void toggleScrollbar();
	void destroyCefViewObjInAdvance();

protected Q_SLOTS:
	void onNavigateToUrl();
	void onNavigateGoBack();
	void onNavigateGoForward();
	void onNavigateReload();
	void onNavigateStop();

public:
	bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

protected:
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;

private:
	Ui::QCefViewTestClass ui;
	CefWebView* m_pCefview;
	QPixmap pm;
};

#endif // QCEFVIEWTEST_H
