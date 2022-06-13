#include "im-widget.h"
#include "custom-stylesheet.h"
#include "msg-item.h"
#include "callback-request.h"
#include <QEvent>
#include <QDebug>
#include <QTimer>
#include <QKeyEvent>
#include <QTextBlock>
#include <QCheckBox>
#include "left-right-button.h"
#include "characters-convert.h"
#include "json-parser.h"
#include "util/base.h"

#include <QTextCodec>

IMWidget::IMWidget(QWidget *parent)
	: BaseWidget(parent),
	  m_pCallbackFill(nullptr),
	  m_timerList(nullptr),
	  m_pNoticeInput(nullptr)
{
	setObjectName("imwidget_bg");
	setStyleSheet(IM_WIDGET_BG);

	setWindowFlags(windowFlags()|Qt::FramelessWindowHint);
	m_playout = new QVBoxLayout(this);
	m_playout->setContentsMargins(0, 10, 0, 0);
	m_playout->setSpacing(0);

	m_pTest = new QWidget(this);
	m_pTest->setFixedHeight(180);
	m_pGroup = new QButtonGroup(m_pTest);
	m_pBegin = new QPushButton("begin",m_pTest);
	m_pToken = new QPushButton("token", m_pTest);
	m_pConn = new QPushButton("connect", m_pTest);
	m_pSubscribe = new QPushButton("subscribe", m_pTest);
	m_pUnSubscribe = new QPushButton("unsubscribe", m_pTest);
	m_pSend = new QPushButton("send", m_pTest);
	m_pFinish = new QPushButton("finish", m_pTest);
	QVBoxLayout *vlay = new QVBoxLayout;
	vlay->setContentsMargins(0, 0, 0, 0);
	vlay->setSpacing(0);
	struct STbuttons {
		QPushButton *btn;
		int type;
	} buttons[] = {
		{m_pBegin, 1},
		{m_pToken, 2},
		{m_pConn, 3},
		{m_pSubscribe, 4},
		{m_pUnSubscribe, 5},
		{m_pSend, 6},
		{m_pFinish, 7},
	};
	for (int i = 0; i < _countof(buttons); ++i) {
		//去除焦点获取时的虚影(键盘上下左右会产生)
		buttons[i].btn->setStyleSheet("color:white;");
		buttons[i].btn->setFixedHeight(30);
		buttons[i].btn->setFocusPolicy(Qt::NoFocus);
		buttons[i].btn->setCheckable(false);
		m_pGroup->addButton(buttons[i].btn, buttons[i].type);
		vlay->addWidget(buttons[i].btn);
	}
	m_pTest->setLayout(vlay);
	connect(m_pGroup, SIGNAL(buttonClicked(int)), this,
		SLOT(OnTestGroup(int)));
	m_playout->addWidget(m_pTest);
	//公告
	m_pNotice = new QWidget(this);
	m_pNotice->setFixedHeight(22);
	QHBoxLayout *hNotice = new QHBoxLayout();
	hNotice->setContentsMargins(0, 0, 0, 0);
	hNotice->setSpacing(0);
	m_pNotice = new QWidget(this);
	m_pNoticeIcon = new QLabel(m_pNotice);
	m_pNoticeIcon->setFixedSize(22,22);
	m_pNoticeIcon->setStyleSheet(IM_WIDGET_NOTICE_ICON);
	m_pNoticeInput = new QLineEdit(m_pNotice);
	m_pNoticeInput->setPlaceholderText(QString(tr("room notice")));
	m_pNoticeInput->setFixedHeight(17);
	m_pNoticeInput->setText(QString(tr("room notice")));
	m_pNoticeInput->setStyleSheet(IM_WIDGET_NOTICE_TEXT);
	m_pNoticeInput->setMinimumWidth(180);
	m_pNoticeInput->setMaximumWidth(200);
	m_pNoticeInput->setReadOnly(true);
	m_pNoticeBtn = new QPushButton(m_pNotice);
	m_pNoticeBtn->setFixedSize(14, 14);
	m_pNoticeBtn->setStyleSheet(IM_WIDGET_NOTICE_BTN);
	hNotice->addSpacing(13);
	hNotice->addWidget(m_pNoticeIcon);
	hNotice->addSpacing(5);
	hNotice->addWidget(m_pNoticeInput);
	hNotice->addSpacing(6);
	hNotice->addWidget(m_pNoticeBtn);
	hNotice->addStretch();
	hNotice->addSpacing(10);
	m_playout->addSpacing(10);
	m_playout->addLayout(hNotice);
	m_pNoticeInput->installEventFilter(this);
	//系统提示
	QHBoxLayout *hsystem = new QHBoxLayout();
	hsystem->setContentsMargins(0, 0, 0, 0);
	hsystem->setSpacing(0);
	m_pSystemTip = new QTextEdit(this);
	m_pSystemTip->setStyleSheet(IM_WIDGET_SYSTEM_TIP);
	m_pSystemTip->setFixedSize(263,100);
	m_pSystemTip->setText(QString(tr("system tip")));
	m_pSystemTip->setEnabled(false);
	QTextDocument *pDoc = nullptr;
	pDoc = m_pSystemTip->document();
	if (pDoc != nullptr) {
		QTextBlock block = pDoc->lastBlock();
		QTextCursor tc(block);
		QTextBlockFormat fmt = block.blockFormat();
		fmt.setLineHeight(6, QTextBlockFormat::LineDistanceHeight);
		tc.setBlockFormat(fmt);
	}
	m_pSystemTip->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pSystemTip->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	

	hsystem->addSpacing(14);
	hsystem->addWidget(m_pSystemTip);
	hsystem->addSpacing(14);
	m_playout->addSpacing(10);
	m_playout->addLayout(hsystem);
	//重连服务器
	m_pReconServer = new QLabel(QString(tr("RECONN_SERVER")),this);
	m_pReconServer->setFixedHeight(17);
	m_pReconServer->setStyleSheet(IM_WIDGET_RECONECT);
	QHBoxLayout *conLay = new QHBoxLayout();
	conLay->setContentsMargins(0, 0, 0, 0);
	conLay->setSpacing(0);
	conLay->addSpacing(14);
	conLay->addWidget(m_pReconServer);
	conLay->addStretch();
	m_playout->addSpacing(5);
	m_playout->addLayout(conLay);
	//list
	QHBoxLayout *list = new QHBoxLayout;
	list->setContentsMargins(0, 0, 0, 0);
	list->setSpacing(0);
	m_pList = new QListWidget(this);
	m_pList->setFocusPolicy(Qt::NoFocus);
	m_pList->setAutoScroll(true);
	m_pList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_pList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pList->setFocusPolicy(Qt::NoFocus);
	m_pList->setContentsMargins(0, 0, 0, 0);
	m_pList->setFixedWidth(276);
	m_pList->setStyleSheet(IM_WIDGET_LIST);
	m_pList->setSpacing(4);
	list->addSpacing(14);
	list->addWidget(m_pList);
	list->addSpacing(5);
	m_playout->addLayout(list);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setContentsMargins(17, 25, 0, 24);
	horLayout->setSpacing(22);

	QCheckBox *checkbox = new QCheckBox(this);
	checkbox->setStyleSheet(IM_WIDGET_SHIELD);
	horLayout->addWidget(checkbox);

	auto redClick = [&] { emit ShowRedEnvelops(); };

	QPushButton *redBtn = new QPushButton(this);
	redBtn->setFixedSize(19, 25);
	redBtn->setStyleSheet(IM_WIDGET_RED_BTN);
	connect(redBtn, &QPushButton::clicked, redClick);
	horLayout->addWidget(redBtn);
	horLayout->addStretch();

	m_playout->addLayout(horLayout);

	connect(m_pNoticeBtn,SIGNAL(clicked()),this,SLOT(OnEditBtnClicked()));
	m_timerList = new QTimer(this);
	m_timerList->setInterval(5);
	
	connect(m_timerList, &QTimer::timeout, this, &IMWidget::OnTimerOut);
	m_timerList->start();
#if 1
	m_pTest->hide();
#endif
	m_pReconServer->hide();
}

IMWidget::~IMWidget() {
	if (m_pCallbackFill != nullptr) {
		m_pCallbackFill->wait();
		m_pCallbackFill->deleteLater();
		m_pCallbackFill = nullptr;
	}
}

void IMWidget::SetLoginToken(const std::string token) {
	m_strToken = token;
}

void IMWidget::OnTestGroup(int btn) {
	emit testSignals(btn);
}

void IMWidget::OnEditBtnClicked() {
	m_pNoticeInput->setReadOnly(false);
	m_pNoticeInput->selectAll();
	m_pNoticeInput->setFocus();
	m_strText = m_pNoticeInput->text();
}

bool IMWidget::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == m_pNoticeInput) {	
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->key() == Qt::Key_Enter ||
			    keyEvent->key() == Qt::Key_Return) {
				m_pNoticeInput->setReadOnly(true);
				QString strText = m_pNoticeInput->text();
				SetNotice(m_pNoticeInput->text());
			}
		} else if (event->type() == QEvent::FocusOut) {
			m_pNoticeInput->setReadOnly(true);
		}
	}
	return QWidget::eventFilter(obj, event);
}

void IMWidget::DoBlackList(const STMessageBody & stBody)
{
	qDebug() << "DoBlackList";
	/*
	* packet req
	* 这个接口很奇怪POST却用get方式组包
	*/
	STReqData req;
	req.func = FUN_DO_BACKLIST;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(QByteArray("terminalType"), QByteArray("1")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	//下列直播id等其他信息目前只是测试
	sprintf(req.url,
		"%s/app/user/anchor/v1/doBlacklist?roomId=%d&type=1&blackNo=%s",
		BASE_URL.c_str(), m_stBeginRsp.roomId,stBody.fromUserId.c_str());
	qDebug() << "url:" << req.url;
	blog(LOG_INFO, "request %s", req.url);
	if (m_pCallbackFill == nullptr) {
		auto callback_ = std::bind(&IMWidget::RspFill, this,
					   std::placeholders::_1,
					   std::placeholders::_2,
					   std::placeholders::_3);
		m_pCallbackFill = new CallbackThread(callback_,req);
	} else {
		m_pCallbackFill->SetReqData(req);
	}
}

void IMWidget::SetBeginLive(const STBeginRsp &rsp) {
	m_stBeginRsp = rsp;
}

void IMWidget::SetRoomInfo(const STRoomInfo& info) {
	if (m_pNoticeInput != nullptr) {
		QString strText = m_pNoticeInput->fontMetrics().elidedText(
			QString::fromStdString(info.notice), Qt::ElideRight,
			m_pNoticeInput->width());
		SetNoticeText(strText);
	}
}

void IMWidget::SetNoticeText(const QString &text) {
	m_pNoticeInput->setText(text);
}

void IMWidget::SetConnected(bool con) {
	m_pReconServer->setVisible(!con);
}

void IMWidget::SetNotice(const QString &text)
{
	STReqData req;
	req.func = FUNC_NOTICE;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(QByteArray("terminalType"), QByteArray("1")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));

	std::string api = "/app/room/v1/notice?notice=";
	QTextCodec* codec = QTextCodec::codecForName("GBK");
	std::string str1 = m_pNoticeInput->text().toStdString();
	std::string str2 = CharatersConvert::UTF8ToGBK(str1);
	sprintf(req.url, "%s%s%s", BASE_URL.c_str(), api.c_str(), str2.c_str());
	blog(LOG_INFO, "request %s", req.url);
	if (m_pCallbackFill == nullptr) {
		auto callback_ = std::bind(&IMWidget::RspFill, this,
					   std::placeholders::_1,
					   std::placeholders::_2,
					   std::placeholders::_3);
		m_pCallbackFill = new CallbackThread(callback_, req);
	} else {
		m_pCallbackFill->SetReqData(req);
	}
}

void IMWidget::RspFill(const QByteArray &bytes,
		       const QNetworkReply::NetworkError &error,
		       const QVariant &req)
{
	if (!bytes.isEmpty() && error == QNetworkReply::NoError) {
		STReqData reqData = qvariant_cast<STReqData>(req);
		if (reqData.func == FUN_DO_BACKLIST) {
			STDoBacklistRsp rsp;
			rsp = ParseDoBacklist(bytes);
			QString strMsg = QString::fromStdString(rsp.appMsg);
		} else if (reqData.func == FUNC_NOTICE) {
			STNoticeRsp rsp;
			rsp = ParseNotice(bytes);
			SetNoticeText(QString::fromStdString(rsp.notice));
		}
	}
}

void IMWidget::InsertMessageBody(const STMessageBody &stBody)
{
	if (stBody.contentType != 0) {
		if (stBody.contentType == TYPE_CHAT ||
		    stBody.contentType == TYPE_GIFT ||
		    stBody.contentType == TYPE_RED_PACKET) {
			std::lock_guard<std::mutex> lock(m_mutexMsg);
			m_listMsg.push_back(stBody);
		}
	}
}

void IMWidget::OnTimerOut() {
	if (m_listMsg.size() > 0) {
		STMessageBody stBody;
		{
			std::lock_guard<std::mutex> lock(m_mutexMsg);
			stBody = m_listMsg.takeAt(0);
		}
		if (stBody.contentType != 0) {
			if (stBody.contentType == TYPE_CHAT ||
			    stBody.contentType == TYPE_GIFT ||
			    stBody.contentType == TYPE_RED_PACKET) {
				QListWidgetItem *listItem = new QListWidgetItem();
				MsgItem *item = new MsgItem(m_strToken, stBody, m_pList, (ContentType)stBody.contentType);
				connect(item, &MsgItem::doBlackList, this,
					&IMWidget::DoBlackList);
				m_pList->addItem(listItem);
				listItem->setSizeHint(QSize(m_pList->width(),
							    item->height()));
				m_pList->setItemWidget(listItem, item);
				m_pList->scrollToBottom();
			}
		}
	}
}
/*
* QListWidgetItem* date = new QListWidgetItem();
* CustomItem* item = new CustomItem(list);
* listWidget->addItem(date);
* date->setSizeHint(dateItem->size());
* listWidget->setItemWidget(date, dateItem);
*/
