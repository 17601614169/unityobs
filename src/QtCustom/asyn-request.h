#ifndef ASYN_REQUEST_H
#define ASYN_REQUEST_H
#include <QObject>
#include <QtNetwork/qnetworkrequest.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <QThread>
#include <memory>
#include <QMutex>
#include <QRunnable>
#include <functional>
#include <QPair>
#include "base-define.h"
#include "qtcustom_global.h"
#include "network/base-request.h"
enum EFunction {
	FUNC_NONE = 0,
	FUNC_LIVE_LIST = 1,
	FUNC_BEGIN_LIVE =2,
	FUNC_GET_CHAT_TOKEN = 3,
	FUNC_GET_FANS_LIST = 4,
	FUNC_GET_ACCOUNT = 5,
	FUNC_HAND_OUT_RED = 6,
	FUNC_FINISH_LIVE = 7,
	FUNC_GIFT_LIST = 8,
	FUNC_RED_DETAILS = 9,
	FUNC_NOTICE = 9,
	FUN_DO_BACKLIST = 10,
	FUN_ROOM_INFO = 11,
	FUNC_GET_USER_BASIC_INFO = 12,
	FUNC_GET_LIVE_TITLE = 13,
	FUNC_ANCHOR_PERMISSION =14,
	FUNC_GET_MASK_LIST = 15,
	FUNC_LIVE_PAUSE =16,
	FUNC_LIVE_REPAUSE =17,
	FUNC_ROOM_LIST =18,
	FUNC_GET_BACKGROUND_LIST = 19,
	FUNC_GET_INFO_CLASS = 20,
	FUNC_GET_PAGE_LIST = 21,
	FUNC_GET_PAGE_LIST_LIMIT = 22,
	FUNC_NOTIC_DELETE_BATCH = 23,
	FUNC_NOTIC_READ_BATCH = 24,
	FUNC_GET_UNREAD_COUNT = 25,
	FUNC_GET_IMTOKEN = 26,
	FUNC_GET_DOWNLOAD = 27,
	FUNC_HEART_BEAT = 28,
	FUNC_NOTIC_READ = 29,
	FUNC_WHITE_DETAIL = 30,
	FUNC_AUTH_CHECK = 31,
	FUNC_DROPOUT=32,
	FUNC_GET_CURRENT_PAGE_LIST=33,
	FUNC_SIGNIN_HEART_BEAT = 34,
};

typedef QList<QPair<QByteArray, QByteArray>> HeaderList;

struct STReqData
{
	EFunction func;
	EMothod method;
	char url[2048];
	char body[2048];
	HeaderList list;
	RequestTask task;
	STReqData()
	{
		func = FUNC_NONE;
		method = GET;
		memset(url, 0, 2048);
		memset(body, 0, 2048);
	}
};
struct STRspData
{
    int error;
    EFunction func;
    int len;
    QByteArray recvBytes;
    STRspData()
    {
	func = FUNC_NONE;
        len = 0;
        error=0;
    }
};
Q_DECLARE_METATYPE(STReqData);
Q_DECLARE_METATYPE(STRspData);


class QTCUSTOM_EXPORT AsynThread : public QThread {
    Q_OBJECT
public:
	AsynThread(QObject *parent = nullptr);
	~AsynThread();
	void InsertReq(const STReqData &stOperateData);
	void SetStop(bool stop);
public slots:
	void RequestFinishd(bool bSuccess, const QByteArray& bytesContent, const QString& strError);
Q_SIGNALS:
	void readyData(const STRspData &recv_data);
	void exitEventLoop();
protected:
	virtual void run() override;
private:
	QMutex m_mutex;
	QVector<STReqData> m_vecReq;
	bool m_bStop;
};

class QTCUSTOM_EXPORT AsynReqManager : public QObject {
    Q_OBJECT
public:
	SINGLETON_DEFINE(AsynReqManager);
	~AsynReqManager();
	void InsertReq(const STReqData &stReqData);
public slots:
	void OnReadyData(const STRspData &recv_data);
Q_SIGNALS:
	void readyData(const STRspData &recv_data);

private:
	AsynReqManager(QObject *parent = nullptr);
	AsynThread* m_ptrAsynThread;
};
#endif
