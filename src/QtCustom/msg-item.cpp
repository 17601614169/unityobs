#include "msg-item.h"
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QDesktopServices>
#include "custom-stylesheet.h"
#include "url-label.h"
#include "custom-config-ini.h"
#include "sacrouge-dialog.h"
#include "msgbox.h"
static int m = 1;
MsgItem::MsgItem(const std::string token,const STMessageBody &stBody, QWidget *parent, ContentType type)
	: QWidget(parent),
	  m_pNormalMsg(nullptr),
	  m_pUserName(nullptr),
	  m_pGiftText(nullptr),
	  m_pGiftIcon(nullptr),
	  m_pGiftNum(nullptr),
	  m_strToken(token)
{
	m_stBody = stBody;
	setMinimumWidth(276);
	setMaximumWidth(276);
	if (type == TYPE_CHAT) {
		QVBoxLayout *vLayout = new QVBoxLayout;
		vLayout->setContentsMargins(0, 0, 0, 0);
		vLayout->setSpacing(0);
		QHBoxLayout *lay = new QHBoxLayout();
		lay->setContentsMargins(0, 0, 0, 0);
		lay->setSpacing(0);
		m_pNormalMsg = new QTextBrowser(this);
		m_pNormalMsg->setFixedWidth(276);
		m_pNormalMsg->setReadOnly(true);
		m_pNormalMsg->setOpenLinks(true);
		m_pNormalMsg->setFocusPolicy(Qt::NoFocus);
		m_pNormalMsg->setOpenExternalLinks(false);
		m_pNormalMsg->setContextMenuPolicy(Qt::NoContextMenu);
		m_pNormalMsg->setWordWrapMode(QTextOption::WrapAnywhere);
		m_pNormalMsg->setStyleSheet(MSG_BROWSER);
		m_pNormalMsg->setVerticalScrollBarPolicy(
			Qt::ScrollBarAlwaysOff);
		m_pNormalMsg->installEventFilter(this);
		//m_pNormalMsg->setEnabled(false);
		QString strNameFormat =
			"<a href='%1' style='line-height:30px;text-decoration:none;color:#C3BEE0;font-weight:400;font-size:12px;'>%2</a>";
		QString strMsgFormat =
			"<span style='line-height:30px; color:#FFFFFF;font-weight:400;font-size:12px;'>%1</span>";
		QString strText;
		strText = strNameFormat.arg(QString::fromStdString(stBody.fromUserName))
				  .arg(QString::fromStdString(stBody.fromUserName)+QString(": ")) +
			  strMsgFormat.arg(QString::fromStdString(stBody.content));
		m_pNormalMsg->setText(strText);
		m_strPlainMsg = strText;
		QString text = m_pNormalMsg->toPlainText();

		QFont font;
		font.setPixelSize(12);
		QFontMetrics metrics(font);
		int w = metrics.width(text);
		int lines = (w - 1) / 276 + 1;
		int height = lines == 1 ? 30 : lines * 20;

		m_pNormalMsg->setVerticalScrollBarPolicy(
			Qt::ScrollBarAlwaysOff);
		m_pNormalMsg->setHorizontalScrollBarPolicy(
			Qt::ScrollBarAlwaysOff);
		m_pNormalMsg->setFixedSize(QSize(276, height));
		connect(m_pNormalMsg, &QTextBrowser::anchorClicked, this,
			&MsgItem::OnAnchorClicked);
		lay->addWidget(m_pNormalMsg);

		vLayout->addLayout(lay);
		vLayout->addSpacing(3);
		setLayout(vLayout);
		this->setFixedSize(QSize(m_pNormalMsg->width(),
					 m_pNormalMsg->height() + 3));
	} else if (type == TYPE_GIFT) {
		QHBoxLayout *hlay1 = new QHBoxLayout(this);
		hlay1->setContentsMargins(0, 0, 0, 0);
		hlay1->setSpacing(0);

		QLabel *bg = new QLabel(this);
		bg->setStyleSheet(MSG_LINEAR_GRADIENT);
		bg->setFixedSize(264,37);
		QHBoxLayout *hlay = new QHBoxLayout;
		hlay->setContentsMargins(0, 0, 0, 0);
		hlay->setSpacing(0);
		m_pUserName = new QLabel(bg);
		m_pUserName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		m_pUserName->setFixedHeight(30);
		m_pUserName->setStyleSheet(MSG_USER_NAME);
		m_pUserName->setText(QString::fromStdString(m_stBody.fromUserName));
		m_pGiftText = new QLabel(bg);
		m_pGiftText->setFixedHeight(30);
		m_pGiftText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		m_pGiftText->setStyleSheet(MSG_GIFT_TEXT);
		QString strGift = QString(tr("send-gift")) + QString::fromStdString(m_stBody.stSendGift.giftName);
		m_pGiftText->setText(strGift);
		m_pGiftIcon = new QUrlLabel(bg);
		m_pGiftIcon->setFixedSize(35, 35);
		RequestTask task;
		task.url = QString::fromStdString(m_stBody.stSendGift.giftUrl);
		task.type = TYPE_DOWNLOAD;
		m_pGiftIcon->SetTask(task);
		
		m_pGiftIcon->setStyleSheet(MSG_GIFT_ICON);
		m_pGiftNum = new QLabel(bg);
		m_pGiftNum->setFixedHeight(30);
		m_pGiftNum->setStyleSheet(MSG_GIFT_NUM);
		m_pGiftNum->setText(QString("x") +
				    QString::number(m_stBody.stSendGift.giftNum));
		hlay->addSpacing(8);
		hlay->addWidget(m_pUserName);
		hlay->addSpacing(4);
		hlay->addWidget(m_pGiftText);
		hlay->addSpacing(4);
		hlay->addWidget(m_pGiftIcon);
		hlay->addSpacing(4);
		hlay->addWidget(m_pGiftNum);
		hlay->addStretch();
		bg->setLayout(hlay);
		hlay1->addSpacing(1);
		hlay1->addWidget(bg);
		hlay1->addSpacing(1);
		this->setFixedHeight(37);
	} else if (type == TYPE_RED_PACKET) {
		QHBoxLayout *hlay1 = new QHBoxLayout(this);
		hlay1->setContentsMargins(0, 0, 0, 0);
		hlay1->setSpacing(0);

		QLabel *bg = new QLabel(this);
		bg->setStyleSheet(MSG_LINEAR_GRADIENT);
		bg->setFixedSize(264, 37);
		QHBoxLayout *hlay = new QHBoxLayout;
		hlay->setContentsMargins(0, 0, 0, 0);
		hlay->setSpacing(0);
		m_pUserName = new QLabel(bg);
		m_pUserName->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		m_pUserName->setFixedHeight(30);
		m_pUserName->setStyleSheet(MSG_USER_NAME);
		m_pUserName->setText(
			QString::fromStdString(m_stBody.fromUserName));
		m_pGiftText = new QLabel(bg);
		m_pGiftText->setFixedHeight(30);
		m_pGiftText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		m_pGiftText->setStyleSheet(MSG_GIFT_TEXT);
		QString strGift =
			QString(tr("send-red-packet"));
		m_pGiftText->setText(strGift);

		m_pGiftIcon = new QUrlLabel(bg);
		m_pGiftIcon->setFixedSize(19, 25);
		m_pGiftIcon->setStyleSheet(MSG_GIFT_RED_PACKET_ICON);
		m_pGiftNum = new QLabel(bg);
		m_pGiftNum->setFixedHeight(30);
		m_pGiftNum->setStyleSheet(MSG_GIFT_NUM);
		m_pGiftNum->setText(
			QString("x") +
			QString::number(m_stBody.stRedPacket.totalAmount));
		hlay->addSpacing(8);
		hlay->addWidget(m_pUserName);
		hlay->addSpacing(4);
		hlay->addWidget(m_pGiftText);
		hlay->addSpacing(4);
		hlay->addWidget(m_pGiftIcon);
		hlay->addSpacing(4);
		hlay->addWidget(m_pGiftNum);
		hlay->addStretch();
		bg->setLayout(hlay);
		hlay1->addSpacing(1);
		hlay1->addWidget(bg);
		hlay1->addSpacing(1);
		this->setFixedHeight(37);
	}
	if (m_pGiftIcon!=nullptr)
		m_pGiftIcon->installEventFilter(this);
}

MsgItem::~MsgItem() {

}

void MsgItem::OnAnchorClicked(const QUrl& url) {
	if (m_pNormalMsg != nullptr) {
		qDebug() << "url:" << url;
		m_pNormalMsg->setText(m_strPlainMsg);
		QString strText = QString(tr("is do-black")) + url.toString();
		
		MSGBTN btn = MsgBox::information(
			NULL, QString(tr("do-black")), strText,
			EOK_CANCLE);
		//拉入黑名单
		//这个接口很奇怪
		//url格式:http://47.103.124.157:8304/app/user/anchor/v1/doBlacklist?roomId=35075&type=1&blackNo=20135293
		if (btn == EOK) {
			emit doBlackList(m_stBody);
		}
	}
}

bool MsgItem::eventFilter(QObject* obj, QEvent* event) {
	if (m_pNormalMsg != nullptr) {
		if (obj == m_pNormalMsg) {
			if (event->type() == QEvent::Wheel) {
				
			}
		}
	}
	if (m_pGiftIcon != nullptr) {
		if (obj == m_pGiftIcon) {
			if (event->type() == QEvent::MouseButtonPress) {
				OnGiftClicked();
			}
		}
	}
	return QWidget::eventFilter(obj,event);
}

void MsgItem::OnGiftClicked() {
	if (m_stBody.contentType == TYPE_RED_PACKET) {
		RedDetailsDialog dialog(m_stBody.stRedPacket.id, m_strToken, this);
		dialog.exec();
	}
}
