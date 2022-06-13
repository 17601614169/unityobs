#ifndef URL_TAB_WIDGET_H
#define URL_TAB_WIDGET_H
#include <QWidget>
class QPushButton;
class QLabel;
class QListWidget;
#include <QListWidgetItem>
#include "qtcustom_global.h"
#include "tab-item.h"
#include "base-define.h"


/**我的直播间*/
const std::string AC_WORKBENCH_LIVE_ROOM = "ac_workbench_live_room";
/**交易记录*/
const std::string AC_WORKBENCH_TRADE_LOG = "ac_workbench_trade_log";
/**红包额度记录*/
const std::string AC_WORKBENCH_RED_QUOTA = "ac_workbench_red_quota";
/**直播数据统计*/
const std::string AC_WORKBENCH_LIVE_STATS = "ac_workbench_live_stats";
/**直播明细*/
const std::string AC_WORKBENCH_LIVE_DETAIL = "ac_workbench_live_detail";
/**知识库*/
const std::string AC_WORKBENCH_KNOWLEDGE = "ac_workbench_knowledge";
/**快捷回复*/
const std::string AC_WORKBENCH_QUICK_REPLY = "ac_workbench_quick_reply";
/**直播评价*/
const std::string AC_WORKBENCH_EVALUATE = "ac_workbench_live_evaluateList";

class QTCUSTOM_EXPORT UrlTabWidget : public QWidget {
	Q_OBJECT
public:
	UrlTabWidget(QWidget* parent=0);
	~UrlTabWidget();
	void CheckedItem(const TabType& type) ;
	void SetItemWidgetSelected(const TabType& type);
	void SetMenuList(std::vector<STMenuItem>& vecItems);
Q_SIGNALS:
	void UrlTabSignal(const TabType& type);
	void forBack(const TabType & type) ;
public slots:
	void OnItemClicked(QListWidgetItem *);
	void OnTabClicked(const TabType & type);
private:
	TabItem *m_pTab1;
	TabItem *m_pTab2;
	TabItem *m_pTab3;
	TabItem *m_pTab4;
	TabItem *m_pTab5;
	TabItem *m_pTab6;
	TabItem *m_pTab7;
	TabItem *m_pTab8;
	TabItem *m_pTab9;
};

#endif // !url-tab-widget_h
