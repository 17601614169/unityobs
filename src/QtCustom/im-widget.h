#ifndef IMWIDGET_H
#define IMWIDGET_H
#include <QWidget>
#include <QLabel>
#include <mutex>
#include <QList>
#include <QLineEdit>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QtNetwork/QNetworkReply>
#include "qtcustom_global.h"
#include "base-define.h"
#include "base-widget.h"
class CallbackThread;

class QTimer;

class QTCUSTOM_EXPORT IMWidget : public BaseWidget {
	Q_OBJECT
public:
	IMWidget(QWidget *parent = nullptr);
	~IMWidget();
	void SetLoginToken(const std::string token);
	void RspFill(const QByteArray &bytes,
		     const QNetworkReply::NetworkError &error,
		     const QVariant &reqData);
	void InsertMessageBody(const STMessageBody &stBody);
	/**
	@brief 公告
	@param void
	@return 无
	*/
	void SetNotice(const QString &text);
	void SetBeginLive(const STBeginRsp& rsp);
	void SetRoomInfo(const STRoomInfo & info);
	void SetNoticeText(const QString &text);
	void SetConnected(bool con);
public slots:
	void OnTestGroup(int);
	void OnEditBtnClicked();
	void DoBlackList(const STMessageBody &);
	void OnTimerOut();

public:
	//ui list add
protected:
	bool eventFilter(QObject *obj, QEvent *event) final;
Q_SIGNALS:
	void testSignals(int);
	void ShowRedEnvelops();

private:
	QWidget *m_pTest;
	QButtonGroup *m_pGroup;
	QPushButton *m_pBegin;
	QPushButton *m_pToken;
	QPushButton *m_pConn;
	QPushButton *m_pSubscribe;
	QPushButton *m_pUnSubscribe;
	QPushButton *m_pSend;
	QPushButton *m_pFinish;
	//公告
	QWidget *m_pNotice;
	QLabel* m_pNoticeIcon;
	QLineEdit* m_pNoticeInput;
	QPushButton *m_pNoticeBtn;
	//系统提示
	QTextEdit *m_pSystemTip;
	//重连服务器
	QLabel *m_pReconServer;
	//聊天list
	QListWidget *m_pList;

	QVBoxLayout *m_playout;
	//登录token
	std::string m_strToken;
	//拉黑回调线程
	CallbackThread *m_pCallbackFill;

	QTimer *m_timerList;
	std::mutex m_mutexMsg;
	QList<STMessageBody> m_listMsg;
	QString m_strText;
	STBeginRsp m_stBeginRsp;
};

#endif // !IMWIDGET_H
