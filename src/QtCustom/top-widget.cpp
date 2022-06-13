#include "top-widget.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>
#include <QMenu>
#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

#include "set-dialog.h"
#include "url-label.h"
#include "custom-stylesheet.h"
#include "customer-service-base-dialog.h"
#include "alter-password.h"
#include <json/json.h>


MessageButton::MessageButton(QWidget *parent)
	: QPushButton(parent)
	, m_pCount(nullptr)
{
	setFixedSize(50, 30);
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);

	layout->addStretch();
	QLabel *label = new QLabel(this);
	label->setFixedSize(18,20);
	label->setStyleSheet("QLabel{border-image:url(:/res/images/message_normal.png);background-color:transparent;}");
	layout->addWidget(label);
	layout->addStretch();

	m_pCount = new QLabel(this);
	m_pCount->setAlignment(Qt::AlignCenter);
	m_pCount->setStyleSheet("QLabel {background:red; border-radius:7px; font-size:10px; color:rgb(255,255,255);}");
	m_pCount->resize(0,0);
}
MessageButton::~MessageButton(){

}

void MessageButton::SetNoteCount(int count) {
	if (count <= 0) {
		m_pCount->setText(QString::number(count));
		m_pCount->resize(0,0);
		m_pCount->setGeometry(0, 0, 0, 0);
	} else if (count > 0 && count < 10) {
		m_pCount->setText(QString::number(count));
		m_pCount->resize(14, 14);
		m_pCount->setGeometry(27, 3, 14, 14);
	} else if (count>=10 && count < 100) {
		m_pCount->setText(QString::number(count));	
		m_pCount->resize(20, 14);
		m_pCount->setGeometry(27, 3, 20, 14);
	} else {
		m_pCount->setText(QString::number(99) + QString("+"));
		m_pCount->resize(26, 14);
		m_pCount->setGeometry(27, 3, 26, 14);
	}
}

TopWidget::TopWidget(STLoginRsp rsp, QWidget *parent)
	: BaseWidget(parent),
	  m_bLeftButtonPressed(false),
	  m_pHead(nullptr),
	  m_pName(nullptr),
	  m_pCompany(nullptr),
	  m_pMessageBtn(nullptr),
	  m_pMinimumBtn(nullptr),
	  m_pMaximumBtn(nullptr),
	  m_pRestoreBtn(nullptr),
	  m_pCloseBtn(nullptr),
	  m_pButtonGroup(nullptr),
	  m_strToken(rsp.stService.token),
	  m_stUser(rsp.stService.stUser)
{
	setMouseTracking(true);
	setObjectName("top_widget_bg");
	setStyleSheet(TOP_WIDGET_BG);
	connect(AsynReqManager::GetInstance(),&AsynReqManager::readyData, this, &TopWidget::OnReadyData);

	m_pButtonGroup = new QButtonGroup(this);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(QMargins(12, 5, 16, 7));

	m_pHead = new QUrlLabel(this,true);
	m_pHead->setFixedSize(QSize(38, 38));
	m_pHead->setPixmap(QPixmap(":/res/images/head_icon.png"));
	m_pHead->SetNetPath(rsp.stService.stUser.stCsUser.avatarUrl.data());
	m_pHead->setAttribute(Qt::WA_TranslucentBackground);
	layout->addWidget(m_pHead);
	layout->addSpacing(8);

	QString name = QString(rsp.stService.stUser.stCsUser.name.data()) + QString(" ") + QString(rsp.stService.stUser.stCsUser.nameEn.data());
	QString company = rsp.stService.stUser.stCsUser.companyName.data();
	QVBoxLayout *verLayout = new QVBoxLayout;
	verLayout->setContentsMargins(0,0,0,0);
	verLayout->setSpacing(0);
	

	m_pName = new QLabel(this);
	m_pName->setMaximumWidth(200);
	m_pName->setStyleSheet(TOP_WIDGET_NAME_LABEL);
	QString text = m_pName->fontMetrics().elidedText(name, Qt::ElideRight, 200, Qt::TextShowMnemonic);
	m_pName->setText(text);
	verLayout->addWidget(m_pName);

	m_pCompany = new QLabel(this);
	m_pCompany->setMaximumWidth(200);
	m_pCompany->setStyleSheet(TOP_WIDGET_COMPANY_LABEL);
	QString text2 = m_pCompany->fontMetrics().elidedText(company, Qt::ElideRight, 200, Qt::TextShowMnemonic);
	m_pCompany->setText(text2);
	verLayout->addWidget(m_pCompany);

	m_pDownBtn = new QPushButton(this);
	m_pDownBtn->setFixedSize(6,5);
	m_pDownBtn->setStyleSheet("QPushButton {border-image:url(:res/images/more.png);}");
	connect(m_pDownBtn, &QPushButton::clicked, this, &TopWidget::OnOperateClick);

	layout->addLayout(verLayout);
	layout->addSpacing(10);
	layout->addWidget(m_pDownBtn);

	layout->addSpacing(50);

	QLabel *label = new QLabel(QString(tr("customer_service_type")), this);
	label->setStyleSheet(TOP_WIDGET_INFO_LABEL);
	layout->addWidget(label);
	layout->addStretch();

	m_pMessageBtn = new MessageButton(this);
	m_pMessageBtn->setStyleSheet("QPushButton {background-color:transparent;}");
	layout->addWidget(m_pMessageBtn);
	layout->addSpacing(20);
	
	m_pMinimumBtn = new QPushButton(this);
	m_pMinimumBtn->setFixedSize(QSize(20, 20));
	m_pMinimumBtn->setStyleSheet(TOP_WIDGET_MIN_BTN);
	layout->addWidget(m_pMinimumBtn);
	layout->addSpacing(10);
	
	m_pMaximumBtn = new QPushButton(this);
	m_pMaximumBtn->setFixedSize(QSize(20, 20));
	m_pMaximumBtn->setStyleSheet(TOP_WIDGET_MAX_BTN);
	layout->addWidget(m_pMaximumBtn);

	m_pRestoreBtn = new QPushButton(this);
	m_pRestoreBtn->setFixedSize(QSize(20, 20));
	m_pRestoreBtn->setStyleSheet(TOP_WIDGET_RESTORE_BTN);
	layout->addWidget(m_pRestoreBtn);
	layout->addSpacing(10);
	
	m_pCloseBtn = new QPushButton(this);
	m_pCloseBtn->setFixedSize(QSize(20, 20));
	m_pCloseBtn->setStyleSheet(TOP_WIDGET_CLOSE_BTN);
	layout->addWidget(m_pCloseBtn);
	setLayout(layout);

	struct STbuttons {
		QPushButton *btn;
		TitleButtonType type;
	} buttons[] = {
		{m_pMessageBtn, TYPE_MESSAGE},
		{m_pMinimumBtn, TYPE_MIN},
		{m_pMaximumBtn, TYPE_MAX},
		{m_pRestoreBtn, TYPE_RESTORE},
		{m_pCloseBtn, TYPE_CLOSE},
	};
	for (int i = 0; i < _countof(buttons); ++i) {
		m_pButtonGroup->addButton(buttons[i].btn, buttons[i].type);
	}
	connect(m_pButtonGroup, SIGNAL(buttonClicked(int)), this,
		SLOT(OnButtonGroupSlot(int)));
	OnTimeout();
	m_timerNote = std::make_unique<QTimer>();
	m_timerNote->setInterval(10000);
	connect(m_timerNote.get(), &QTimer::timeout, this, &TopWidget::OnTimeout);
	m_timerNote->start();
}

TopWidget::~TopWidget() {
}

void TopWidget::SetMax(bool bMax) {
	m_pRestoreBtn->setVisible(bMax);
	m_pMaximumBtn->setVisible(!bMax);
}

void TopWidget::SetMoveCallback(const std::function<void(QPoint)> &fun)
{
	m_func = fun;
}

QPoint TopWidget::GetSetPosition()
{
	return QPoint();
}

void TopWidget::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		m_bLeftButtonPressed = true;
		m_startPt = event->globalPos();
		QPoint pt2 = event->pos();
		QRect rectName = m_pName->geometry();
		QRect rectDrop = m_pDownBtn->geometry();
		int left = rectName.left();
		int right = rectDrop.right();
		QRect rect = QRect(left,0,right,50);
		if (rect.contains(pt2)) {
			OnOperateClick();
		}
	}
	__super::mousePressEvent(event);
}

void TopWidget::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		m_bLeftButtonPressed = false;
	}
	__super::mouseReleaseEvent(event);
}

void TopWidget::mouseMoveEvent(QMouseEvent *event) {
	if (m_bLeftButtonPressed && (event->buttons() & Qt::LeftButton)) {
		QPoint pt = event->globalPos() - m_startPt;
		if (m_func)
			m_func(pt);

		m_startPt = event->globalPos();
	}
	__super::mouseMoveEvent(event);
}

void TopWidget::OnButtonGroupSlot(int type) {
	TitleButtonType buttontype = (TitleButtonType)type;

	switch (buttontype) {
	case TYPE_MAX:
		SetMax(true);
		break;
	case TYPE_RESTORE:
		SetMax(false);
		break;
	case TYPE_CLOSE:
		emit CloseWindow();
		break;
	default:
		break;
	}

	emit ButtonGroupSignals(buttontype);
}

void TopWidget::OnTimeout() {
	STReqData req;
	req.func = FUNC_GET_UNREAD_COUNT;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("3")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/countUnRead");

	AsynReqManager::GetInstance()->InsertReq(req);
}

void TopWidget::OnReadyData(const STRspData &stRsp) {
	switch (stRsp.func) {
		case FUNC_GET_UNREAD_COUNT: {
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(stRsp.recvBytes.data(), root)) {
				qDebug() << "parse error";
			} else {
				qDebug() << "parse success";
				if (root.isMember("result")) {
					int count = root["result"].asInt();
					m_pMessageBtn->SetNoteCount(count);
				}
			}
			
			break;
		}
	}
}

void TopWidget::OnOperateClick()
{
	if (m_pDownBtn != nullptr) {
		QMenu menu;
		menu.setStyleSheet("QMenu {border:1px solid #D3CCDF; background-color:rgb(255,255,255);}"
				"QMenu::item {background-color:transparent; color:rgb(85, 85, 85); padding:8px 20px; font-size:14px; border-bottom:1px solid #DBDBDB;}"
				"QMenu::item:selected {background-color:rgb(249,248,251); color:rgb(85, 85, 85);}");

		menu.addAction(QString(tr("change_password")), this, SLOT(OnChangePassword()));
		menu.addAction(QString(tr("essential_information")), this, SLOT(OnEssentialInfo()));
		menu.addAction(QString(tr("exit")), this, SLOT(OnExit()));
		QRect rect = m_pDownBtn->geometry();
		QPoint pt = this->mapToGlobal(rect.bottomLeft());
		menu.exec(pt);
	}
	
}

void TopWidget::OnChangePassword() {
	AlterPassword dialog(m_stUser);
	dialog.exec();
}

void TopWidget::OnEssentialInfo() {
	UserInfoDialog dialog(m_strToken, m_stUser, m_pHead->GetMd5NetPath());
	if (dialog.exec() == QDialog::Accepted) {
		STCsUser csUser = dialog.GetCsUser();
		m_stUser.stCsUser = csUser;
		emit UpdateLoginInfo(csUser);
		m_pHead->SetNetPath(QString::fromStdString(csUser.avatarUrl));
		QString name = QString::fromStdString(csUser.name) + QString(" ") + QString::fromStdString(csUser.nameEn);
		QString text = m_pName->fontMetrics().elidedText(name, Qt::ElideRight, 200, Qt::TextShowMnemonic);
		
		if (m_pName!=nullptr)
			m_pName->setText(text);
	}
}

void TopWidget::OnExit() {
	ExitDialog dialog;
	if (dialog.exec() == QDialog::Accepted) {
		emit ExitWindow();
	}
}
