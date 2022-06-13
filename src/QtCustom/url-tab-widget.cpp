#include "url-tab-widget.h"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QVBoxLayout>
#include "util/base.h"
const QString strListStye = QString("QListWidget{border:none;padding:0px;padding-top:0px;padding-left:0px;"
	"margin:0px;"
	"outline:0px;"
	"background:#393E50;"
	"border-radius:2px;}"
	"QListWidget::Item{padding:0px 0px 0px 0px;margin:0px;background:#393E50;}"
	"QListWidget::Item:hover{background:#323645;padding:0 0px;border:none;}"
	"QListWidget::item:selected{background:#323645;padding:0 0px;border:none;}"
	"QListWidget::item:selected:!active{background:#323645;padding:0 0px;}");

UrlTabWidget::UrlTabWidget(QWidget *parent)
	: QWidget(parent)
	,m_pTab1(nullptr)
	,m_pTab2(nullptr)
	,m_pTab3(nullptr)
	,m_pTab4(nullptr)
	,m_pTab5(nullptr)
	,m_pTab6(nullptr)
	,m_pTab7(nullptr)
	,m_pTab8(nullptr)
	,m_pTab9(nullptr)
{
	setFixedWidth(120);
	QVBoxLayout *v = new QVBoxLayout();
	v->setContentsMargins(0,0,0,0);
	v->setSpacing(0);
	QWidget *centerWidget = new QWidget(this);
	centerWidget->setFixedWidth(120);
	centerWidget->setStyleSheet("background:#393E50;");
	m_pTab1 = new TabItem(TYPE_TAB_LIVE,centerWidget);
	m_pTab2 = new TabItem(TYPE_TAB_EXCHANGE,centerWidget);
	m_pTab3 = new TabItem(TYPE_TAB_RED_PACKET,centerWidget);
	m_pTab4 = new TabItem(TYPE_TAB_DATA_STATIC,centerWidget);
	m_pTab5 = new TabItem(TYPE_TAB_LIVE_DETAIL,centerWidget);
	m_pTab6 = new TabItem(TYPE_TAB_KNOWLEDGE,centerWidget);
	m_pTab7 = new TabItem(TYPE_TAB_QUICK_REPLY,centerWidget);
	m_pTab8 = new TabItem(TYPE_EVALUTE, centerWidget);
	connect(m_pTab1,&TabItem::tabClick,this,&UrlTabWidget::OnTabClicked);
	connect(m_pTab2,&TabItem::tabClick,this,&UrlTabWidget::OnTabClicked);
	connect(m_pTab3,&TabItem::tabClick,this,&UrlTabWidget::OnTabClicked);
	connect(m_pTab4,&TabItem::tabClick,this,&UrlTabWidget::OnTabClicked);
	connect(m_pTab5,&TabItem::tabClick,this,&UrlTabWidget::OnTabClicked);
	connect(m_pTab6,&TabItem::tabClick,this,&UrlTabWidget::OnTabClicked);
	connect(m_pTab7,&TabItem::tabClick,this,&UrlTabWidget::OnTabClicked);
	connect(m_pTab8, &TabItem::tabClick, this, &UrlTabWidget::OnTabClicked);
	m_pTab1->setFixedSize(120,44);
	m_pTab2->setFixedSize(120,44);
	m_pTab3->setFixedSize(120,44);
	m_pTab4->setFixedSize(120,44);
	m_pTab5->setFixedSize(120,44);
	m_pTab6->setFixedSize(120,44);
	m_pTab7->setFixedSize(120,44);
	m_pTab8->setFixedSize(120, 44);
	QVBoxLayout* vCenter = new QVBoxLayout();
	vCenter->setContentsMargins(0,0,0,0);
	vCenter->setSpacing(0);
	vCenter->addWidget(m_pTab1);
	vCenter->addWidget(m_pTab2);
	vCenter->addWidget(m_pTab3);
	vCenter->addWidget(m_pTab4);
	vCenter->addWidget(m_pTab5);
	vCenter->addWidget(m_pTab8);
	vCenter->addWidget(m_pTab6);
	vCenter->addWidget(m_pTab7);
	vCenter->addStretch();
	centerWidget->setLayout(vCenter);
#if TEST_IM_ENABLED
	m_pTab9 = new TabItem(TYPE_IM,m_pUrlList);
	item = new QListWidgetItem(m_pUrlList);
	item->setSizeHint(QSize(120,44));
	m_pUrlList->setItemWidget(item,m_pTab9);
#endif
#if TEST_URL_ENABLED
	
#endif
	v->addWidget(centerWidget);
	this->setLayout(v);
	CheckedItem(TYPE_TAB_LIVE);

	auto ForBackSlot = [&](const TabType & type) {
		emit forBack(type);
	};
	connect(m_pTab6,&TabItem::forBack,ForBackSlot);
#if _DEBUG
#else
	m_pTab1->setVisible(false);
	m_pTab2->setVisible(false);
	m_pTab3->setVisible(false);
	m_pTab4->setVisible(false);
	m_pTab5->setVisible(false);
	m_pTab6->setVisible(false);
	m_pTab7->setVisible(false);
#endif
}

UrlTabWidget::~UrlTabWidget() {

}

void UrlTabWidget::CheckedItem(const TabType& type) {
	m_pTab1->SetChecked(type);
	m_pTab2->SetChecked(type);
	m_pTab3->SetChecked(type);
	m_pTab4->SetChecked(type);
	m_pTab5->SetChecked(type);
	m_pTab6->SetChecked(type);
	m_pTab7->SetChecked(type);
	m_pTab8->SetChecked(type);
}

void UrlTabWidget::OnTabClicked(const TabType & type){
	CheckedItem(type);
	emit UrlTabSignal(type);
}

void UrlTabWidget::SetItemWidgetSelected(const TabType& type) {
#if 0
	for (int i = 0; i < m_pUrlList->count(); ++i) {
		QListWidgetItem *item = m_pUrlList->item(i);
		TabItem* widget = dynamic_cast<TabItem*>(m_pUrlList->itemWidget(item));
		if (widget != nullptr) {
			TabType type2 = widget->GetType();
			if (type == type2) {
				m_pUrlList->setItemSelected(item,true);
				CheckedItem(type);
				break;
			}
		}
	}
#else
	CheckedItem(type);
#endif
}

void UrlTabWidget::OnItemClicked(QListWidgetItem* item) {
	/*TabItem* widget = dynamic_cast<TabItem*>(m_pUrlList->itemWidget(item));
	if (widget != nullptr) {
		TabType type = widget->GetType();
		CheckedItem(type);
		emit UrlTabSignal(type);
	}*/
}

void UrlTabWidget::SetMenuList(std::vector<STMenuItem>& vecItems) {
	std::string menus;
	std::map<std::string,int> mapItems;
	blog(LOG_INFO,"menu size %d",vecItems.size());
	for (int i = 0; i < vecItems.size(); i++) {
		mapItems[vecItems[i].code]=1;
		blog(LOG_INFO,"menu item:%s",vecItems[i].code.c_str());
	}
#if 1
	std::map<std::string, int>::iterator iter1 = mapItems.find("ac_workbench_live_room");
	m_pTab1->setVisible(iter1 != mapItems.end());
	std::map<std::string, int>::iterator iter2 = mapItems.find(AC_WORKBENCH_TRADE_LOG);
	m_pTab2->setVisible(iter2 != mapItems.end());
	std::map<std::string, int>::iterator iter3 = mapItems.find(AC_WORKBENCH_RED_QUOTA);
	m_pTab3->setVisible(iter3 != mapItems.end());

	std::map<std::string, int>::iterator iter4 = mapItems.find(AC_WORKBENCH_LIVE_STATS);
	m_pTab4->setVisible(iter4 != mapItems.end());

	std::map<std::string, int>::iterator iter5 = mapItems.find(AC_WORKBENCH_LIVE_DETAIL);
	m_pTab5->setVisible(iter5 != mapItems.end());
	std::map<std::string, int>::iterator iter6 = mapItems.find("ac_workbench_knowledge");
	m_pTab6->setVisible(iter6 != mapItems.end());
	std::map<std::string, int>::iterator iter7 = mapItems.find(AC_WORKBENCH_QUICK_REPLY);
	m_pTab7->setVisible(iter7 != mapItems.end());
	std::map<std::string, int>::iterator iter8 = mapItems.find(AC_WORKBENCH_EVALUATE);
	m_pTab8->setVisible(iter8 != mapItems.end());
#else
	m_pTab2->hide();
	m_pTab3->hide();
	m_pTab4->hide();
	m_pTab5->hide();
	m_pTab6->hide();
#endif
	TabType tt = TYPE_TAB_NONE;
	if (iter1 != mapItems.end()) {
		tt = TYPE_TAB_LIVE;
	} else if (iter2 != mapItems.end()) {
		tt = TYPE_TAB_EXCHANGE;
	}else if (iter3 != mapItems.end()) {
		tt = TYPE_TAB_RED_PACKET;
	}else if (iter4 != mapItems.end()) {
		tt = TYPE_TAB_DATA_STATIC;
	}else if (iter5 != mapItems.end()) {
		tt = TYPE_TAB_LIVE_DETAIL;
	}else if (iter6 != mapItems.end()) {
		tt = TYPE_TAB_KNOWLEDGE;
	} else if (iter7 != mapItems.end()) {
		tt = TYPE_TAB_QUICK_REPLY;
	} else if (iter8 != mapItems.end()) {
		tt = TYPE_EVALUTE;
	}
#if _DEBUG
#else
	OnTabClicked(tt);
#endif
}
