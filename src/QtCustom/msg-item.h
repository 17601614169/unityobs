#ifndef MSG_ITEM_H
#define MSG_ITEM_H
#include<QWidget>
#include "base-define.h"
class QLabel;
class QPushButton;
class QTextBrowser;
struct STMessageBody;
class MaskLabel;
class QUrlLabel;
class MsgItem : public QWidget {
	Q_OBJECT
public:
	MsgItem(const std::string token, const STMessageBody &stBody, QWidget *parent = 0,
		ContentType type = TYPE_CHAT);
	~MsgItem();

public:
	void OnAnchorClicked(const QUrl &);
public slots:
	void OnGiftClicked();
signals:
	void doBlackList(const STMessageBody&);
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private:
	QTextBrowser *m_pNormalMsg;
	QString m_strPlainMsg;
	//gift,biggift
	QLabel *m_pUserName;
	QLabel *m_pGiftText;
	QUrlLabel *m_pGiftIcon;
	QLabel *m_pGiftNum;
	STMessageBody m_stBody;
	std::string m_strToken;
};

#endif // ! msg_item_h
