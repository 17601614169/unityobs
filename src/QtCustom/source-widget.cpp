#include "source-widget.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QStyleOption>
#include <QPainter>

#include <qdebug.h>

#include "signal-proxy_model.h"
#include "custom-stylesheet.h"

AddSourceButton::AddSourceButton(QWidget *parent)
	: QPushButton(parent),
	  m_pIcon(nullptr),
	  m_pText(nullptr),
	  m_type(SOURCE_CAMERA)
{
	setFixedHeight(38);
	setStyleSheet(SOURCE_BUTTON);

	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->setContentsMargins(QMargins(10, 0, 0, 0));
	mainLayout->setSpacing(10);

	m_pIcon = new QLabel(this);
	m_pIcon->setFixedSize(QSize(19, 18));
	mainLayout->addWidget(m_pIcon);

	m_pText = new QLabel();
	m_pText->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	m_pText->setStyleSheet(SOURCE_BUTTON_TEXT);
	mainLayout->addWidget(m_pText);

	setLayout(mainLayout);
}
AddSourceButton::~AddSourceButton() {}

void AddSourceButton::SetButtonType(SourceType type)
{
	m_type = type;
	QString strPath;
	QString strText;

	auto onclick = [&] {
		connect(this, &AddSourceButton::AddSourceClicked,
			SignalProxyModel::GetInstance(),
			&SignalProxyModel::AddSourceClicked);
		emit AddSourceClicked(common::GetSourceId(m_type));
	};
	connect(this, &AddSourceButton::clicked, this, onclick,
		Qt::UniqueConnection);

	switch (type) {
	case SOURCE_CAMERA:
		strPath = ":/res/images/shexiang_icon.png";
		strText = QString(tr("camera"));
		break;
	case SOURCE_PICTURE:
		strPath = ":/res/images/tupian_icon.png";
		strText = QString(tr("pictrue"));
		break;
	case SOURCE_TEXT:
		strPath = ":/res/images/wenzi_icon.png";
		strText = QString(tr("text"));
		break;
	case SOURCE_WINODW:
		strPath = ":/res/images/chuangkou_icon.png";
		strText = QString(tr("window"));
		break;
	case SOURCE_SCREEN:
		strPath = ":/res/images/pinmu_icon.png";
		strText = QString(tr("screen"));
		break;
	case SOURCE_MEDIA:
		strPath = ":/res/images/pinmu_icon.png";
		strText = QString(tr("media"));
		break;
	default:
		break;
	}

	if (m_pIcon)
		m_pIcon->setPixmap(QPixmap(strPath));
	if (m_pText)
		m_pText->setText(strText);
}

AddSourceWidget::AddSourceWidget(QWidget *parent)
	: QWidget(parent)
{
	setFixedSize(QSize(SOURCEWIDGET_WIDTH, SOURCEWIDGET_HEIGHT));
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowFlags(windowFlags() | Qt::Popup | Qt::FramelessWindowHint);
	setObjectName("SourceWidget");
	setStyleSheet(SOURCE_WIDGET_BG);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(QMargins(0, 8, 0, 13));
	mainLayout->setSpacing(0);

	auto click = [&] {
		close();
	};

	SourceType typeArray[6] = {SOURCE_CAMERA, SOURCE_PICTURE, SOURCE_TEXT,
			    SOURCE_WINODW, SOURCE_SCREEN, SOURCE_MEDIA};
	for (int i = 0; i < sizeof(typeArray) / sizeof(typeArray[0]); i++) {
		AddSourceButton *btn = new AddSourceButton(this);
		connect(btn, &AddSourceButton::clicked, click);
		btn->SetButtonType(typeArray[i]);
		mainLayout->addWidget(btn);

		QWidget *line = new QWidget(this);
		line->setFixedHeight(1);
		line->setStyleSheet(DIVIDING_LINE_LIST);
		mainLayout->addWidget(line);
	}

	setLayout(mainLayout);
}

AddSourceWidget::~AddSourceWidget() {
}

void AddSourceWidget::paintEvent(QPaintEvent *event) {
	QStyleOption option;
	option.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);
}
