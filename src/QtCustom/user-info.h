#ifndef USERINFO_H
#define USERINFO_H

#include <QWidget>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

#include <memory>
#include "qtcustom_global.h"
#include "base-define.h"
#include "base-widget.h"
#include "custom-event.h"
#include "left-right-button.h"

class QLineEdit;
class QLabel;
class QUrlLabel;
class CallbackThread;
class QPushButton;

class QTCUSTOM_EXPORT UserInfoWidget : public BaseWidget {
	Q_OBJECT
public:
	enum INFO {
		INFO_LIVE_TIME = 0,
		INFO_PASUE_TIME,
		INFO_BIT_RATE
	};

	explicit UserInfoWidget(const std::string &token, QWidget *parent = nullptr);
	virtual ~UserInfoWidget();
	void SetTitle(const QString & title);
	void SetId(const QString & id);
	void ResetLiveState();

public slots:
	void OnStartLive();
	void OnPauseLive();
	void OnFinishLive();
	void StreamStateSlot(const StreamState& state);
Q_SIGNALS:
	void StreamStateChanged(const StreamState& state);
private:
	QLabel *m_pTitle;
	QLabel *m_pId;
	QLabel *m_pLiveState;
	QLabel *m_pLiveTime;
	QLabel *m_pPauseTime;
	QLabel *m_pBitRate;
	LeftRightButton *m_pStartLive;
	LeftRightButton *m_pPauseLive;
	LeftRightButton *m_pFinishLive;

	std::string m_strToken;
	QTimer *m_pPauseTimer;
	QTimer *m_pLiveTimer;
	int m_nPauseTime;
	int m_nLiveTime;
	bool m_bPause;//仅表示暂停显示状态
	std::unique_ptr<QTimer> m_netRateTimer;
};

#endif // !USERINFO_H
