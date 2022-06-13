#include "left-right-button.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMargins>

#include "custom-stylesheet.h"

LeftRightButton::LeftRightButton(const QString &strImage,
			     const QString &text, QWidget *parent)
	: QPushButton(parent),
	  m_pLayout(nullptr),
	  m_pImage(nullptr),
	  m_pText(nullptr),
	  m_margins(11, 0, 0, 0),
	  m_space(3)
{
	m_pLayout = new QHBoxLayout();
	m_pLayout->setContentsMargins(m_margins);
	m_pLayout->setSpacing(m_space);
	m_pImage = new QLabel(this);
	QPixmap image = QPixmap(strImage);
	m_pImage->setFixedSize(image.size());
	m_pImage->setPixmap(image);
	m_pImage->setAttribute(Qt::WA_TranslucentBackground);
	m_pLayout->addWidget(m_pImage);

	m_pText = new QLabel(text, this);
	m_pText->setStyleSheet(LEFT_RIGHT_LABEL);
	m_pLayout->addWidget(m_pText);

	setLayout(m_pLayout);
}

LeftRightButton::~LeftRightButton() {}

void LeftRightButton::SetMargins(QMargins margins) {
	m_margins = margins;
	m_pLayout->setContentsMargins(m_margins);
}

void LeftRightButton::SetSpace(int space) {
	m_space = space;
	m_pLayout->setSpacing(m_space);
}

void LeftRightButton::SetTextQss(const QString &qss) {
	m_textQss = qss;
	m_pText->setStyleSheet(m_textQss);
}

void LeftRightButton::SetLabelText(const QString& text) {
	m_pText->setText(text);
}
