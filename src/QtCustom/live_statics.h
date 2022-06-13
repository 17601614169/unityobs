#ifndef LIVE_STATICS_H
#define LIVE_STATICS_H
#include <QWidget>
#include "move-dialog.h"
#include "qtcustom_global.h"
#include "asyn-request.h"
#include "callback-request.h"
#include <QtNetwork/QNetworkReply>
class QPushButton;
class QLabel;

class QTCUSTOM_EXPORT LiveStatics : public MoveDialog {
	Q_OBJECT
public:
	LiveStatics(const STReqData& req, QWidget *parent = 0);
	~LiveStatics();
	void SetWatchTime(const std::string & time);
	void SetWatchTotalNum(int);
	void SetTotalIncome(int);
	void SetAttentionNum(int);

	void RspCallback(const QByteArray &, const QNetworkReply::NetworkError &);
private:
	QPushButton *m_pClose;
	QLabel *m_pLiveTime;
	QLabel *m_pWatchTotalNum;
	QLabel *m_pTotalIncome;
	QLabel *m_pAttention;
	CallbackThread *m_CallBackThread;
};

#endif // !live_statics_h
