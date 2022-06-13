#pragma once
#include <qwidget.h>
#include "qtcustom_global.h"
#include "webengine-widget.h"
#include <QLineEdit>
#include <QPushButton>
#include <qtextedit.h>
class QTCUSTOM_EXPORT TestUrlWidget : public QWidget {
	Q_OBJECT
public:
	TestUrlWidget(QWidget* parent=nullptr);
	~TestUrlWidget();

public:
	void LoadUrl(const QString & url);
	void AppendYlUrl(const QString & url);
private:
	QLineEdit* m_pLineEdit;
	QTextEdit* m_pYlUrlEdit;
	QTextEdit* m_pExampleUrlEdit;
	QPushButton* m_pLoadBtn;
	WebBrowserWidget* m_pBrowser;
};
