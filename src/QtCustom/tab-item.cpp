#include "tab-item.h"
#include <QHBoxLayout>
#include <QLabel>
#include <qpushbutton.h>
#include <QMouseEvent>
#include "custom-stylesheet.h"
TabItem::TabItem(TabType type,QWidget* parent) : QWidget(parent)
	,m_pIcon(nullptr)
	,m_pText(nullptr)
	,m_tabType(type)
	,m_pForBack(nullptr)
{
	setFixedSize(120,44);
	m_pBackground = new QLabel(this);

	QHBoxLayout * layout = new QHBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(0);
	layout->addSpacing(12);
	QString strIcon;
	QString strText;
	switch (m_tabType) {
	case TYPE_TAB_LIVE: {
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/zhibo.png").arg(":/res/images/url/zhibo_hover.png");
		strText = tr("TYPE_TAB_LIVE");
	}
		break;
	case TYPE_TAB_EXCHANGE:{
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/jiaoyi.png").arg(":/res/images/url/jiaoyi_hover.png");
		strText = tr("TYPE_TAB_EXCHANGE");
	}
		break;
	case TYPE_TAB_RED_PACKET:{
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/hongbao_edu.png").arg(":/res/images/url/hongbao_edu_hover.png");
		strText = tr("TYPE_TAB_RED_PACKET");
	}
		break;
	case TYPE_TAB_DATA_STATIC:{
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/tongji.png").arg(":/res/images/url/tongji_hover.png");
		strText = tr("TYPE_TAB_DATA_STATIC");
	}
		break;
	case TYPE_TAB_LIVE_DETAIL:{
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/mingxi.png").arg(":/res/images/url/nmingxi_hover.png");
		strText = tr("TYPE_TAB_LIVE_DETAIL");
	}
		break;
	case TYPE_TAB_KNOWLEDGE:{
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/zhishi.png").arg(":/res/images/url/zhishi_hover.png");
		strText = tr("TYPE_TAB_KNOWLEDGE");
	}
		break;
	case TYPE_TAB_QUICK_REPLY:{
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/quick_reply.png").arg(":/res/images/url/quick_reply_hover.png");
		strText = tr("TYPE_TAB_QUICK_REPLY");
	}
		break;
	case TYPE_IM: {
		strIcon=TAB_URL_STYLE.arg(":/res/images/url/quick_reply.png").arg(":/res/images/url/quick_reply_hover.png");
		strText = tr("TEST IM");
	}break;
	case TYPE_EVALUTE: {
		strIcon=TAB_URL_STYLE.arg(":/res/images/evaluate.png").arg(":/res/images/evaluate_hover.png");
		strText = tr("TYPE_EVALUEATE");
	}break;
	}
	m_pIcon = new QLabel(m_pBackground);
	m_pIcon->setFixedSize(18,18);
	m_pIcon->setAlignment(Qt::AlignLeft);
	m_pIcon->setStyleSheet(strIcon);
	
	m_pText = new QLabel(m_pBackground);
	m_pText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_pText->setText(strText);
	m_pText->setStyleSheet("QLabel{color:#CACBD0;font-size:12px;background:transparent;}"
		"QLabel:hover{color:#FFFFFF;}"
		"QLabel:checked{color:#FFFFFF;}"
		"QLabel:enabled{color:#FFFFFF;}"
		"QLabel:disabled{color:#CACBD0;}");
	m_pForBack = new QPushButton("<",m_pBackground);
	m_pForBack->setFixedSize(15,15);
	m_pForBack->setStyleSheet("QPushButton{background:transparent;border:none;}QPushButton:hover{color:white;}");
	layout->addWidget(m_pIcon);
	layout->addSpacing(10);
	layout->addWidget(m_pText);
	layout->addStretch();
	layout->addWidget(m_pForBack);
	layout->addSpacing(5);
	m_pBackground->setLayout(layout);
	auto forBackSlot = [&]() {
		emit forBack(m_tabType);
	};
	connect(m_pForBack,&QPushButton::clicked,forBackSlot);
	m_pForBack->hide();
	QHBoxLayout* main = new QHBoxLayout;
	main->setContentsMargins(0,0,0,0);
	main->setSpacing(0);
	main->addWidget(m_pBackground);
	this->setLayout(main);
}

TabItem::~TabItem() {

}

void TabItem::SetChecked(const TabType& type) {
	m_pIcon->setEnabled(m_tabType==type);
	m_pText->setEnabled(m_tabType==type);
	if (m_tabType == type) {
		m_pBackground->setStyleSheet("background:#1D212E;");
	} else {
		m_pBackground->setStyleSheet("background:transparent;");
	}
}

TabType TabItem::GetType() {
	return m_tabType;
}

void TabItem::mousePressEvent(QMouseEvent* event) {
	emit tabClick(m_tabType);
	__super::mousePressEvent(event);
}
