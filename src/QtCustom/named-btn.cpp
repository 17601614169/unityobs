#include "named-btn.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "custom-stylesheet.h"

NamedBtn::NamedBtn(const QString &strName, QWidget *parent)
	: QPushButton(parent), m_pButton(nullptr), m_pName(nullptr)
{
	setStyleSheet(NAME_BTN_STYLE);

	setMouseTracking(true);
	QVBoxLayout *vlayout = new QVBoxLayout(this);
	QHBoxLayout *btnlayout = new QHBoxLayout;
	btnlayout->setContentsMargins(0, 0, 0, 0);
	btnlayout->setSpacing(0);
	btnlayout->addStretch();
	m_pButton = new QPushButton(this);
	btnlayout->addWidget(m_pButton);
	btnlayout->addStretch();
	m_pButton->setFixedSize(24, 24);
	m_pName = new QLabel(strName, this);
	m_pName->setFixedHeight(16);
	m_pName->setAlignment(Qt::AlignCenter);
	m_pName->setStyleSheet(NAME_BTN_TEXT);
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	vlayout->addStretch();
	vlayout->addLayout(btnlayout);
	vlayout->addSpacing(0);
	vlayout->addWidget(m_pName);
	vlayout->addStretch();
}

NamedBtn::~NamedBtn() {}
void NamedBtn::SetButtonType(BtnTypes type)
{
	btnType = type;
	auto buttonClick = [&]() { emit buttonClicked((int)btnType); };
	connect(m_pButton, &QPushButton::clicked, this, buttonClick,
		Qt::UniqueConnection);
	QString strStyle;
	switch (type) {
	case TYPE_MASK: {
		strStyle = NAME_BTN_MASKBTN;
	} break;
	case TYPE_BKGROUND: {
		strStyle = NAME_BTN_BKGROUNDBTN;
	} break;
	case TYPE_DEVICE: {
		strStyle = NAME_BTN_DEVICEBTN;
	} break;
	case TYPE_QUAILTY: {
		strStyle = NAME_BTN_QUAILTYBTN;
	} break;
	case TYPE_MKF: {
		strStyle = NAME_BTN_MKFBTN;
		m_pButton->setFixedSize(17,22);
	}break;
	case TYPE_3DMASK: {
		strStyle = NAME_BTN_MASK3DBTN;
	} break;
	default:
		break;
	}
	SetButtonStyle(strStyle);
}

void NamedBtn::SetButtonStyle(const QString &style)
{
	if (this->m_pButton != nullptr) {
		this->m_pButton->setStyleSheet(style);
	}
}

void NamedBtn::SetName(const QString& name) {
	m_pName->setText(name);
	m_pName->adjustSize();
}
