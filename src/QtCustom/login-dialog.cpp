#include "login-dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "custom-stylesheet.h"
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QSslConfiguration>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <string>
#include <QUrl>
#include <QTimer>
#include <QKeyEvent>
#include <QNetworkInterface>
#include "custom-config-ini.h"
#include "md5.h"
#include "base-define.h"
#include "util/base.h"
#include "string-convert.h"
#include <json/json.h>
#include "customer-service-base-dialog.h"
#include "json-parser.h"
#include "msgbox.h"

QString GetHostMacAddress()
{
	QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();
	int nCnt = nets.count();
	QString strMacAddr = "";
	for(int i = 0; i < nCnt; i ++)
	{
	
		if(nets[i].flags().testFlag(QNetworkInterface::IsUp) && nets[i].flags().testFlag(QNetworkInterface::IsRunning) && !nets[i].flags().testFlag(QNetworkInterface::IsLoopBack))
		{
			strMacAddr = nets[i].hardwareAddress();
			break;
		}
	}
	if (strMacAddr.length() > 1) {
		blog(LOG_INFO,"m_a:%s",strMacAddr.toLocal8Bit().data());
	}
	return strMacAddr;
}

LoginDialog::LoginDialog(QWidget *parent)
	: MoveDialog(parent), m_bOperate(false),m_bSuccess(false)
{
	connect(AsynReqManager::GetInstance(),&AsynReqManager::readyData, this, &LoginDialog::OnReadyData);

	this->setMinimumSize(818, 658);
	this->setMaximumSize(818, 658);
	setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	SetPixmap(":/res/images/login_mask.png");
	QVBoxLayout *vlayout = new QVBoxLayout(this);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	m_pClose = new QPushButton(this);
	m_pClose->setStyleSheet(SMALL_CLOSE_BTN);
	m_pClose->setFixedSize(36,36);
	m_pClose->setAutoDefault(false);
	m_pClose->setDefault(false);
	
	QHBoxLayout *h1 = new QHBoxLayout;
	h1->setContentsMargins(0, 0, 0, 0);
	h1->setSpacing(0);
	h1->addStretch();
	h1->addWidget(m_pClose);
	h1->addSpacing(55);
	vlayout->addSpacing(46);
	vlayout->addLayout(h1);

	m_pIcon = new QLabel(this);
	m_pIcon->setStyleSheet(LOGIN_APP_ICON);
	m_pIcon->setFixedSize(100, 100);
	QHBoxLayout *h2 = new QHBoxLayout;
	h2->setContentsMargins(0, 0, 0, 0);
	h2->setSpacing(0);
	h2->addStretch();
	h2->addWidget(m_pIcon);
	h2->addStretch();
	vlayout->addSpacing(21);
	vlayout->addLayout(h2);

	m_pAppName = new QLabel(tr("LOGIN_APP_NAME"),this);
	m_pAppName->setFixedHeight(18);
	m_pAppName->setStyleSheet(LOGIN_APP_NAME);
	m_pAppName->setAlignment(Qt::AlignCenter);
	QHBoxLayout *hname = new QHBoxLayout;
	hname->setContentsMargins(0, 0, 0, 0);
	hname->setSpacing(0);
	hname->addStretch();
	hname->addWidget(m_pAppName);
	hname->addStretch();
	vlayout->addSpacing(4);
	vlayout->addLayout(hname);

	//商户编码
	m_pShopCodeIcon = new QLabel(this);
	m_pShopCodeIcon->setStyleSheet(LOGIN_SHOP_CODE_ICON);
	m_pShopCodeIcon->setFixedSize(20, 20);
	m_pShopCode = new QLineEdit(this);
	m_pShopCode->setPlaceholderText(tr("please input shop code"));
	m_pShopCode->setFixedSize(277, 20);
	m_pShopCode->setStyleSheet(LOGIN_SHOP_CODE);
	m_pShopCode->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);

	QHBoxLayout *h = new QHBoxLayout;
	h->setContentsMargins(0, 0, 0, 0);
	h->setSpacing(0);
	h->addSpacing(257);
	
	h->addWidget(m_pShopCodeIcon);
	h->addSpacing(10);
	h->addWidget(m_pShopCode);
	h->addStretch();
	vlayout->addSpacing(21);
	vlayout->addLayout(h);
	vlayout->addSpacing(15);
	m_pLine = new QLabel(this);
	m_pLine->setStyleSheet(LOGIN_LINE);
	m_pLine->setFixedSize(310,1);
	QHBoxLayout *line = new QHBoxLayout;
	line->setContentsMargins(0, 0, 0, 0);
	line->setSpacing(0);
	line->addStretch();
	line->addWidget(m_pLine);
	line->addStretch();
	vlayout->addLayout(line);
	//用户名
	
	m_pUserNameIcon = new QLabel(this);
	m_pUserNameIcon->setStyleSheet(LOGIN_USER_NAME_ICON);
	m_pUserNameIcon->setFixedSize(20, 20);
	m_pUserName = new QLineEdit(this);
	m_pUserName->setStyleSheet(LOGIN_SHOP_CODE);
	m_pUserName->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	m_pUserName->setFixedSize(277,20);
	m_pUserName->setPlaceholderText(QString(tr("please input username")));
	QHBoxLayout *h3 = new QHBoxLayout;
	h3->setContentsMargins(0, 0, 0, 0);
	h3->setSpacing(0);
	h3->addSpacing(257);
	h3->addWidget(m_pUserNameIcon);
	h3->addSpacing(10);
	h3->addWidget(m_pUserName);
	h3->addStretch();

	vlayout->addSpacing(25);
	vlayout->addLayout(h3);

	m_pLine1 = new QLabel(this);
	m_pLine1->setFixedSize(310, 1);
	m_pLine1->setStyleSheet(LOGIN_LINE);
	QHBoxLayout *line1 = new QHBoxLayout;
	line1->setContentsMargins(0, 0, 0, 0);
	line1->setSpacing(0);
	line1->addStretch();
	line1->addWidget(m_pLine1);
	line1->addStretch();
	vlayout->addSpacing(15);
	vlayout->addLayout(line1);
	//密码
	m_pPasswordIcon = new QLabel(this);
	m_pPasswordIcon->setStyleSheet(LOGIN_PASSWORD_ICON);
	m_pPasswordIcon->setFixedSize(20, 20);
	m_pPassword = new QLineEdit(this);
	m_pPassword->setStyleSheet(LOGIN_SHOP_CODE);
	m_pPassword->setFixedSize(249, 20);
	m_pPassword->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
	m_pPassword->setPlaceholderText(QString(tr("please input password")));
	m_pPassword->setEchoMode(QLineEdit::Password);
	
	m_pEye = new QPushButton(this);
	m_pEye->setCheckable(true);
	m_pEye->setFixedSize(28,28);
	m_pEye->setStyleSheet(EYE_STYLE);
	m_pEye->setChecked(false);
	auto EyeSlot = [&]() {
		bool check = m_pEye->isChecked();
		if (check) {
			m_pPassword->setEchoMode(QLineEdit::Normal);
		} else {
			m_pPassword->setEchoMode(QLineEdit::Password);
		}
	};
	connect(m_pEye,&QPushButton::clicked,EyeSlot);


	QHBoxLayout *h4 = new QHBoxLayout;
	h4->setContentsMargins(0, 0, 0, 0);
	h4->setSpacing(0);
	h4->addSpacing(257);
	h4->addWidget(m_pPasswordIcon);
	h4->addSpacing(10);
	h4->addWidget(m_pPassword);
	h4->addWidget(m_pEye);
	h4->addStretch();
	vlayout->addSpacing(25);
	vlayout->addLayout(h4);

	m_pLine2 = new QLabel(this);
	m_pLine2->setFixedSize(310, 1);
	m_pLine2->setStyleSheet(LOGIN_LINE);

	QHBoxLayout *line2 = new QHBoxLayout;
	line2->setContentsMargins(0, 0, 0, 0);
	line2->addStretch();
	line2->addWidget(m_pLine2);
	line2->addStretch();
	vlayout->addSpacing(15);
	vlayout->addLayout(line2);
	
	
	m_pLoginBtn = new QPushButton(QString(tr("LOGIN")),this);
	m_pLoginBtn->setFixedSize(309,45);
	m_pLoginBtn->setStyleSheet(LOGIN_BTN);
	m_pLoginBtn->setFocusPolicy(Qt::NoFocus);
	m_pLoginBtn->setAutoDefault(true);
	m_pLoginBtn->setDefault(true);
	QHBoxLayout *h6 = new QHBoxLayout;
	h6->setContentsMargins(0, 0, 0, 0);
	h6->setSpacing(0);
	h6->addStretch();
	h6->addWidget(m_pLoginBtn);
	h6->addStretch();
	vlayout->addSpacing(25);
	vlayout->addLayout(h6);
	QString strMac = GetHostMacAddress();

	MD5 macUnique(strMac.toStdString());
	m_strMacMd5 = macUnique.toString();
	QString uniqueCH = StringConvert::GetInstance()->GetString(STR_UNIQUECODE);
	QString strText = uniqueCH + QString::fromStdString(m_strMacMd5);
	m_pUniqueCode = new QLabel(strText,this);
	m_pUniqueCode->setFixedHeight(22);
	m_pUniqueCode->setStyleSheet(QString(
		"color:#444444;font-size:14px;"
		"font-family:Microsoft YaHei;"
		"background:transparent;line-height:22px;"
	));

	m_pReqAuth=new QPushButton(StringConvert::GetInstance()->GetString(STR_UNIQUECODE_REQ),this);
	m_pReqAuth->setFixedSize(82,30);
	m_pReqAuth->setStyleSheet(QString(
		"background: #E6F7FF;border-radius:6px;border:1px solid #91D5FF;"
		"color:#1890ff;line-height:22px;font-weight:400;"
		"font-size:14px;font-family:Microsoft YaHei;"
	));
	connect(m_pReqAuth, &QPushButton::clicked, this, &LoginDialog::OnReqAuthClicked);
	m_pReqAuth->setVisible(false);

	m_pAuthIcon = new QLabel(this);
	m_pAuthIcon->setFixedSize(14,14);
	m_pAuthIcon->setStyleSheet(QString(
		"background:transparent;"
		"border-image:url(:/res/images/login/secico.png);"
	));
	m_pAuthIcon->setVisible(false);
	
	m_pAuthTxt = new QLabel(StringConvert::GetInstance()->GetString(STR_UNIQUECODE_AUTHENTICATED),this);
	m_pAuthTxt->setFixedHeight(22);
	m_pAuthTxt->setStyleSheet(QString(
		"font-family:Microsoft YaHei;"
		"color:#888888;font-size:13px;line-height:22px;font-weight:400;background:transparent;"
	));
	m_pAuthTxt->setVisible(false);

	QHBoxLayout *h7 = new QHBoxLayout;
	h7->setContentsMargins(0, 0, 0, 0);
	h7->setSpacing(0);
	h7->addStretch();
	h7->addWidget(m_pUniqueCode);
	h7->addSpacing(12);
	h7->addWidget(m_pReqAuth);
	h7->addWidget(m_pAuthIcon);
	h7->addSpacing(2);
	h7->addWidget(m_pAuthTxt);
	h7->addStretch();
	vlayout->addSpacing(25);
	vlayout->addLayout(h7);

	m_pUnAuthIcon=new QLabel(this);
	m_pUnAuthIcon->setFixedSize(14,14);
	m_pUnAuthIcon->setStyleSheet(QString(
		"background:transparent;"
		"border-image:url(:/res/images/login/auto_reject.png);"
	));
	m_pUnAuthIcon->setVisible(false);

	m_pUnAuthTxt=new QLabel(StringConvert::GetInstance()->GetString(STR_UNIQUECODE_REJECT),this);
	m_pUnAuthTxt->setFixedHeight(22);
	m_pUnAuthTxt->setStyleSheet(QString(
		"font-family:Microsoft YaHei;"
		"color:#888888;font-size:13px;line-height:22px;font-weight:400;background:transparent;"
	));
	m_pUnAuthTxt->setVisible(false);

	QHBoxLayout *h8 = new QHBoxLayout;
	h8->setContentsMargins(0, 0, 0, 0);
	h8->setSpacing(0);
	h8->addStretch();
	h8->addWidget(m_pUnAuthIcon);
	h8->addSpacing(5);
	h8->addWidget(m_pUnAuthTxt);
	h8->addStretch();
	vlayout->addSpacing(5);
	vlayout->addLayout(h8);

	//connect(m_pLoginBtn,SIGNAL(clicked()),this,SLOT(OnLoginBtnClicked()));
	connect(m_pLoginBtn,&QPushButton::clicked,[&](){
		OnLoginBtnClicked();
	});
	
	vlayout->addStretch();
	
	auto close_slot = [&]() {
		QByteArray arr;
		emit loginSignal(LOGIN_CLOSE_CLICK, arr);
	};
	connect(m_pClose, &QPushButton::clicked,close_slot);
	QIcon icon;
        icon.addFile(QString::fromUtf8(":/res/images/keepfun.ico"), QSize(), QIcon::Normal, QIcon::Off);
        setWindowIcon(icon);
#if _DEBUG
	m_pShopCode->setText("CS1035");
	m_pUserName->setText("caojianfeng");
	m_pPassword->setText("ics123");
#else
	//高勇强快捷登录
	m_pShopCode->setText("CS1035");
	m_pUserName->setText("techtest03");
	m_pPassword->setText("ics123");


	QFile file("./login.txt");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;
	else {
		QByteArray lg = file.readAll();
		if (!lg.isEmpty()) {
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(lg.data(), root)) {
				std::string code = root["code"].asString();
				std::string un = root["un"].asString();
				std::string pwd = root["pwd"].asString();
				if (!code.empty() && !un.empty() && !pwd.empty()) {
					m_pShopCode->setText(QString::fromStdString(code));
					m_pUserName->setText(QString::fromStdString(un));
					m_pPassword->setText(QString::fromStdString(pwd));
				}
			}
		}
	}
#endif

	m_pCheckTimer = new QTimer(this);
	m_pCheckTimer->setInterval(5000);
	connect(m_pCheckTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
	WhiteDetail();
	setAttribute(Qt::WA_DeleteOnClose);
}

void LoginDialog::OnTimeOut() {
	WhiteDetail();
}

LoginDialog::~LoginDialog() {}

void LoginDialog::WhiteDetail() {
	STReqData req;
	req.func = FUNC_WHITE_DETAIL;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	sprintf(req.url, "%s/shop/white/detail?str=%s", BASE_URL.c_str(), m_strMacMd5.c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

void LoginDialog::WhiteAuthCheck(const QString &shopCode, const QString &userName, const QString &password) {
	STReqData req;
	req.func = FUNC_AUTH_CHECK;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("deviceId"), QByteArray("1")));
	sprintf(req.url, "%s/shop/auth/check", BASE_URL.c_str());

	Json::FastWriter writer;
	Json::Value root;
	root["str"] = Json::Value(m_strMacMd5);
	root["shopCode"] = Json::Value(shopCode.toStdString());
	root["userName"] = Json::Value(userName.toStdString());
	MD5 md5(password.toStdString());
	std::string md5Pw = md5.toString();
	root["password"] = Json::Value(md5Pw);
	sprintf(req.body, "%s", writer.write(root).c_str());
	blog(LOG_INFO,"mac req:%s",req.body);
	AsynReqManager::GetInstance()->InsertReq(req);
}

void LoginDialog::SetApplyStatus(int status) {

	m_pReqAuth->setVisible(status==-1);
	m_pAuthIcon->setVisible(status==1);
	m_pAuthTxt->setVisible(status==1);

	m_pUnAuthIcon->setVisible(status==2 || status==0);
	m_pUnAuthTxt->setVisible(status==2|| status==0);
	if (status == 2) {
		m_pUnAuthTxt->setText(StringConvert::GetInstance()->GetString(STR_UNIQUECODE_REJECT));
	} else if (status ==0) {
		m_pUnAuthTxt->setText(StringConvert::GetInstance()->GetString(STR_UNIQUECODE_AUTHING));
	}
}

void LoginDialog::SetOperate(bool operate) {
	m_bOperate = operate;
}
bool LoginDialog::IsOperate() {
	return m_bOperate;
}

void LoginDialog::OnReadyData(const STRspData &stRsp) {
	switch (stRsp.func) {
		case FUNC_WHITE_DETAIL: {
			STDeviceDetail stDetail = ParseDeviceDetail(stRsp.recvBytes);
			if (stDetail.success) {
				SetApplyStatus(stDetail.applyStatus);
				if (m_pCheckTimer->isActive()) {
					m_pCheckTimer->stop();
					m_pCheckTimer->start();
				} else {
					m_pCheckTimer->start();
				}
			}
			break;
		}
		case FUNC_AUTH_CHECK: {
			STBase stBase;
			ParseBase(stRsp.recvBytes, stBase);
			WhiteDetail();
		}
		default:
			break;
	}
}

void LoginDialog::OnReqAuthClicked() {
	if (m_pShopCode->text().isEmpty() || m_pUserName->text().isEmpty() || m_pPassword->text().isEmpty()) {
		TipsDialog dialog(StringConvert::GetInstance()->GetString(STR_WHITE_ARGS_NOT_NULL), this);
		dialog.exec();
		return;
	}
	WhiteAuthCheck(m_pShopCode->text(), m_pUserName->text(), m_pPassword->text());
	TipsDialog dialog(StringConvert::GetInstance()->GetString(STR_WHITE_APPLY), this);
	dialog.exec();
}

void LoginDialog::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Enter:
	case Qt::Key_Return:
		OnLoginBtnClicked();
		break;
	default:
		break;
	}
}

void LoginDialog::OnLoginBtnClicked() {
	if (IsOperate()||m_bSuccess)
		return;
	if (m_pUserName->text().isEmpty()) {
		return;
	}
	if (m_pPassword->text().isEmpty()) {
		return;
	}
	m_bOperate = true;
	m_strName = m_pUserName->text();
	m_strPassword = m_pPassword->text();

	std::string url = BASE_URL +
		std::string("/shop/auth/login");
	
	QString strScheme = QString::fromStdString(url);
	QNetworkRequest networkReq;
	networkReq.setUrl(QUrl(strScheme));
	QNetworkAccessManager *pManager = new QNetworkAccessManager;
	QNetworkReply *pNetworkReply = nullptr;
	std::string userName = m_pUserName->text().toStdString();
	
	std::string password = m_pPassword->text().toStdString();
	MD5 md5(password);
	std::string md5Pw = md5.toString();
	std::string shopcode = m_pShopCode->text().toStdString();
	char body[2048] = {0};
	QString strIpAddress;
	//QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	//// 获取第一个本主机的IPv4地址
	//int nListSize = ipAddressesList.size();
	//for (int i = 0; i < nListSize; ++i)
	//{
	//	if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
	//	ipAddressesList.at(i).toIPv4Address()) {
	//	strIpAddress = ipAddressesList.at(i).toString();
	//	break;
	//	}
	//}

	strIpAddress = QString::fromStdString(m_strMacMd5) + QString("teck2021");

	MD5 ipMd5(strIpAddress.toStdString());
	std::string ipMd5HSH = ipMd5.toString();

#if 0
	sprintf_s(body, "{\"password\":\"%s\",\"shopCode\":\"%s\",\"userName\":\"%s\"}",
		"e10adc3949ba59abbe56e057f20f883e", "CS1035", "wangqing01");
#else
	sprintf_s(body, "{\"password\":\"%s\",\"shopCode\":\"%s\",\"userName\":\"%s\",\"checkCode\":\"%s\"}",
		md5Pw.c_str(), shopcode.c_str(), userName.c_str(),ipMd5HSH.c_str());
#endif
	blog(LOG_INFO, "url:%s",url.c_str());
	blog(LOG_INFO, "login body:%s", body);
	networkReq.setRawHeader("Content-Type", "application/json");
	networkReq.setRawHeader("platform", "5");
	networkReq.setRawHeader("deviceId", "1");
	
	if (strScheme.contains("https")) {
#ifndef QT_NO_SSL
		bool sup = QSslSocket::supportsSsl();
		QSslConfiguration conf = networkReq.sslConfiguration();
		conf.setPeerVerifyMode(QSslSocket::VerifyNone);
		conf.setProtocol(QSsl::TlsV1SslV3);
		networkReq.setSslConfiguration(conf);
		sup = QSslSocket::supportsSsl();
		if (sup) {
			blog(LOG_INFO, "support ssl");
		} else {
			blog(LOG_INFO, "not support ssl");
			QString supVersion = QSslSocket::sslLibraryBuildVersionString();
			blog(LOG_INFO, "openssl version:%s", supVersion.toStdString().data());
		}
#endif
	}
	pNetworkReply = pManager->post(networkReq, body);
	// 阻塞请求等待返回
	QEventLoop eventLoop;
	connect(pNetworkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
	QTimer::singleShot(15000, &eventLoop, SLOT(quit()));
	eventLoop.exec();
	
	if (pNetworkReply->error() != QNetworkReply::NoError) {
		blog(LOG_INFO, "error:%d,%s,url:%s", pNetworkReply->error(),
		     pNetworkReply->errorString().toStdString().c_str(),
		     pNetworkReply->url().toString().toLocal8Bit().data());
		
	} else {
		blog(LOG_INFO,"get login response");
		QByteArray strValue = pNetworkReply->readAll();
		blog(LOG_INFO,"login data:%s",strValue.data());
		STBase stBase;
		ParseBase(strValue, stBase);
		if (stBase.success) {
			m_bSuccess = true;
		}
		emit loginSignal(LOGIN_CLICK, strValue);
		
	}
	pNetworkReply->deleteLater();
	pManager->deleteLater();
	m_bOperate = false;
}
