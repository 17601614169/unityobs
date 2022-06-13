#include "user-info.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QApplication>
#include <QTimer>
#include "url-label.h"
#include "callback-request.h"
#include "custom-config-ini.h"
#include "json-parser.h"
#include "custom-stylesheet.h"
#include "util/base.h"
#include "customer-service-base-dialog.h"

using namespace CustomConfig;

#ifdef _WINDOWS
#include <Windows.h>

#define MAX_INTERFACE_NAME_LEN 256
#define MAXLEN_PHYSADDR 8
#define MAXLEN_IFDESCR 256
#define ANY_SIZE 1
typedef struct _MIB_IFROW
{
    WCHAR wszName[MAX_INTERFACE_NAME_LEN];
    DWORD dwIndex;
    DWORD dwType;
    DWORD dwMtu;
    DWORD dwSpeed;
    DWORD dwPhysAddrLen;
    BYTE bPhysAddr[MAXLEN_PHYSADDR];
    DWORD dwAdminStatus;
    DWORD dwOperStatus;
    DWORD dwLastChange;
    DWORD dwInOctets;
    DWORD dwInUcastPkts;
    DWORD dwInNUcastPkts;
    DWORD dwInDiscards;
    DWORD dwInErrors;
    DWORD dwInUnknownProtos;
    DWORD dwOutOctets;
    DWORD dwOutUcastPkts;
    DWORD dwOutNUcastPkts;
    DWORD dwOutDiscards;
    DWORD dwOutErrors;
    DWORD dwOutQLen;
    DWORD dwDescrLen;
    BYTE bDescr[MAXLEN_IFDESCR];
} MIB_IFROW,*PMIB_IFROW;

typedef struct _MIB_IFTABLE
{
    DWORD dwNumEntries;
    MIB_IFROW table[ANY_SIZE];
} MIB_IFTABLE, *PMIB_IFTABLE;

typedef DWORD (__stdcall *GIT)(PMIB_IFTABLE, PULONG, BOOL);
GIT lpGetIfTable;

static HINSTANCE hIphlpDll;
static DWORD32 numb = 0;

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

void _CheckNetGoto(HINSTANCE hinstance, int &rate)
{
    BOOL bResult=false;
    DWORD dwSize = 0;

     lpGetIfTable = (GIT)GetProcAddress(hinstance, "GetIfTable");
    if (lpGetIfTable == NULL) 
        return;

    //MIB_IFTABLE *pMIT = new MIB_IFTABLE[sizeof(MIB_IFTABLE)];
    //if (lpGetIfTable(pMIT, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
    //    delete []pMIT;
    //    pMIT = new MIB_IFTABLE[dwSize];
    //}

    MIB_IFTABLE *pMIT;

    // Allocate memory for our pointers.
    pMIT = (MIB_IFTABLE *) MALLOC(sizeof (MIB_IFTABLE));
    if (pMIT == NULL) {
        printf("Error allocating memory needed to call GetIfTable\n");
        return;
    }
    // Make an initial call to GetIfTable to get the
    // necessary size into dwSize
    dwSize = sizeof (MIB_IFTABLE);
    if (lpGetIfTable(pMIT, &dwSize, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        FREE(pMIT);
        pMIT = (MIB_IFTABLE *) MALLOC(dwSize);
        if (pMIT == NULL) {
            printf("Error allocating memory needed to call GetIfTable\n");
            return;
        }
    }

    //dwNumEntries alwayse>=1, and the last entry is loopback interface.
    if (lpGetIfTable(pMIT, &dwSize, 0) == NO_ERROR)
    {
        if (pMIT->dwNumEntries <= 1) { 
            bResult = false; 
        } else {
		if (numb == 0) {
			numb = pMIT->table[0].dwOutOctets;
			rate = 0;
		} else {
			int value = pMIT->table[0].dwOutOctets > numb ? pMIT->table[0].dwOutOctets - numb : (0xFFFFFFFF-numb) + pMIT->table[0].dwOutOctets;
			rate = value * 8 / (2 * 1000);
			numb = pMIT->table[0].dwOutOctets;
		}
        }
    }

    if (pMIT != NULL) {
        FREE(pMIT);
        pMIT = NULL;
    }
}

#endif // _WINDOWS

UserInfoWidget::UserInfoWidget(const std::string &token, QWidget *parent)
	: BaseWidget(parent),
	  m_pTitle(nullptr),
	  m_pId(nullptr),
	  m_pLiveState(nullptr),
	  m_pLiveTime(nullptr),
	  m_pPauseTime(nullptr),
	  m_pBitRate(nullptr),
	  m_pStartLive(nullptr),
	  m_pPauseLive(nullptr),
	  m_pFinishLive(nullptr),
	  m_strToken(token),
	  m_pPauseTimer(nullptr),
	  m_pLiveTimer(nullptr),
	  m_nPauseTime(0),
	  m_nLiveTime(0),
	  m_bPause(true)
{
	setObjectName("user_info_bg");
	//setStyleSheet(USER_INFO_BG);

	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->setContentsMargins(QMargins(10, 10, 10, 10));
	mainLayout->setSpacing(10);

	QVBoxLayout *verLayout = new QVBoxLayout();
	verLayout->setContentsMargins(0, 0, 0, 0);
	verLayout->setSpacing(7);
	mainLayout->addLayout(verLayout);

	QHBoxLayout *horLayout1 = new QHBoxLayout();
	horLayout1->setContentsMargins(0,0,0,0);
	horLayout1->setSpacing(5);
	verLayout->addLayout(horLayout1);

	m_pTitle = new QLabel(this);
	m_pTitle->setStyleSheet(USER_INFO_TITLE);
	horLayout1->addWidget(m_pTitle);

	m_pId = new QLabel(this);
	m_pId->setStyleSheet(USER_INFO_ID);
	horLayout1->addWidget(m_pId);
	horLayout1->addStretch();

	QHBoxLayout *horLayout2 = new QHBoxLayout();
	horLayout2->setContentsMargins(0,0,0,0);
	horLayout2->setSpacing(0);
	verLayout->addLayout(horLayout2);

	auto addlabel = [&](QString str) {
		QLabel *label = new QLabel(str, this);
		label->setStyleSheet(USER_INFO_GENERAL);
		horLayout2->addWidget(label);
	};

	addlabel(QString(tr("live_state")));

	m_pLiveState = new QLabel(QString(tr("live_state_not_start")), this);
	m_pLiveState->setStyleSheet(USER_INFO_LIVE_STATE);
	horLayout2->addWidget(m_pLiveState);
	horLayout2->addSpacing(8);

	addlabel(QString(tr("live_time")));

	m_pLiveTime = new QLabel("00:00:00", this);
	m_pLiveTime->setStyleSheet(USER_INFO_DEPTH);
	horLayout2->addWidget(m_pLiveTime);
	horLayout2->addSpacing(8);

	addlabel(QString(tr("pasue_time")));

	m_pPauseTime = new QLabel("00:00:00", this);
	m_pPauseTime->setStyleSheet(USER_INFO_DEPTH);
	horLayout2->addWidget(m_pPauseTime);
	horLayout2->addSpacing(8);

	addlabel(QString(tr("bit_rate")));

	m_pBitRate = new QLabel(this);
	m_pBitRate->setStyleSheet(USER_INFO_DEPTH);
	horLayout2->addWidget(m_pBitRate);
	horLayout2->addSpacing(8);
	horLayout2->addStretch();

	mainLayout->addStretch();

	m_pFinishLive = new LeftRightButton(":res/images/finish_live.png", QString(tr("finish_live")), this);
	m_pFinishLive->setStyleSheet(USER_INFO_FINISH_BTN);
	m_pFinishLive->SetSpace(10);
	m_pFinishLive->setFixedSize(110,36);
	m_pFinishLive->setVisible(false);
	connect(m_pFinishLive, &LeftRightButton::clicked, this, &UserInfoWidget::OnFinishLive);
	mainLayout->addWidget(m_pFinishLive);

	m_pPauseLive = new LeftRightButton(":res/images/pause_live.png", QString(tr("pause_live")), this);
	m_pPauseLive->setStyleSheet(USER_INFO_START_BTN);
	m_pPauseLive->SetSpace(10);
	m_pPauseLive->setFixedSize(110,36);
	m_pPauseLive->setVisible(false);
	connect(m_pPauseLive, &LeftRightButton::clicked, this, &UserInfoWidget::OnPauseLive);
	mainLayout->addWidget(m_pPauseLive);
	
	m_pStartLive = new LeftRightButton(":res/images/start_live.png", QString(tr("start_live")), this);
	m_pStartLive->setStyleSheet(USER_INFO_START_BTN);
	m_pStartLive->SetSpace(10);
	m_pStartLive->setFixedSize(110,36);
	connect(m_pStartLive, &LeftRightButton::clicked, this, &UserInfoWidget::OnStartLive);
	mainLayout->addWidget(m_pStartLive);

	setLayout(mainLayout);

	auto pause_time_out = [&]{
		m_nPauseTime++;
		int seconds = m_nPauseTime % 60;
		int minutes = m_nPauseTime / 60;
		int realMinutes = minutes % 60;
		int hours = minutes / 60;
		QString text = QString::asprintf("%02d:%02d:%02d", hours, realMinutes, seconds);
		m_pPauseTime->setText(text);
	};
	m_pPauseTimer = new QTimer(this);
	m_pPauseTimer->setInterval(1000);
	connect(m_pPauseTimer, &QTimer::timeout, this, pause_time_out);

	auto live_time_out = [&] {
		m_nLiveTime++;
		int seconds = m_nLiveTime % 60;
		int minutes = m_nLiveTime / 60;
		int realMinutes = minutes % 60;
		int hours = minutes / 60;
		QString text = QString::asprintf("%02d:%02d:%02d", hours, realMinutes, seconds);
		m_pLiveTime->setText(text);
	};
	m_pLiveTimer = new QTimer(this);
	m_pLiveTimer->setInterval(1000);
	connect(m_pLiveTimer, &QTimer::timeout, this, live_time_out);

	auto net_time_out = [&] {
#ifdef _WINDOWS
		if (hIphlpDll == NULL) 
			return;
		int rate;
		_CheckNetGoto(hIphlpDll, rate);
		m_pBitRate->setText(QString::number(rate) + QString("KB/S"));
#endif // _WINDOWS
	};

	m_netRateTimer = std::make_unique<QTimer>();
	m_netRateTimer->setInterval(2000);
	connect(m_netRateTimer.get(), &QTimer::timeout, this, net_time_out);
	m_netRateTimer->start();

#ifdef _WINDOWS
	hIphlpDll = LoadLibraryW(L"iphlpapi.dll");
#endif // _WINDOWS
}

UserInfoWidget::~UserInfoWidget() {

#ifdef _WINDOWS
	FreeLibrary(hIphlpDll);
#endif // _WINDOWS

	if (m_pPauseTimer && m_pPauseTimer->isActive()) {
		m_pPauseTimer->stop();
		delete m_pPauseTimer;
		m_pPauseTimer = nullptr;
	}

	if (m_pLiveTimer && m_pLiveTimer->isActive()) {
		m_pLiveTimer->stop();
		delete m_pLiveTimer;
		m_pLiveTimer = nullptr;
	} 
}

void UserInfoWidget::OnPauseLive() {
	if (m_bPause) {
		emit StreamStateChanged(STREAM_PAUSE);
	} else {
		emit StreamStateChanged(STREAM_REPAUSE);
	}
}

void UserInfoWidget::OnFinishLive() {
	emit StreamStateChanged(STREAM_STOP);
}

void UserInfoWidget::OnStartLive() {
	emit StreamStateChanged(STREAM_START);
}

void UserInfoWidget::StreamStateSlot(const StreamState& state) {
	if (state == STREAM_START) {
		if (m_pPauseTimer && m_pPauseTimer->isActive())
		m_pPauseTimer->stop();

		if (m_pLiveTimer && !m_pLiveTimer->isActive())
			m_pLiveTimer->start();

		m_pPauseTime->setText("00:00:00");
		m_pFinishLive->setVisible(true);
		m_pPauseLive->setVisible(true);
		m_pPauseLive->SetLabelText(QString(tr("pause_live")));
		m_pStartLive->setVisible(false);
		m_pLiveState->setText(QString(tr("live_state_ing")));
	} else if (state == STREAM_STOP){
		if (m_pPauseTimer && m_pPauseTimer->isActive())
		m_pPauseTimer->stop();

		if (m_pLiveTimer && m_pLiveTimer->isActive())
			m_pLiveTimer->stop();

		m_nPauseTime = 0;
		m_pPauseTime->setText("00:00:00");
		m_nLiveTime = 0;
		m_pLiveTime->setText("00:00:00");
		m_pFinishLive->setVisible(false);
		m_pPauseLive->setVisible(false);
		m_pStartLive->setVisible(true);
		m_pLiveState->setText(QString(tr("live_state_finish")));
	} else if (state == STREAM_PAUSE)
	{
		m_nPauseTime = 0;
		if (m_pPauseTimer && !m_pPauseTimer->isActive())
			m_pPauseTimer->start();

		m_pFinishLive->setVisible(true);
		m_pPauseLive->setVisible(true);
		m_pPauseLive->SetLabelText(QString(tr("repause_live")));
		m_pStartLive->setVisible(false);
		m_bPause = false;
		m_pLiveState->setText(QString(tr("live_state_pause")));
	}
	else if (state == STREAM_REPAUSE)
	{
		if (m_pPauseTimer && m_pPauseTimer->isActive())
		m_pPauseTimer->stop();

		if (m_pLiveTimer && !m_pLiveTimer->isActive())
			m_pLiveTimer->start();

		m_pPauseTime->setText("00:00:00");
		m_pFinishLive->setVisible(true);
		m_pPauseLive->setVisible(true);
		m_pPauseLive->SetLabelText(QString(tr("pause_live")));
		m_pStartLive->setVisible(false);
		m_bPause = true;
		m_pLiveState->setText(QString(tr("live_state_ing")));
	}
}

void UserInfoWidget::SetTitle(const QString& title) {
	if (m_pTitle != nullptr) {
		m_pTitle->setText(title);
	}
}

void UserInfoWidget::SetId(const QString& id) {
	if (m_pId != nullptr) {
		m_pId->setText(id);
	}
}

void UserInfoWidget::ResetLiveState() {
	if (m_pLiveState)
		m_pLiveState->setText(QString(tr("live_state_not_start")));
}
