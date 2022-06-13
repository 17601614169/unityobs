#ifndef CEF_VIEW_WIDGET
#define CEF_VIEW_WIDGET
#include <QDialog>
class QCefView;
class QHBoxLayout;
#include "cefmodule_global.h"
class CEFMODULE_EXPORT CefViewWidget : public QDialog {
	Q_OBJECT
public:
	CefViewWidget(const QString &url,QWidget* parent=0);
	~CefViewWidget();

public:
	void Load(const QString& url);
private:
	QCefView *m_pCefView;
	QHBoxLayout *m_pLayout;
};

#endif // !CEF_VIEW_WIDGET
