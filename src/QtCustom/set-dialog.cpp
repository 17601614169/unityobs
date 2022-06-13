#include "set-dialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QDebug>

#include "left-right-button.h"
#include "custom-stylesheet.h"

SettingDialog::SettingDialog(QWidget *parent)
	: TranslucenWidget(parent),
	  m_pStartup(nullptr),
	  m_pMinGroup(nullptr),
	  m_pCloseGroup(nullptr)
{
	setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);

	resize(160, 299);
	SetPixmap(":/res/images/setting_bg.png");

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(0, 0, 0, 5);
	mainLayout->setSpacing(0);

	QGridLayout *grid = new QGridLayout();
	grid->setContentsMargins(13, 21, 10, 16);
	grid->setVerticalSpacing(14);
	grid->setHorizontalSpacing(10);
	mainLayout->addLayout(grid);

	QLabel *set = new QLabel(QString(tr("set")), this);
	set->setFixedHeight(17);
	set->setStyleSheet(SETTING_SET);
	grid->addWidget(set, 0, 0, 1, 2);

	auto chClick = [&] {
		bool bCheck = m_pStartup->isChecked();
		emit AutoStart(bCheck);
	};

	m_pStartup = new QCheckBox(QString(tr("auto_start")), this);
	m_pStartup->setFixedHeight(17);
	m_pStartup->setStyleSheet(SETTING_CHECK_BOX);
	connect(m_pStartup, &QCheckBox::clicked, this, chClick);
	grid->addWidget(m_pStartup, 1, 0, 1, 2);

	QLabel *minlabel = new QLabel(QString(tr("min_label")), this);
	minlabel->setFixedHeight(17);
	minlabel->setStyleSheet(SETTING_MIN);
	grid->addWidget(minlabel, 2, 0, 1, 2);

	QStringList list;
	list << QString(tr("taskbar")) << QString(tr("tray"))
	     << QString(tr("exit")) << QString(tr("tray"));

	m_pMinGroup = new QButtonGroup(this);
	m_pMinGroup->setExclusive(true);
	for (int i = 0; i < 2; i++) {
		QRadioButton *radio = new QRadioButton(list[i], this);
		radio->setFixedHeight(18);
		radio->setStyleSheet(SETTING_RADIO_BUTTON);

		m_pMinGroup->addButton(radio, TYPE_MIN_TASKBAR + i);
		grid->addWidget(radio, 3, i, 1, 1);
	}
	connect(m_pMinGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(SettingButtonClick(int)));

	QLabel *closelabel = new QLabel(QString(tr("close_label")), this);
	closelabel->setFixedHeight(17);
	closelabel->setStyleSheet(SETTING_CLOSE);
	grid->addWidget(closelabel, 4, 0, 1, 2);

	m_pCloseGroup = new QButtonGroup(this);
	m_pCloseGroup->setExclusive(true);
	for (int i = 0; i < 2; i++) {
		QRadioButton *radio = new QRadioButton(list[i + 2], this);
		radio->setFixedHeight(18);
		radio->setStyleSheet(SETTING_RADIO_BUTTON);

		m_pCloseGroup->addButton(radio, TYPE_CLOSE_EXIT + i);
		grid->addWidget(radio, 5, i, 1, 1);
	}
	connect(m_pCloseGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(SettingButtonClick(int)));

	struct ButtonInfo {
		QString path;
		QString name;
		SET_TYPE type;
	} buttons[] = {
		{QString(":/res/images/update.png"), QString(tr("update")), TYPE_UPDATE},
		       {QString(":/res/images/exit.png"), QString(tr("exit")), TYPE_CLOSE_WINDOW}
	};

	QButtonGroup *group = new QButtonGroup(this);
	for (auto info: buttons) {
		QWidget *line = new QWidget(this);
		line->setFixedHeight(1);
		line->setStyleSheet(DIVIDING_LINE_LIST);
		mainLayout->addWidget(line);

		LeftRightButton *button = new LeftRightButton(info.path, info.name, this);
		button->setFixedHeight(40);
		button->setStyleSheet(SETTING_BUTTON);
		group->addButton(button, info.type);
		mainLayout->addWidget(button);
		if (info.type == TYPE_UPDATE)
			button->setEnabled(false);
	}
	connect(group, SIGNAL(buttonClicked(int)), this, SIGNAL(SettingButtonClick(int)));

	QWidget *linetwo = new QWidget(this);
	linetwo->setFixedHeight(1);
	linetwo->setStyleSheet(DIVIDING_LINE_LIST);
	mainLayout->addWidget(linetwo);

	setLayout(mainLayout);
}

SettingDialog::~SettingDialog() {}

void SettingDialog::SetStatus(bool bAuto, bool bMinToTray, bool bCloseToTray)
{
	if (m_pStartup)
		m_pStartup->setChecked(bAuto);

	if (m_pMinGroup) {
		if (bMinToTray)
			qobject_cast<QRadioButton *>(m_pMinGroup->button(TYPE_MIN_TRAY))->setChecked(true);
		else
			qobject_cast<QRadioButton *>(m_pMinGroup->button(TYPE_MIN_TASKBAR))->setChecked(true);
	}

	if (m_pCloseGroup) {
		if (bCloseToTray)
			qobject_cast<QRadioButton *>(m_pCloseGroup->button(TYPE_CLOSE_TRAY))->setChecked(true);
		else
			qobject_cast<QRadioButton *>(m_pCloseGroup->button(TYPE_CLOSE_EXIT))->setChecked(true);
	}
}
