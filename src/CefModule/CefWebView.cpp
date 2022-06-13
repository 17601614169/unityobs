
#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QPainter>
#include <QDesktopServices>
#include "CefWebView.h"


CefWebView::CefWebView(QWidget *parent /* = 0 */, bool osr /* = false */)
	: QCefView("", parent, osr)
{
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	if(osr)
	{
		setAttribute(Qt::WA_TranslucentBackground, true);
	}
}

CefWebView::~CefWebView()
{
	stop();
}

void CefWebView::load(const QString& url)
{
	qDebug() << "QxCefView::load - url:" << url;

	if (browserIsLoading())
	{
		browserStopLoad();
	}
	navigateToUrl(url);
}

void CefWebView::reload()
{
	if (browserIsLoading())
	{
		browserStopLoad();
	}
	browserReload();
}

bool CefWebView::isLoading()
{
	return browserIsLoading();
}

void CefWebView::stop()
{
	browserStopLoad();
}

void CefWebView::back()
{
	if (browserCanGoBack())
	{
		browserGoBack();
	}
}

void CefWebView::forward()
{
	if (browserCanGoForward())
	{
		browserGoForward();
	}
}

void CefWebView::callJsEvent(const QString& eventName, const QCefEvent& event)
{
	broadcastEvent(eventName, event);
}

void CefWebView::execJaveScript(const QString& js)
{
	__super::execJaveScript(js);
}

void CefWebView::setScrollBarPolicy(Qt::Orientation orientation, Qt::ScrollBarPolicy policy)
{
	__super::setScrollBarPolicy(orientation, policy);
}

void CefWebView::setLinkOpenPolicy(QCefView::LinkOpenPolicy policy)
{
	__super::setLinkOpenPolicy(policy);
}

void CefWebView::onLoadStart(const QString& url, bool bIsMainFrame)
{
	__super::onLoadStart(url, bIsMainFrame);
	emit loadStarted(url, bIsMainFrame);
}

void CefWebView::onLoadEnd(int httpStatusCode, bool bIsMainFrame)
{
	__super::onLoadEnd(httpStatusCode, bIsMainFrame);
	emit loadFinished(httpStatusCode, bIsMainFrame);

	if (httpStatusCode != 200 && httpStatusCode != 0)
	{
		qDebug() << __FUNCTION__ << " bIsMainFrame:" << bIsMainFrame << " httpStatusCode:" << httpStatusCode;
	}
}

void CefWebView::onLoadError(int errorCode, const QString& errorMsg, const QString& failedUrl, bool bIsMainFrame)
{
	qDebug() << __FUNCTION__ << " - errorCode:" << errorCode
		<< "bIsMainFrame:" << bIsMainFrame
		<< "errorMsg:" << errorMsg
		<< "failedUrl:" << failedUrl;

	__super::onLoadError(errorCode, errorMsg, failedUrl, bIsMainFrame);
	emit loadError(errorCode, errorMsg, failedUrl, bIsMainFrame);
}

void CefWebView::onRenderProcessTerminated(int exitCode)
{
	emit renderProcessTerminated(exitCode);
}

void CefWebView::onQCefUrlRequest(const QString& url)
{
	qDebug() << __FUNCTION__ << url;
}

void CefWebView::onQCefQueryRequest(const QCefQuery& query)
{
	qDebug() << __FUNCTION__ << query.reqeust();

	query.setResponseResult(true, query.reqeust());
	responseQCefQuery(query);
}

void CefWebView::onInvokeMethodNotify(int browserId, 
	int frameId, 
	const QString& method, 
	const QVariantList& arguments)
{
	qDebug() << __FUNCTION__ << method << arguments;
	
	emit signalJsCallCxx(method, arguments);
}


//////////////////////////////////////////////////////////////////////////

QxCefViewContainer::QxCefViewContainer(QWidget *parent, bool bNeedToolBar)
	: QWidget(parent),
	m_pCefView(nullptr),
	m_pBtnRefresh(nullptr),
	m_pBtnClose(nullptr),
	m_bNeedToolBar(bNeedToolBar),
	m_bIsLeftPress(false),
	m_ptLastPos(QPoint(0, 0))
{
	setWindowFlags(windowFlags() | Qt::Tool | Qt::FramelessWindowHint);
	setContextMenuPolicy(Qt::NoContextMenu);
	initCtrls();
	initConnections();
}

QxCefViewContainer::~QxCefViewContainer()
{
}

void QxCefViewContainer::browerLoad(const QString& url)
{
	if (m_pCefView)
	{
		if (m_strUrl != url)
		{
			m_strUrl = url;
			m_pCefView->load(url);
		}
		else
		{
			m_pCefView->reload();
		}
	}
}

void QxCefViewContainer::browerReload()
{
	if (m_pCefView && !m_strUrl.isEmpty())
	{
		m_pBtnRefresh->setEnabled(false);
		QTimer::singleShot(5000, m_pBtnRefresh, [&]{
			m_pBtnRefresh->setEnabled(true);
		});
		m_pCefView->reload();
	}
}

bool QxCefViewContainer::browerIsLoading()
{
	if (m_pCefView)
	{
		return m_pCefView->isLoading();
	}
	return false;
}

void QxCefViewContainer::browerStop()
{
	if (m_pCefView)
	{
		m_pCefView->stop();
	}
}

void QxCefViewContainer::browerBack()
{
	if (m_pCefView)
	{
		m_pCefView->back();
	}
}

void QxCefViewContainer::browerForward()
{
	if (m_pCefView)
	{
		m_pCefView->forward();
	}
}

void QxCefViewContainer::setBkground(const QString& file)
{
	if (QFile::exists(file))
	{
		m_pmBg.load(file);
	}
}

void QxCefViewContainer::execJaveScript(const QString& js)
{
	if (m_pCefView)
	{
		m_pCefView->execJaveScript(js);
	}
}

void QxCefViewContainer::setScrollBarPolicy(Qt::Orientation orientation, Qt::ScrollBarPolicy policy)
{
	if (m_pCefView)
	{
		m_pCefView->setScrollBarPolicy(orientation, policy);
	}
}

void QxCefViewContainer::setNeedToolBar(bool need)
{
	m_pBtnClose->setVisible(need);
	m_pBtnRefresh->setVisible(need);
	m_bNeedToolBar = need;
}

void QxCefViewContainer::initCtrls()
{
	m_pBtnClose = new QPushButton(this);
	m_pBtnClose->setObjectName("style_customboxbtnclose");
	m_pBtnClose->setFixedSize(15, 15);

	m_pBtnRefresh = new QPushButton(this);
	m_pBtnRefresh->setObjectName("style_btnrefresh");
	m_pBtnRefresh->setFixedSize(14, 16);

	m_pBtnClose->setVisible(m_bNeedToolBar);
	m_pBtnRefresh->setVisible(m_bNeedToolBar);

	m_pCefView = new CefWebView(this);
	m_pCefView->setFixedSize(1270, 660);
	
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addStretch();
	layout->addWidget(m_pBtnRefresh, 0, Qt::AlignVCenter);
	layout->addSpacing(10);
	layout->addWidget(m_pBtnClose, 0, Qt::AlignVCenter);
	layout->addSpacing(10);

	QVBoxLayout *mainlayout = new QVBoxLayout(this);
	mainlayout->setContentsMargins(3, 0, 3, 3);
	mainlayout->setSpacing(0);
	mainlayout->addSpacing(10);
	mainlayout->addLayout(layout);
	mainlayout->addSpacing(10);
	mainlayout->addWidget(m_pCefView);

	setLayout(mainlayout);

	//layout体系，要先show(),才能改变窗口的大小
	//The layout system will be notified as soon as the widget is shown.
	show();
}

void QxCefViewContainer::initConnections()
{
	connect(m_pCefView, &CefWebView::loadFinished, this, [&](int httpStatusCode){
		m_pBtnRefresh->setEnabled(true);
	});

	connect(m_pCefView, &QCefView::linkClicked, this, [&](const QString & url){
		QDesktopServices::openUrl(QUrl(url));
	});

	connect(m_pBtnClose, &QPushButton::clicked, this, [&]{
		if (m_pCefView){
			m_pCefView->stop();
		}
		hide();
	});

	connect(m_pBtnRefresh, &QPushButton::clicked, this, [&]{
		if (m_pCefView){
			m_pCefView->reload();
		}
	});
}

void QxCefViewContainer::paintEvent(QPaintEvent*)
{
	QPainter p(this);

	if(m_bNeedToolBar)
	{
		p.fillRect(QRect(0, 0, width(), 35), QBrush(QColor(42,41, 42)));
		p.fillRect(rect().adjusted(2, 2, -2, -2), QBrush(QColor(48, 29, 19)));
		p.fillRect(rect().adjusted(3,36,-3, -3), QBrush(QColor(243,227, 196)));
		p.setPen(QColor(31, 22, 17));
		p.drawRect(rect().adjusted(0, 0, -1, -1));
		p.setPen(QColor(67, 50, 40));
		p.drawRect(rect().adjusted(1, 1, -2, -2));
	}

	if(!m_pmBg.isNull())
	{
		p.drawPixmap(rect(), m_pmBg);
	}
}

void QxCefViewContainer::mousePressEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton)
	{
		m_bIsLeftPress = true;
		m_ptLastPos = event->globalPos();
	}
}

void QxCefViewContainer::mouseMoveEvent(QMouseEvent *event)
{
	if (m_bIsLeftPress)
	{
		QPoint ptOffset = event->globalPos() - m_ptLastPos;
		m_ptLastPos = event->globalPos();
		move(ptOffset + pos());	
	}
}

void QxCefViewContainer::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_bIsLeftPress = false;
	}
}


WebViewWidget::WebViewWidget(QWidget* parent) :
	QWidget(parent),m_pCefView(nullptr)
{
	QHBoxLayout* layout = new QHBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);
	m_pCefView = new CefWebView(this);
	m_pCefView->setGeometry(0,0,282,738);
	m_pCefView->setLinkOpenPolicy(QCefView::eLinkDontOpenByCef);
	connect(m_pCefView, &QCefView::linkClicked, this, [&](const QString & url){
		qDebug() << "link clicked:" << url;
		m_pCefView->load(url);
	});
	connect(m_pCefView, &QCefView::qfProtocolUrlNotify, this, [=](const QString & url){
		qDebug() << "qfProtocolUrlNotify:" << url;
	});
	setLayout(layout);
}

WebViewWidget::~WebViewWidget() {

}

void WebViewWidget::Load(const std::string& url)
{
	if (m_pCefView != nullptr) {
		m_pCefView->load(QString::fromStdString(url));
	}
}

void WebViewWidget::Reload() {
	if (m_pCefView != nullptr) {
		m_pCefView->reload();
	}
}

void WebViewWidget::resizeEvent(QResizeEvent* event) {
	if (m_pCefView != nullptr) {
		m_pCefView->setGeometry(0,0,this->width(),this->height());
	}
	QWidget::resizeEvent(event);
}
