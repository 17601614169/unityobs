#include "webengine-widget.h"
#include <QHBoxLayout>
#include <QTextCodec>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QtWebEngineWidgets/QWebEngineSettings>
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineProfile>
#include <QtWebChannel/QWebChannel>
#include <QMessageBox>
#include <QVariant>
#include <QUrl>
#include <QDesktopServices>
#include <QPaintEvent>
#include <qopenglwidget.h>
#include <QWebEngineHistory>
#include <QDebug>
#include "util/base.h"
#include "string-convert.h"
#include "asyn-request.h"
#include "custom-config-ini.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QWebEngineCookieStore>
#include <qdesktopservices.h>
#ifndef OS_WIN
# ifdef _DEBUG
#  pragma comment(lib, "Qt5WebEngined.lib")
#  pragma comment(lib, "Qt5WebEngineWidgetsd.lib")
# else
#  pragma comment(lib, "Qt5WebEngine.lib")
#  pragma comment(lib, "Qt5WebEngineWidgets.lib")
# endif
#endif

WebJsObject *WebJsObject::instance()
{
    static WebJsObject obj;
    return &obj;
}

QString WebJsObject::Invoke(const QString& script)
{
	blog(LOG_INFO,"js Invoke script:%s",script.toStdString().c_str());
	QStringList list = script.split(",");
	if (list.size() > 0) {
		QString method = list[0];
		/**
		* 进入直播间
		*/
		if (method.compare("EnterRoom", Qt::CaseInsensitive) == 0 &&list.size()>=3) {
			if (!list[1].isEmpty()) {
				blog(LOG_INFO, "roomid:%s",list[1].toStdString().c_str());
				emit enterRoomSignal(list[1],list[2]);
			} else {
				QMessageBox::information(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS),
					StringConvert::GetInstance()->GetString(STR_PROCESS_INVALID_ARGV));
			}
		}
		/**
		* 主播明细查询
		*/
		else if (method.compare("QueryAnchor", Qt::CaseInsensitive) == 0&&list.size()>=4) {
			if (!list[1].isEmpty()) {
				emit liveDetailSignal(list[1],list[2],list[3]);
			}
		}
		else if (method.compare("download", Qt::CaseInsensitive) == 0 && list.size() >= 2) {
			STReqData req;
			req.func = FUNC_GET_DOWNLOAD;
			req.method = DOWNLOAD;
			RequestTask task;
			task.url = list[1];
			task.type = TYPE_DOWNLOAD;
			QUrl url(task.url);
			std::string path ;
			QString path_= QFileDialog::getExistingDirectory(nullptr,"download","./");
			if (path_ != nullptr) {
				path = path_.toStdString();
				path += "/";
				CustomConfig::SetDownloadPath(path);
			}
			task.file =QString::fromStdString(path)+ url.fileName();
			req.task = task;
			AsynReqManager::GetInstance()->InsertReq(req);
			
		} else if (method.compare("closeroom", Qt::CaseInsensitive) == 0) {
			emit closeRoom();
		} else if (method.compare("openClientBrowser", Qt::CaseInsensitive) == 0 &&  list.size() >= 2) {
			QString url = list[1];
			if (!url.isEmpty())
				QDesktopServices::openUrl(QUrl(url));
		}
	}
	return QString();
}

WebJsObject::WebJsObject(QObject *parent) : QObject(parent)
{

}

 QWebEngineView* WebEngineView::createWindow(QWebEnginePage::WebWindowType)
 {
	WebEngineWidget *parent = dynamic_cast<WebEngineWidget *>(parentWidget());
        WebEngineView* view = new WebEngineView(parent);
	parent->ResetWebView(view);
        return static_cast<QWebEngineView*>(view);
 }

WebEngineWidget::WebEngineWidget(QWidget* parent /* = nullptr*/)
	: BaseWebWidget(parent)
	, m_pWebEngineView(nullptr)
	,m_pMainLayout(nullptr)
	,m_pChannel(nullptr)
{
	QWebEngineSettings *websetting1 = QWebEngineSettings::defaultSettings();
	websetting1->setAttribute(QWebEngineSettings::PluginsEnabled, true);
 
	QWebEngineSettings *websetting = QWebEngineSettings::globalSettings();
	websetting->setAttribute(QWebEngineSettings::PluginsEnabled, true);
	try {
		blog(LOG_INFO, "create webview");
		m_pWebEngineView = new WebEngineView(this);
		InitWebView(m_pWebEngineView);

		QWebEngineProfile* engineProfile = m_pWebEngineView->page()->profile();
		engineProfile->clearHttpCache();

		QWebEngineCookieStore* cookie = engineProfile->cookieStore();
		cookie->deleteAllCookies();

		QString cachePath  = engineProfile->cachePath();
		QDir cachePathDie(cachePath);
		if(cachePathDie.exists())
		{
			bool rlt=cachePathDie.rmdir(cachePath);
			if(!rlt)
			{
			
			}
		}

	} catch (std::exception e) {
		blog(LOG_INFO,"exception:%s",e.what());
	}
	m_pMainLayout = new QVBoxLayout;
	m_pMainLayout->addWidget(m_pWebEngineView);
	m_pMainLayout->setContentsMargins(0, 0, 0, 0);
	m_pMainLayout->setSpacing(0);
	setLayout(m_pMainLayout);
	blog(LOG_INFO, "WebEngineWidget construct");
}

WebEngineWidget::~WebEngineWidget() {
	if (m_pMainLayout != nullptr) {
		m_pMainLayout->deleteLater();
		m_pMainLayout = nullptr;
	}
	if (m_pWebEngineView != nullptr) {
		m_pWebEngineView->deleteLater();
		m_pWebEngineView=nullptr;
	}
	if (m_pChannel != nullptr) {
		m_pChannel->deleteLater();
		m_pChannel=nullptr;
	}
}

void WebEngineWidget::SetBgTransparent() {
	if (m_pWebEngineView) {
		m_pWebEngineView->setAttribute(Qt::WA_TranslucentBackground);
		m_pWebEngineView->setAttribute(Qt::WA_OpaquePaintEvent, false);
		m_pWebEngineView->page()->setBackgroundColor(Qt::transparent);
	}
}

void WebEngineWidget::ResetWebView(WebEngineView* view) {
	if (m_pWebEngineView) {
		m_pMainLayout->removeWidget(m_pWebEngineView);
		ResetView();
		m_pWebEngineView = view;
		InitWebView(view);
		m_pMainLayout->addWidget(view);
	}
}

void WebEngineWidget::ResetView() {
	if (m_pWebEngineView != nullptr) {
		m_pWebEngineView->deleteLater();
		m_pWebEngineView=nullptr;
	}
	if (m_pChannel != nullptr) {
		m_pChannel->deleteLater();
		m_pChannel=nullptr;
	}
}
void WebEngineWidget::InitWebView(WebEngineView* view) {
	if (view != nullptr)
	{
		view->setContextMenuPolicy(Qt::NoContextMenu);
		view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
		view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
		view->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
		view->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
		view->settings()->setDefaultTextEncoding("utf-8");
		view->page()->profile()->setHttpCacheType(QWebEngineProfile::HttpCacheType::MemoryHttpCache);
		view->page()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
		view->setMinimumSize(1024,600);
		m_pChannel = new QWebChannel(view->page());
		m_pChannel->registerObject("qtclient", WebJsObject::instance());
		QHash<QString, QObject*> retval = m_pChannel->registeredObjects();
		view->page()->setWebChannel(m_pChannel);
		connect(m_pWebEngineView->page(), SIGNAL(loadFinished(bool)), this, SLOT(LoadFinished(bool)));
		view->setAttribute(Qt::WA_TranslucentBackground);
		view->setAttribute(Qt::WA_OpaquePaintEvent, false);
		view->page()->setBackgroundColor(Qt::transparent);
	}
	
}

void WebEngineWidget::Load(const QString& url) {
	if (m_pWebEngineView) {
		m_pWebEngineView->load(QUrl(url));
	}
}

void WebEngineWidget::LoadLocalFile(const QString& file_name) {
	if (m_pWebEngineView) {
		m_pWebEngineView->load(QUrl(QUrl::fromLocalFile(file_name)));
	}
}

void WebEngineWidget::SetUrl(const QString& url) {
	if (m_pWebEngineView) {
		m_pWebEngineView->setUrl(QUrl(url));
	}
}

QString WebEngineWidget::Url() {
	QString strUrl;
	if (m_pWebEngineView) {
		strUrl = m_pWebEngineView->url().toString();
	}
	return strUrl;
}

void WebEngineWidget::Reload() {
	if (m_pWebEngineView) {
		QString url = m_pWebEngineView->url().toString();
		m_pWebEngineView->reload();
	}
}

void WebEngineWidget::ForBack() {
	if (m_pWebEngineView) {
		m_pWebEngineView->history()->back();
	}
}

void WebEngineWidget::ForWard() {
	if (m_pWebEngineView) {
		m_pWebEngineView->history()->forward();
	}
}

bool WebEngineWidget::CallJavaScript(const QString& script,JavaScriptResultCallback* callback) 
{
	if (m_pWebEngineView != nullptr) {
		if (callback != nullptr)
			m_pWebEngineView->page()->runJavaScript(script, *callback);
		else
			m_pWebEngineView->page()->runJavaScript(script);
		return true;
	}
	return false;
}


bool WebEngineWidget::CallJavaScript(const QString& script) {
	bool result = false;
	if (m_pWebEngineView !=nullptr) {
		m_pWebEngineView->page()->runJavaScript(script);
	}
	return true;
}

void WebEngineWidget::LoadStarted() {
	qDebug() << "LoadStarted";
}

void WebEngineWidget::LoadFinished(bool ok) {
	qDebug() << "LoadFinished  ok:" << ok;
}

void WebEngineWidget::LoadError(int error_code) {
	qDebug() << "LoadError error_code:" << error_code;
}

void WebEngineWidget::LinkClicked(const QUrl& url) {
	m_pWebEngineView->load(url);
}

void WebEngineWidget::OnUrlChanged(const QUrl& url) {
	m_pWebEngineView->load(url);
}

QWebEngineView * WebEngineWidget::createWindow(QWebEnginePage::WebWindowType type) {
	return m_pWebEngineView;
 }

WebBrowserWidget::WebBrowserWidget(QWidget* parent)
	: WebEngineWidget(parent) {
	connect(WebJsObject::instance(),&WebJsObject::enterRoomSignal,this,&WebBrowserWidget::enterRoomSignal);
	connect(WebJsObject::instance(),&WebJsObject::liveDetailSignal,this,&WebBrowserWidget::liveDetailSignal);
	connect(WebJsObject::instance(),&WebJsObject::closeRoom,this,&WebBrowserWidget::closeRoom);
}

WebBrowserWidget::~WebBrowserWidget() {

}
