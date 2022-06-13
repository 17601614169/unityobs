#include "bottom-ctrls.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QTimer>
#include <QEvent>

#include "string-convert.h"
#include "custom-stylesheet.h"
#include "custom-config-ini.h"
#include "left-right-button.h"
#include "signal-proxy_model.h"
#include "common.h"
#include "url-label.h"
#include "customer-service-base-dialog.h"

BottomCtrls::BottomCtrls(QWidget *parent)
	: QWidget(parent), m_pMicSliderContext(nullptr)
{
	m_pButtonGroup = new QButtonGroup(this);
	//道具界面按钮
	m_p3dMaskBtn = new NamedBtn(QString(tr("mask3D")), this);
	m_p3dMaskBtn->setFixedSize(54, 44);
	//m_pMaskBtn = new NamedBtn(QString(tr("mask")), this);
	//m_pMaskBtn->setFixedSize(54, 44);
	m_pBkGroundBtn = new NamedBtn(QString(tr("bkground")), this);
	m_pBkGroundBtn->setFixedSize(54, 44);

	m_pMkfBtn = new NamedBtn(QString(tr("mkf")), this);
	m_pMkfBtn->setFixedSize(54, 44);

	m_pDeviceBtn = new NamedBtn(QString(tr("device")), this);
	m_pDeviceBtn->setFixedSize(54, 44);
	m_pQualityBtn = new NamedBtn(QString(tr("quality")), this);
	m_pQualityBtn->setFixedSize(54, 44);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(10, 0, 10, 0);
	layout->setSpacing(0);

	auto add_text = [&] {
		connect(this, &BottomCtrls::AddSourceClicked, SignalProxyModel::GetInstance(), &SignalProxyModel::AddSourceClicked, Qt::UniqueConnection);
		emit AddSourceClicked(common::GetSourceId(SOURCE_TEXT));
	};
	QString qss("QLabel {color:rgb(34, 34, 34); font-size:16px; text-align:left; background-color:transparent; font-family:Microsoft YaHei;}");
	LeftRightButton *addText = new LeftRightButton(":res/images/add_text.png", QString(tr("add_text")), this);
	addText->setFixedSize(120, 40);
	addText->setStyleSheet(USER_INFO_FINISH_BTN);
	addText->SetTextQss(qss);
	addText->SetMargins(QMargins(9,9,14,8));
	addText->SetSpace(0);
	connect(addText, &LeftRightButton::clicked, this, add_text);
	layout->addWidget(addText);
	layout->addSpacing(10);

	auto add_image = [&] {
		connect(this, &BottomCtrls::AddSourceClicked, SignalProxyModel::GetInstance(), &SignalProxyModel::AddSourceClicked, Qt::UniqueConnection);
		emit AddSourceClicked(common::GetSourceId(SOURCE_PICTURE));
	};
	LeftRightButton *addImage = new LeftRightButton(":res/images/add_image.png", QString(tr("add_image")), this);
	addImage->setFixedSize(120, 40);
	addImage->setStyleSheet(USER_INFO_FINISH_BTN);
	addImage->SetTextQss(qss);
	addImage->SetMargins(QMargins(9,9,14,8));
	addImage->SetSpace(0);
	connect(addImage, &LeftRightButton::clicked, this, add_image);
	layout->addWidget(addImage);
	layout->addSpacing(10);

	auto add = [&] {
		emit BeautyClicked();
	};

	LeftRightButton *addBeauty = new LeftRightButton(":res/images/add_filter.png", QString(tr("add_filter")), this);
	addBeauty->setFixedSize(120, 40);
	addBeauty->setStyleSheet(USER_INFO_FINISH_BTN);
	addBeauty->SetTextQss(qss);
	addBeauty->SetMargins(QMargins(9,9,14,8));
	addBeauty->SetSpace(0);
	connect(addBeauty, &LeftRightButton::clicked, this, add);
	layout->addWidget(addBeauty);

	layout->addStretch();

	layout->addWidget(m_p3dMaskBtn);
	layout->addSpacing(10);
	//layout->addWidget(m_pMaskBtn);
	//layout->addSpacing(10);
	layout->addWidget(m_pBkGroundBtn);
	layout->addSpacing(10);
	layout->addWidget(m_pMkfBtn);
	layout->addSpacing(10);
	layout->addWidget(m_pDeviceBtn);
	layout->addSpacing(10);
	layout->addWidget(m_pQualityBtn);
	setLayout(layout);
	struct STbuttons {
		NamedBtn *btn;
		BtnTypes type;
	} buttons[] = {
		{m_p3dMaskBtn, TYPE_3DMASK},
		//{m_pMaskBtn, TYPE_MASK},
		{m_pBkGroundBtn, TYPE_BKGROUND},
		{m_pMkfBtn, TYPE_MKF},
		{m_pDeviceBtn, TYPE_DEVICE},
		{m_pQualityBtn, TYPE_QUAILTY},
	};
	for (int i = 0; i < _countof(buttons); ++i) {
		//去除焦点获取时的虚影(键盘上下左右会产生)
		buttons[i].btn->setFocusPolicy(Qt::NoFocus);
		buttons[i].btn->setCheckable(false);
		buttons[i].btn->SetButtonType(buttons[i].type);
		m_pButtonGroup->addButton(buttons[i].btn, buttons[i].type);
		connect(buttons[i].btn, SIGNAL(buttonClicked(int)),
			m_pButtonGroup, SIGNAL(buttonClicked(int)));
	}
	connect(m_pButtonGroup, SIGNAL(buttonClicked(int)), this,
		SLOT(OnButtonGroupSlot(int)));

	m_pMicSliderContext = new CustomSlider(nullptr);
	m_bEnterMic = false;
	m_pPosTimer = new QTimer(this);
	m_pPosTimer->setInterval(500);
	auto TSlot = [&]() {
		ResetMicSliderPosition();
		HideMicSlider();
	};
	connect(m_pPosTimer, &QTimer::timeout, TSlot);
	m_pMkfBtn->installEventFilter(this);
	m_pPosTimer->start();
	m_pMicSliderContext->hide();
}

QSlider* BottomCtrls::GetMicSlider() {
	if (m_pMicSliderContext!=nullptr) {
		return m_pMicSliderContext->GetSlider();
	}
	return nullptr;
}

void BottomCtrls::HideMicSlider() {
	QPoint pos1 = m_pMicSliderContext->mapFromGlobal(QCursor::pos());
	if (!m_pMicSliderContext->rect().contains(pos1)) {
		if (!m_bEnterMic) {
			m_pMicSliderContext->hide();
		}
	}
}

void BottomCtrls::ResetMicSliderPosition() {
	if (m_pMicSliderContext != nullptr && m_pMkfBtn!=nullptr) {
		if (m_pMicSliderContext->isVisible() ) {
			QRect rect = m_pMkfBtn->geometry();
			QPoint point =
				this->mapToGlobal(QPoint(rect.x(), rect.y()));
			QPoint pos(point.x() +
					   (m_pMkfBtn->width() -
					    m_pMicSliderContext->width()) /
						       2,
				   point.y() - (m_pMicSliderContext->height() +
						10));
			m_pMicSliderContext->move(pos);
		}
	}
}

BottomCtrls::~BottomCtrls() {
	if (m_pMicSliderContext != nullptr) {
		m_pMicSliderContext->deleteLater();
		m_pMicSliderContext = nullptr;
	}
}

void BottomCtrls::OnButtonGroupSlot(int type) {
	emit buttonSignal(type);
}

bool BottomCtrls::eventFilter(QObject *obj, QEvent *event) {
	if (obj == m_pMkfBtn) {
		ResetMicSliderPosition();
		if (event->type() == QEvent::Enter) {
			m_bEnterMic = true;
			m_pMicSliderContext->raise();
			m_pMicSliderContext->show();
			
		} else if (event->type() == QEvent::Leave) {
			m_bEnterMic = false;
		}
	}
	return QWidget::eventFilter(obj,event);
}
