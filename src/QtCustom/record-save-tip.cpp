#include "record-save-tip.h"
#include <QHBoxLayout>

#include "custom-stylesheet.h"

RecordSaveTip::RecordSaveTip(QWidget *parent) : TranslucenWidget(parent)
{
	SetPixmap(":/res/images/record_bg.png");
	resize(208,53);

	m_pTip = new QLabel(QString(tr("save-tip")),this);
	m_pTip->setFixedSize(63, 17);

	m_pTip->setStyleSheet(RECORD_SAVE_TIP);
	m_pViewDetail = new QPushButton(QString(tr("view-record-detail")), this);
	m_pViewDetail->setFixedSize(53, 24);
	m_pViewDetail->setStyleSheet(RECORD_VIEW_BTN);
	m_pSet = new QPushButton(QString(tr("record-path-set")), this);
	m_pSet->setFixedSize(51, 24);
	m_pSet->setStyleSheet(RECORD_SET_BTN);
	QHBoxLayout *hlayout = new QHBoxLayout(this);
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->setSpacing(0);
	hlayout->addStretch();
	hlayout->addWidget(m_pTip);
	hlayout->addSpacing(8);
	hlayout->addWidget(m_pViewDetail);
	hlayout->addSpacing(8);
	hlayout->addWidget(m_pSet);
	hlayout->addStretch();
	auto view_slot = [&]() {
		emit btnSignal(RecordSaveTip::TYPE_VIEW);
	};
	auto set_slot = [&]() {
		emit btnSignal(RecordSaveTip::TYPE_SET);
	};
	connect(m_pViewDetail, &QPushButton::clicked, view_slot);
	connect(m_pSet, &QPushButton::clicked, set_slot);

	m_pDiplayTimer = std::make_unique<QTimer>();
	m_pDiplayTimer->setInterval(5000);
	auto time_out = [&]() {
		this->hide();
		if (m_pDiplayTimer.get()) {
			m_pDiplayTimer->stop();
		}
	};
	connect(m_pDiplayTimer.get(), &QTimer::timeout, time_out);
}

RecordSaveTip::~RecordSaveTip()
{
}

void RecordSaveTip::ResetPos(const QPoint &pos,int w,int h) {
	int x = pos.x() + (w - 208) / 2;
	int y = pos.y() - 53 - 50;

	QPoint move_pos(x,y);
	move(move_pos);
}

void RecordSaveTip::ShowWithLimitTime() {
	if (m_pDiplayTimer.get()) {
		m_pDiplayTimer->start();
	}
	this->show();
	this->raise();
}
