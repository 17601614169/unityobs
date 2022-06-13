#include "msgbox.h"
#include <QVBoxLayout>
#include "custom-stylesheet.h"
MsgBox::MsgBox(QWidget *parent, const QString &title, const QString &text,
	       MSGBTN buttons)
	: MoveDialog(parent),m_pTitle(nullptr), m_pClose(nullptr),m_pContent(nullptr), m_pOk(nullptr), m_pCancle(nullptr)
{
	setFixedSize(424,250);
	setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	SetPixmap(":/res/images/msg-mask.png");
	QWidget *top = new QWidget(this);
	top->setFixedSize(424-76,40);
	top->setStyleSheet("border-image:url(:/res/images/cs_top_bg.png);background:transparent;");

	m_pTitle = new QLabel(top);
	m_pTitle->setFixedHeight(22);
	m_pTitle->setText(title);
	m_pTitle->setStyleSheet("background:transparent;");
	m_pClose = new QPushButton(top);
	m_pClose->setFixedSize(16,16);
	m_pClose->setStyleSheet(CLOSE_BTN);
	QHBoxLayout* hLay1 = new QHBoxLayout;
	hLay1->setContentsMargins(0,0,0,0);
	hLay1->setSpacing(0);
	hLay1->addSpacing(17);
	hLay1->addWidget(m_pTitle);
	hLay1->addStretch();
	hLay1->addWidget(m_pClose);
	hLay1->addSpacing(20);
	QVBoxLayout * vLay1 = new QVBoxLayout(top);
	vLay1->setContentsMargins(0,0,0,0);
	vLay1->setSpacing(0);
	vLay1->addSpacing(9);
	vLay1->addLayout(hLay1);
	vLay1->addSpacing(9);

	m_pContent= new QLabel(this);
	m_pContent->setStyleSheet("font-size:14px;color:#222222;font-weight:400;background:transparent;");
	m_pContent->setWordWrap(true);
	m_pContent->setFixedWidth(288);
	m_pContent->setText(text);
	m_pContent->adjustSize();

	QVBoxLayout* vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(0,0,0,0);
	vLayout->setSpacing(0);

	QHBoxLayout* hMain1 = new QHBoxLayout;
	hMain1->setContentsMargins(0,0,0,0);
	hMain1->setSpacing(0);
	hMain1->addStretch();
	hMain1->addWidget(top);
	hMain1->addStretch();

	
	
	QHBoxLayout* hLay2 = new QHBoxLayout;
	hLay2->setContentsMargins(0,0,0,0);
	hLay2->setSpacing(0);
	hLay2->addStretch();
	hLay2->addWidget(m_pContent);
	hLay2->addStretch();
	if (buttons == ENONE) {
		vLayout->addSpacing(29);
		vLayout->addLayout(hMain1);
		vLayout->addSpacing(31);
		vLayout->addLayout(hLay2);
		vLayout->addStretch();
	} else {
		vLayout->addSpacing(29);
		vLayout->addLayout(hMain1);
		vLayout->addSpacing(15);
		vLayout->addLayout(hLay2);

		m_pOk= new QPushButton(tr("ok"),this);
		m_pOk->setFixedSize(70,30);
		m_pOk->setStyleSheet(MSG_BTN_STYLE);
		m_pOk->setFocusPolicy(Qt::NoFocus);

		QHBoxLayout* hlaybtn = new QHBoxLayout;
		hlaybtn->setContentsMargins(0,0,0,0);
		hlaybtn->setSpacing(0);
		hlaybtn->addStretch();
		hlaybtn->addWidget(m_pOk);

		if (buttons == EOK_CANCLE) {
			m_pCancle= new QPushButton(tr("cancle"),this);
			m_pCancle->setFixedSize(70,30);
			m_pCancle->setStyleSheet(MSG_BTN_STYLE);
			m_pCancle->setFocusPolicy(Qt::NoFocus);
			hlaybtn->addSpacing(100);
			hlaybtn->addWidget(m_pCancle);
		}
		hlaybtn->addStretch();

		vLayout->addStretch();
		vLayout->addLayout(hlaybtn);
		vLayout->addSpacing(58);
		
	}
	setLayout(vLayout);

	auto CloseSlot = [&]() {
		m_eBtn = ECANCLE;
		close();
		emit sigClose();
	};
	auto OkSlot = [&]() {
		m_eBtn = EOK;
		close();
		emit sigClose();
	};
	auto CancleSlot = [&]() {
		m_eBtn = ECANCLE;
		close();
		emit sigClose();
	};
	connect(m_pClose,&QPushButton::clicked,CloseSlot);
	connect(m_pOk,&QPushButton::clicked,OkSlot);
	connect(m_pCancle,&QPushButton::clicked,CancleSlot);
}

MsgBox::~MsgBox()
{

}

 MSGBTN MsgBox::information(QWidget *parent, const QString &title,const QString &text, MSGBTN buttons)
{
	 MsgBox msg(parent,title,text,buttons);
	if (msg.exec()==-1)
		return ECANCLE;
	return msg.m_eBtn;
 }

 int MsgBox::GetMsgValue() {
	 return m_eBtn;
 }
