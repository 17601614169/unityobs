#include "live_statics.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include "custom-stylesheet.h"
#include <QTextCodec>
#include "characters-convert.h"
#include "json-parser.h"
LiveStatics::LiveStatics(const STReqData& req, QWidget *parent)
	: MoveDialog(parent), m_CallBackThread(nullptr)
{
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
	setMinimumSize(383,245);
	setMaximumSize(383, 245);
	SetColor(QString("#FCFBFD"),12);
	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	
	m_pClose = new QPushButton(this);
	m_pClose->setFixedSize(16,16);
	m_pClose->setStyleSheet(CLOSE_BTN);
	QHBoxLayout *hlayout1 = new QHBoxLayout;
	hlayout1->setContentsMargins(0, 0, 0, 0);
	hlayout1->setSpacing(0);
	hlayout1->addStretch();
	hlayout1->addWidget(m_pClose);
	hlayout1->addSpacing(12);
	mainLayout->addSpacing(16);
	mainLayout->addLayout(hlayout1);

	QLabel *label_end = new QLabel(tr("live ended"), this);
	label_end->setStyleSheet(LIVE_END_TIP);
	label_end->setFixedHeight(25);
	QHBoxLayout *hlayout2 = new QHBoxLayout;
	hlayout2->setContentsMargins(0, 0, 0, 0);
	hlayout2->setSpacing(0);
	hlayout2->addStretch();
	hlayout2->addWidget(label_end);
	hlayout2->addStretch();
	mainLayout->addSpacing(17);
	mainLayout->addLayout(hlayout2);

	m_pLiveTime = new QLabel(tr("live_time_txt")+QString("00:00:00"),this);
	m_pLiveTime->setFixedHeight(20);
	m_pLiveTime->setStyleSheet(LIVE_TIME);
	QHBoxLayout *hlayout3 = new QHBoxLayout;
	hlayout3->setContentsMargins(0, 0, 0, 0);
	hlayout3->setSpacing(0);
	hlayout3->addStretch();
	hlayout3->addWidget(m_pLiveTime);
	hlayout3->addStretch();
	mainLayout->addSpacing(10);
	mainLayout->addLayout(hlayout3);

	m_pWatchTotalNum = new QLabel(QString("0"), this);
	m_pWatchTotalNum->setStyleSheet(STATIC_NUM);
	m_pWatchTotalNum->setFixedSize(100, 24);
	m_pWatchTotalNum->setAlignment(Qt::AlignCenter);
	QLabel* pWatchTotalNum = new QLabel(tr("watch_total_num"), this);
	pWatchTotalNum->setStyleSheet(STATIC_TXT);
	pWatchTotalNum->setFixedSize(100, 17);
	pWatchTotalNum->setAlignment(Qt::AlignCenter);
	QVBoxLayout *v1 = new QVBoxLayout;
	v1->setContentsMargins(0, 0, 0, 0);
	v1->setSpacing(0);
	v1->addWidget(m_pWatchTotalNum);
	v1->addSpacing(5);
	v1->addWidget(pWatchTotalNum);

	QLabel *sep1 = new QLabel(this);
	sep1->setFixedSize(1,44);
	sep1->setStyleSheet("background: #0C0728;");

	m_pTotalIncome = new QLabel(QString("0"), this);
	m_pTotalIncome->setStyleSheet(STATIC_NUM);
	m_pTotalIncome->setFixedSize(100, 24);
	m_pTotalIncome->setAlignment(Qt::AlignCenter);
	QLabel *pTotalIncome = new QLabel(tr("income_total_num"), this);
	pTotalIncome->setStyleSheet(STATIC_TXT);
	pTotalIncome->setFixedSize(100, 17);
	pTotalIncome->setAlignment(Qt::AlignCenter);
	QVBoxLayout *v2 = new QVBoxLayout;
	v2->setContentsMargins(0, 0, 0, 0);
	v2->setSpacing(0);
	v2->addWidget(m_pTotalIncome);
	v2->addSpacing(5);
	v2->addWidget(pTotalIncome);

	QLabel *sep2 = new QLabel(this);
	sep2->setFixedSize(1, 44);
	sep2->setStyleSheet("background: #0C0728;");
	
	m_pAttention = new QLabel(QString("0"), this);
	m_pAttention->setStyleSheet(STATIC_NUM);
	m_pAttention->setFixedSize(100, 24);
	m_pAttention->setAlignment(Qt::AlignCenter);
	QLabel *pAttention = new QLabel(tr("attention_num"), this);
	pAttention->setFixedSize(100, 17);
	pAttention->setStyleSheet(STATIC_TXT);
	pAttention->setAlignment(Qt::AlignCenter);
	QVBoxLayout *v3 = new QVBoxLayout;
	v3->setContentsMargins(0, 0, 0, 0);
	v3->setSpacing(0);
	v3->addWidget(m_pAttention);
	v3->addSpacing(5);
	v3->addWidget(pAttention);

	QHBoxLayout *hlayout4 = new QHBoxLayout;
	hlayout4->setContentsMargins(0, 0, 0, 0);
	hlayout4->setSpacing(0);
	hlayout4->addStretch();
	hlayout4->addLayout(v1);
	hlayout4->addSpacing(3);
	hlayout4->addWidget(sep1);
	hlayout4->addSpacing(3);
	hlayout4->addLayout(v2);
	hlayout4->addSpacing(3);
	hlayout4->addWidget(sep2);
	hlayout4->addSpacing(3);
	hlayout4->addLayout(v3);
	hlayout4->addStretch();
	
	mainLayout->addSpacing(44);
	mainLayout->addLayout(hlayout4);
	mainLayout->addStretch();
	setLayout(mainLayout);
	connect(m_pClose,SIGNAL(clicked()),this,SLOT(close()));

	auto callback = std::bind(&LiveStatics::RspCallback, this,
				  std::placeholders::_1, std::placeholders::_2);
	m_CallBackThread = new CallbackThread(req, callback);
}

LiveStatics::~LiveStatics() {
	if (m_CallBackThread != nullptr) {
		m_CallBackThread->wait();
		m_CallBackThread->deleteLater();
		m_CallBackThread = nullptr;
	}
}

void LiveStatics::RspCallback(const QByteArray & bytes,
			      const QNetworkReply::NetworkError & error)
{
	qDebug() << "bytes:" << bytes;
	
	STFinishRsp info = ParseFinish(bytes);
	if (info.success) {
		SetWatchTime(info.liveTime);
		SetWatchTotalNum(info.visitorNum);
		SetTotalIncome(info.income);
		SetAttentionNum(info.newAttention);
	}
	if (m_CallBackThread != nullptr) {
		qDebug()<<"is running:"<<m_CallBackThread->isRunning();
	}
}

void LiveStatics::SetWatchTotalNum(int num) {
	m_pWatchTotalNum->setText(QString::number(num));
}

void LiveStatics::SetTotalIncome(int num) {
	m_pTotalIncome->setText(QString::number(num));
}

void LiveStatics::SetAttentionNum(int num) {
	m_pAttention->setText(QString::number(num));
}

void LiveStatics::SetWatchTime(const std::string &time) {
	QString strString = tr("live_time_txt") + QString::fromStdString(time);
	m_pLiveTime->setText(strString);
}
