#include "mainwindow.h"
#include <windows.h>
#include <QCoreApplication>
#include <QThread>
#include <QHBoxLayout>
#include <QDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTime>
#include <QHideEvent>
#include <QDebug>
#include "QCefViewSDK/include/QCefSetting.h"

#define OSR_WEBVIEW
QCefViewMainWidget::QCefViewMainWidget(QWidget *parent)
	: QMainWindow(parent)
	, m_pCefview(nullptr)
{
	ui.setupUi(this);
	//qDebug() << "MainThreadId()" << QThread::currentThreadId();

#ifdef OSR_WEBVIEW
	m_pCefview = new CefWebView(this, true);
#else
	m_pCefview = new CefWebView(this, false);
#endif
	
	m_pCefview->setGeometry(0,0,800,600);
	m_pCefview->setLinkOpenPolicy(QCefView::eLinkDontOpenByCef);
	//m_pCefview->setCefContextMenuEnable(true);
	//m_pCefview->setCefDevToolsEnable(true);
	//m_pCefview->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
	//m_pCefview->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);

	connect(m_pCefview, &QCefView::linkClicked, this, [&](const QString & url){
		qDebug() << "link clicked:" << url;
		m_pCefview->load(url);
	});
	connect(m_pCefview, &QCefView::qfProtocolUrlNotify, this, [=](const QString & url){
		qDebug() << "qfProtocolUrlNotify:" << url;
	});

	QString strUrl = "http://pt.7fgame.com/myheros/home/hy.html";
	m_pCefview->load("https://www.huya.com/");
	//m_pCefview->execJaveScript("leftDH.setHDRedDot('1:1,')");


	ui.lineEdit_url->setText(strUrl);

	QVBoxLayout* layoutV = new QVBoxLayout();
	layoutV->setContentsMargins(2, 2, 2, 2);
	layoutV->setSpacing(3);

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setContentsMargins(2, 2, 2, 2);
	layout->setSpacing(3);

	layout->addWidget(ui.lineEdit_url);
	layout->addWidget(ui.btn_back);
	layout->addWidget(ui.btn_forward);
	layout->addWidget(ui.btn_reload);
	layout->addWidget(ui.btn_stop);
	layoutV->addLayout(layout);

	ui.cefContainer->layout()->addWidget(m_pCefview);
	layoutV->addWidget(ui.cefContainer);
	centralWidget()->setLayout(layoutV);

	QPushButton* pBtn = new QPushButton(this);
	pBtn->move(3, 28);
	pBtn->resize(100,30);
	pBtn->setText("Cef");
	pBtn->hide();
	connect(pBtn, &QPushButton::clicked, this, [=]{
		if (m_pCefview){
			m_pCefview->setVisible(!m_pCefview->isVisible());
		}
	});

	connect(ui.btn_back, SIGNAL(clicked()),
		this, SLOT(onNavigateGoBack()));

	connect(ui.btn_forward, SIGNAL(clicked()),
		this, SLOT(onNavigateGoForward()));

	connect(ui.btn_reload, SIGNAL(clicked()),
		this, SLOT(onNavigateReload()));

	connect(ui.btn_stop, SIGNAL(clicked()),
		this, SLOT(onNavigateStop()));

	connect(ui.lineEdit_url, SIGNAL(returnPressed()),
		this, SLOT(onNavigateToUrl()));
}

QCefViewMainWidget::~QCefViewMainWidget()
{
}

void QCefViewMainWidget::onNavigateToUrl()
{
	if (m_pCefview)
	{
		QString strUrl = ui.lineEdit_url->text();
		m_pCefview->load(strUrl);
	}
}

void QCefViewMainWidget::onNavigateReload()
{
	if (m_pCefview)
	{
		m_pCefview->reload();
	}
}

void QCefViewMainWidget::onNavigateGoBack()
{
	if (m_pCefview && m_pCefview->browserCanGoBack())
	{
		m_pCefview->back();
	}
}

void QCefViewMainWidget::onNavigateGoForward()
{
	if (m_pCefview && m_pCefview->browserCanGoForward())
	{
		m_pCefview->forward();
	}
}

void QCefViewMainWidget::onNavigateStop()
{
	if (m_pCefview && m_pCefview->browserIsLoading())
	{
		m_pCefview->stop();
	}
}

void QCefViewMainWidget::paintEvent(QPaintEvent *)
{
	if (!pm.isNull())
	{
		QPainter p(this);
		p.drawPixmap(ui.cefContainer->geometry().x(),ui.cefContainer->geometry().y(),pm);
	}
}

bool QCefViewMainWidget::eventFilter(QObject *obj, QEvent *event)
{
	return QWidget::eventFilter(obj, event);
}

bool QCefViewMainWidget::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	return false;
}

void QCefViewMainWidget::hideEvent(QHideEvent *e)
{
	e->accept();
}

void QCefViewMainWidget::toggleScrollbar()
{
	static bool sHide = false;
	if (m_pCefview)
	{
		if (!sHide)
		{
			m_pCefview->execJaveScript("document.body.style.overflow = 'hidden';");
		}
		else
		{
			m_pCefview->execJaveScript("document.body.style.overflow = 'auto';");
		}
		sHide = !sHide;
	}
}

void QCefViewMainWidget::destroyCefViewObjInAdvance()
{
	if (m_pCefview)
	{
		delete m_pCefview;
		m_pCefview = nullptr;
	}
}
