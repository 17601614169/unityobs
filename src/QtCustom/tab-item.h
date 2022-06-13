#ifndef tab_item_h
#define tab_item_h
#include "qtcustom_global.h"
#include <QWidget>
class QLabel;
class QPushButton;
enum TabType {
	TYPE_TAB_NONE = -1,
	/**
	* 直播间
	*/
	TYPE_TAB_LIVE = 0,
	/**
	* 交易记录
	*/
	TYPE_TAB_EXCHANGE = 1,
	/**
	* 红包额度记录
	*/
	TYPE_TAB_RED_PACKET = 2,
	/**
	* 直播数据统计
	*/
	TYPE_TAB_DATA_STATIC = 3,
	/**
	* 直播明细
	*/
	TYPE_TAB_LIVE_DETAIL = 4,
	/**
	* 知识库
	*/
	TYPE_TAB_KNOWLEDGE = 5,
	/**
	* 快捷回复
	*/
	TYPE_TAB_QUICK_REPLY = 6,
	/**
	* 房间列表
	*/
	TYPE_TAB_ROOM_LIST= 7,
	TYPE_EVALUTE = 8,
	TYPE_IM=9,
};

class QTCUSTOM_EXPORT TabItem : public QWidget {
	Q_OBJECT
public:
	TabItem(TabType type,QWidget* parent=0);
	~TabItem();
	void SetChecked(const TabType & type);
	TabType GetType() ;

protected:
	virtual void mousePressEvent(QMouseEvent* event) override;
Q_SIGNALS:
	void forBack(const TabType & type) ;
	void tabClick(const TabType &type) ;
private:
	QLabel* m_pBackground;
	QLabel* m_pIcon;
	QLabel* m_pText;
	QPushButton* m_pForBack;
	TabType m_tabType;
};

#endif // !tab_item_h
