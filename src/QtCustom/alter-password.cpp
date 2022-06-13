#include "alter-password.h"
#include <QLabel>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "custom-stylesheet.h"
#include "common-function.h"
#include <QRegExpValidator>

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
#include "md5.h"
#include "base-define.h"
#include "util/base.h"
#include "custom-config-ini.h"
#include "characters-convert.h"
#include "json-parser.h"
#include "string-convert.h"
#include <QMessageBox>
#include <QByteArray>
#include "msgbox.h"

AlterPassword::AlterPassword(STUser stUser, QWidget* parent)
	: MoveDialog(parent)
	,m_pCloseBtn(nullptr)
	,m_pTitle(nullptr)
	,m_pUserName(nullptr)
	,m_pUserNameEdit(nullptr)
	,m_pOldPassword(nullptr)
	,m_pOldPasswordEdit(nullptr)
	,m_pNewPassword(nullptr)
	,m_pNewPasswordEdit(nullptr)
	,m_pNewConfirmPassword(nullptr)
	,m_pNewConfirmPasswordEdit(nullptr)
	,m_pSaveBtn(nullptr)
{
	setFixedSize(379+76, 380+76);
	SetNinePixmap(":/res/images/msg-mask.png", 50, 60, 379+76, 380+76);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);

	QLabel *ltop = new QLabel(this);
	ltop->setFixedSize(379,40);
	QPixmap pixmap;
	pixmap = CommonFunction::GetMidPixmap(":/res/images/cs_top_bg.png",379);
	ltop->setStyleSheet("background:transparent;");
	ltop->setPixmap(pixmap);
	

	m_pTitle	= new QLabel(tr("alter password"),ltop);
	m_pTitle->setStyleSheet(LABEL_TITLE_TEXT);
	m_pTitle->setFixedSize(100,18);

	m_pCloseBtn	= new QPushButton(ltop);
	m_pCloseBtn->setStyleSheet(CLOSE_BTN);
	m_pCloseBtn->setFixedSize(18,18);
	connect(m_pCloseBtn, SIGNAL(clicked()), this, SLOT(close()));
	QHBoxLayout* hltop = new QHBoxLayout;
	hltop->setContentsMargins(0,0,0,0);
	hltop->setSpacing(0);
	hltop->addSpacing(17);
	hltop->addWidget(m_pTitle);
	hltop->addStretch();
	hltop->addWidget(m_pCloseBtn);
	hltop->addSpacing(20);
	ltop->setLayout(hltop);

	QRegExpValidator *pRevalidotor = new QRegExpValidator(QRegExp("[0-9a-zA-Z]{6,20}"), this);

	m_pUserName	= new QLabel(tr("login account"),this);
	m_pUserName->setFixedSize(66,20);
	m_pUserName->setStyleSheet(LABEL_TEXT);
	m_pUserNameEdit = new QLineEdit(this);
	m_pUserNameEdit->setFixedSize(228,36);
	m_pUserNameEdit->setStyleSheet(LABEL_INPUT_EDIT);
	m_pUserNameEdit->setPlaceholderText(tr("please input user name"));
	m_pUserNameEdit->setText(QString::fromStdString(stUser.userName));
	m_pUserNameEdit->setReadOnly(true);
	m_pUserNameEdit->setMaxLength(30);

	m_pOldPassword  = new QLabel(tr("origin password"),this);
	m_pOldPassword->setFixedSize(66,20);
	m_pOldPassword->setStyleSheet(LABEL_TEXT);
	m_pOldPasswordEdit= new QLineEdit(this);
	m_pOldPasswordEdit->setFixedSize(228,36);
	m_pOldPasswordEdit->setStyleSheet(LABEL_INPUT_EDIT);
	m_pOldPasswordEdit->setPlaceholderText(tr("input rules"));
	m_pOldPasswordEdit->setValidator(pRevalidotor);
	

	m_pNewPassword= new QLabel(tr("new password"),this);
	m_pNewPassword->setFixedSize(66,20);
	m_pNewPassword->setStyleSheet(LABEL_TEXT);
	m_pNewPasswordEdit= new QLineEdit(this);
	m_pNewPasswordEdit->setFixedSize(228,36);
	m_pNewPasswordEdit->setStyleSheet(LABEL_INPUT_EDIT);
	m_pNewPasswordEdit->setPlaceholderText(tr("input rules"));
	m_pNewPasswordEdit->setValidator(pRevalidotor);

	m_pNewConfirmPassword= new QLabel(tr("confirm new password"),this);
	m_pNewConfirmPassword->setFixedSize(66,20);
	m_pNewConfirmPassword->setStyleSheet(LABEL_TEXT);
	m_pNewConfirmPasswordEdit= new QLineEdit(this);
	m_pNewConfirmPasswordEdit->setFixedSize(228,36);
	m_pNewConfirmPasswordEdit->setStyleSheet(LABEL_INPUT_EDIT);
	m_pNewConfirmPasswordEdit->setPlaceholderText(tr("input rules"));
	m_pNewConfirmPasswordEdit->setValidator(pRevalidotor);

	m_pSaveBtn= new QPushButton(tr("save"),this);
	m_pSaveBtn->setFixedSize(140,36);
	m_pSaveBtn->setStyleSheet("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px; font-size:14px; color:#222222;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");
	m_pSaveBtn->setFocusPolicy(Qt::NoFocus);
	m_pSaveBtn->setAutoDefault(true);
	m_pSaveBtn->setDefault(true);
	connect(m_pSaveBtn, SIGNAL(clicked()), this, SLOT(OnSaveBtnClicked()));

	QVBoxLayout* vlay = new QVBoxLayout;
	vlay->setContentsMargins(38,29,38,47);
	vlay->setSpacing(0);
	/**
	* 顶部栏
	*/
	QHBoxLayout* h1 = new QHBoxLayout;
	h1->setContentsMargins(0,0,0,0);
	h1->setSpacing(0);
	h1->addWidget(ltop);
	vlay->addLayout(h1);
	/**
	* 横线
	*/
	
	/**
	* 登陆账号
	*/
	QHBoxLayout* h3 = new QHBoxLayout;
	h3->setContentsMargins(0,0,0,0);
	h3->setSpacing(0);
	h3->addSpacing(30);
	h3->addWidget(m_pUserName);
	h3->addSpacing(15);
	h3->addWidget(m_pUserNameEdit);
	h3->addStretch();
	vlay->addSpacing(29);
	vlay->addLayout(h3);
	/**
	* 原始密码
	*/
	QHBoxLayout* h4 = new QHBoxLayout;
	h4->setContentsMargins(0,0,0,0);
	h4->setSpacing(0);
	h4->addSpacing(30);
	h4->addWidget(m_pOldPassword);
	h4->addSpacing(15);
	h4->addWidget(m_pOldPasswordEdit);
	h4->addStretch();
	vlay->addSpacing(15);
	vlay->addLayout(h4);
	/**
	* 新密码
	*/
	QHBoxLayout* h5 = new QHBoxLayout;
	h5->setContentsMargins(0,0,0,0);
	h5->setSpacing(0);
	h5->addSpacing(30);
	h5->addWidget(m_pNewPassword);
	h5->addSpacing(15);
	h5->addWidget(m_pNewPasswordEdit);
	h5->addStretch();
	vlay->addSpacing(15);
	vlay->addLayout(h5);
	/**
	* 再次输入
	*/
	QHBoxLayout* h6 = new QHBoxLayout;
	h6->setContentsMargins(0,0,0,0);
	h6->setSpacing(0);
	h6->addSpacing(30);
	h6->addWidget(m_pNewConfirmPassword);
	h6->addSpacing(15);
	h6->addWidget(m_pNewConfirmPasswordEdit);
	h6->addStretch();
	vlay->addSpacing(15);
	vlay->addLayout(h6);
	/**
	* 横线
	*/
	

	/**
	* 保存
	*/
	QHBoxLayout* h8 = new QHBoxLayout;
	h8->setContentsMargins(0,0,0,0);
	h8->setSpacing(0);
	h8->addStretch();
	h8->addWidget(m_pSaveBtn);
	h8->addStretch();
	vlay->addSpacing(37);
	vlay->addLayout(h8);
	vlay->addStretch();
	setLayout(vlay);
}

AlterPassword::~AlterPassword() {

}

bool AlterPassword::IsEnglish(const QString &str)
{
    QByteArray ba = str.toLatin1();
    const char *s = ba.data();
    bool bret = true;
    while(*s)
    {
        if((*s>='A' && *s<='Z') || (*s>='a' && *s<='z'))
        {

        }
        else
        {
            bret = false;
            break;
        }
        s++;
    }
    return bret;
}

void AlterPassword::OnSaveBtnClicked() {
	std::string url = BASE_URL +
		std::string("/shop/auth/updatePassWord");
	QString strScheme = QString::fromStdString(url);
	QNetworkRequest networkReq;
	networkReq.setUrl(QUrl(strScheme));
	QNetworkAccessManager *pManager = new QNetworkAccessManager;
	QNetworkReply *pNetworkReply = nullptr;
	std::string name = m_pUserNameEdit->text().toStdString();
	std::string oldpassword = m_pOldPasswordEdit->text().toStdString();
	MD5 md5(oldpassword);
	std::string oldmd5 = md5.toString();

	std::string newpassword = m_pNewPasswordEdit->text().toStdString();
	MD5 md52(newpassword);
	std::string newmd5 = md52.toString();

	std::string newcfmpassword = m_pNewConfirmPasswordEdit->text().toStdString();
	MD5 md53(newcfmpassword);
	std::string newcfmmd5 = md53.toString();
	if (name.empty()||oldpassword.empty()||newpassword.empty()||newcfmpassword.empty()) {
		MsgBox::information(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS),
				StringConvert::GetInstance()->GetString(STR_HANDLE_INPUT_NOEMPTY));
		return;
	}
	if (m_pNewPasswordEdit->text().contains(QRegExp("^\\d+$"))) {
		MsgBox::information(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS),
				StringConvert::GetInstance()->GetString(STR_PASSWORD_LIMIT));
		return;
	}
	if (IsEnglish(m_pNewPasswordEdit->text())) {
		MsgBox::information(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS),
				StringConvert::GetInstance()->GetString(STR_PASSWORD_LIMIT));
		return;
	}
	if (m_pNewPasswordEdit->text() != m_pNewConfirmPasswordEdit->text()) {
		MsgBox::information(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS),
				StringConvert::GetInstance()->GetString(STR_CONFIRM_PASSWORD_AGAIN));
		return;
	}
	char body[2048] = {0};
	sprintf_s(body,"{\"newPassWord\":\"%s\",\"newPassWord1\":\"%s\",\"passWord\":\"%s\"}",
		newmd5.c_str(), newcfmmd5.c_str(), oldmd5.c_str());

	blog(LOG_INFO, "login body:%s", body);
	networkReq.setRawHeader("Content-Type", "application/json");
	networkReq.setRawHeader("platform", "5");
	networkReq.setRawHeader("deviceId", "1");
	std::string token = CustomConfig::GetLoginData("token");
	networkReq.setRawHeader("token",token.c_str());
	
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
	pNetworkReply = pManager->put(networkReq, body);
	// 阻塞请求等待返回
	std::string errorMsg;
	QEventLoop eventLoop;
	connect(pNetworkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
	QTimer::singleShot(15000, &eventLoop, SLOT(quit()));
	eventLoop.exec();

	if (pNetworkReply->error() != QNetworkReply::NoError) {
		blog(LOG_INFO, "error:%d,url:%s", pNetworkReply->error(),
		     pNetworkReply->url().toString().toLocal8Bit().data());
		
	} else {
		QByteArray strValue = pNetworkReply->readAll();
		errorMsg = CharatersConvert::UTF8ToGBK(std::string(strValue));
		STUpdatePassword stPw = ParseUpdatePassword(strValue);
		if (stPw.success) {
			MsgBox::information(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS),
				StringConvert::GetInstance()->GetString(STR_HANDLE_SUCCESS));
		} else {
			MsgBox::information(nullptr,StringConvert::GetInstance()->GetString(STR_PROCESS),
				StringConvert::GetInstance()->GetString(STR_HANDLE_FAIL));
		}
	}
	pNetworkReply->deleteLater();
	pManager->deleteLater();
	close();
}
