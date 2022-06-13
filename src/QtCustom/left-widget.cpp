#include "left-widget.h"

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QListWidget>
#include <QCheckBox>
#include <QPainter>
#include <QScrollBar>
#include <QDesktopWidget>
#include <QApplication>

#include <qdebug.h>

#include "signal-proxy_model.h"
#include "source-widget.h"
#include "custom-stylesheet.h"
#include "string-convert.h"
#include "source-base-dialog.h"

bool EnumItem(obs_scene_t *, obs_sceneitem_t *item, void *ptr)
{
	QVector<OBSSceneItem> &items = *reinterpret_cast<QVector<OBSSceneItem> *>(ptr);

	if (obs_sceneitem_is_group(item)) {
		obs_data_t *data = obs_sceneitem_get_private_settings(item);

		bool collapse = obs_data_get_bool(data, "collapsed");
		if (!collapse) {
			obs_scene_t *scene =
				obs_sceneitem_group_get_scene(item);

			obs_scene_enum_items(scene, EnumItem, &items);
		}

		obs_data_release(data);
	}

	items.insert(0, item);
	return true;
}

LeftWidget::LeftWidget(const GetSceneCallback &getScene, QWidget *parent)
	: BaseWidget(parent),
	  m_pSourceList(nullptr),
	  m_getScene(getScene)
{
	setObjectName("left_widget_bg");
	setStyleSheet(LEFT_WIDGET_BG);

	connect(this, &LeftWidget::MoveSourceItem, SignalProxyModel::GetInstance(), &SignalProxyModel::MoveSourceItem);
	connect(this, &LeftWidget::SelectSourceItem, SignalProxyModel::GetInstance(), &SignalProxyModel::SelectSourceItem);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(QMargins(0, 8, 0, 6));
	mainLayout->setSpacing(0);

	QLabel *label = new QLabel(QString(tr("scene")), this);
	label->setFixedHeight(29);
	label->setStyleSheet(LEFT_WIDGET_SCENE);
	mainLayout->addWidget(label);

	auto itemclick = [&](QListWidgetItem *item) {
		if (item) {
			int count = m_pSourceList->count();
			for (int i = 0; i < count; i++) {
				SourceItem *source = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(i)));
				source->SetSelect(false);
			}
			SourceItem *sourceItem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(item));
			sourceItem->SetSelect(true);
			OBSSceneItem scene = sourceItem->GetSceneItem();
			if (scene != previousItem) {
				if (previousItem.Get()) {
					emit SelectSourceItem((void *)(&scene), (void *)(&previousItem));
				} else {
					emit SelectSourceItem((void *)(&scene), nullptr);
				}
				previousItem = scene;
			}
		}
	};

	m_pSourceList = new QListWidget(this);
	m_pSourceList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pSourceList->setFocusPolicy(Qt::NoFocus);
	m_pSourceList->setStyleSheet(LEFT_WIDGET_LIST);
	m_pSourceList->verticalScrollBar()->setStyleSheet(LEFT_WIDGET_SCROLL);
	connect(m_pSourceList, &QListWidget::itemClicked, this, itemclick);
	mainLayout->addWidget(m_pSourceList);

	QWidget *line = new QWidget(this);
	line->setFixedHeight(1);
	line->setStyleSheet(DIVIDING_LINE_BLACK);
	mainLayout->addWidget(line);

	QWidget *bottomWidget = new QWidget(this);
	bottomWidget->setFixedSize(QSize(175, 69));
	bottomWidget->setAttribute(Qt::WA_TranslucentBackground);

	QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
	bottomLayout->setContentsMargins(QMargins(10, 0, 0, 0));
	bottomLayout->setSpacing(20);

	QPushButton *addSource = new QPushButton(this);
	addSource->setFixedSize(QSize(18, 18));
	addSource->setStyleSheet(LEFT_WIDGET_ADD_SOURCE_BTN);
	connect(addSource, &QPushButton::clicked, this, &LeftWidget::OnClickAdd);
	bottomLayout->addWidget(addSource);

	QPushButton *moveDown = new QPushButton(this);
	moveDown->setFixedSize(QSize(18, 18));
	moveDown->setStyleSheet(LEFT_WIDGET_MOVE_DOWN_BTN);
	bottomLayout->addWidget(moveDown);

	QPushButton *moveUp = new QPushButton(this);
	moveUp->setFixedSize(QSize(18, 18));
	moveUp->setStyleSheet(LEFT_WIDGET_MOVE_UP_BTN);
	bottomLayout->addWidget(moveUp);
	bottomLayout->addStretch();

	auto moveclick = [&, moveDown, moveUp]() {
		int index = m_pSourceList->currentRow();
		if (index != -1) {
			OBSSceneItem scene = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(index)))->GetSceneItem();
			QPushButton *button = qobject_cast<QPushButton *>(sender());
			
			std::string strName = StringConvert::GetInstance()->GetString(STR_SECRET).toStdString();
			obs_sceneitem_t *item = nullptr;
			if (m_getScene != nullptr) {
				OBSScene scene = m_getScene();
				item = obs_scene_find_source(scene.Get(), strName.c_str());
			}

			if (button == moveDown) {
				if (index < m_pSourceList->count() - 1) {
					if (item != scene.Get()) {
						emit MoveSourceItem((void *)(&scene), 1);
					}
				}
			} else if (button == moveUp) {
				if (index - 1 > -1) {
					OBSSceneItem sceneprev = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(index - 1)))->GetSceneItem();
					if (item != sceneprev.Get()) {
						emit MoveSourceItem((void *)(&scene), -1);
					}
				}
			}
		}
	};

	connect(moveDown, &QPushButton::clicked, this, moveclick);
	connect(moveUp, &QPushButton::clicked, this, moveclick);

	bottomWidget->setLayout(bottomLayout);
	mainLayout->addWidget(bottomWidget);

	setLayout(mainLayout);
}

LeftWidget::~LeftWidget() {
}

void LeftWidget::ReorderItems(QVector<OBSSceneItem> &items)
{
	int count = m_pSourceList->count();
	int oldIndex = -1, newIndex = -1;
	for (int i = 0; i < count; i++) {
		QListWidgetItem *item = m_pSourceList->item(i);
		SourceItem *sourceitem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(item));
		OBSSceneItem sceneitem = sourceitem->GetSceneItem();
		if (sceneitem != items[i]) {
			oldIndex = i;
			break;
		}
	}

	for (int i = oldIndex + 1; i < count; i++) {
		QListWidgetItem *item = m_pSourceList->item(i);
		SourceItem *sourceitem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(item));
		OBSSceneItem sceneitem = sourceitem->GetSceneItem();
		if (sceneitem != items[i]) {
			newIndex = i;
			break;
		}
	}
	
	if (oldIndex != -1 && newIndex != -1) {
		ChangeItemPos(oldIndex, newIndex - oldIndex);
	}
}

void LeftWidget::SelectItem(OBSSceneItem sceneitem, bool select) {
	int count = m_pSourceList->count();
	int row = m_pSourceList->currentRow();
	for (int i = 0; i < count; i++) {
		QListWidgetItem *item = m_pSourceList->item(i);
		SourceItem *sourceitem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(item));
		if (sceneitem == sourceitem->GetSceneItem()) {
			if (select) {
				m_pSourceList->setCurrentRow(i);
			}
			else if (!select && obs_sceneitem_visible(sceneitem)) {
				if (row > -1 && row != i)
					m_pSourceList->setCurrentRow(row);
				else
					m_pSourceList->setCurrentRow(-1);
			}

			sourceitem->SetSelect(select);
			break;
		}
	}
}

void LeftWidget::DeleteSourceItem(OBSSceneItem sceneitem) {
	int count = m_pSourceList->count();
	for (int i = 0; i < count; i++) {
		QListWidgetItem *item = m_pSourceList->item(i);
		SourceItem *sourceitem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(item));
		if (sceneitem == sourceitem->GetSceneItem()) {
			m_pSourceList->takeItem(i);
			m_pSourceList->removeItemWidget(item);
			delete item;
			break;
		}
	}
}

void LeftWidget::OnClickAdd()
{
	QPushButton *btn = (QPushButton *)sender();
	QPoint p(0, 0);
	QPoint pt = btn->mapToGlobal(p);

	AddSourceWidget *wd = new AddSourceWidget(this);
	wd->setGeometry(pt.x() - 7, pt.y() - SOURCEWIDGET_HEIGHT - 2,
			SOURCEWIDGET_WIDTH, SOURCEWIDGET_HEIGHT);
	wd->show();
}

bool LeftWidget::ChangeItemPos(int index, int step) {
	if (!m_pSourceList)
		return false;

	int count = m_pSourceList->count();
	int nextIndex = index + step;
	if (index < 0 || index >= count || nextIndex < 0 || nextIndex >= count)
		return false;

	int selIndex = -1;
	OBSSceneItem firstItem, secondItem;
	if (index < nextIndex) {
		firstItem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(index)))->GetSceneItem();
		secondItem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(nextIndex)))->GetSceneItem();
		if (obs_sceneitem_selected(firstItem))
			selIndex = nextIndex;
		else if (obs_sceneitem_selected(secondItem))
			selIndex = index;
	} else {
		return false;
	}
	/*else {
		first = nextIndex;
		second = index;
		firstItem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(nextIndex)))->GetSceneItem();
		secondItem = qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(index)))->GetSceneItem();
	}*/

	auto delItem = [&](int row) {
		QListWidgetItem *item = m_pSourceList->takeItem(row);
		m_pSourceList->removeItemWidget(item);
		delete item;
	};

	auto insertItem = [&](int row, OBSSceneItem sceneItem) {
		SourceItem *sourceItem = new SourceItem(sceneItem);
		sourceItem->Init();
		QListWidgetItem *item = new QListWidgetItem();
		//设置 Item 的大小
		item->setSizeHint(QSize(0, sourceItem->height()));
		m_pSourceList->insertItem(row, item);
		m_pSourceList->setItemWidget(item, sourceItem);
	};

	delItem(nextIndex);
	insertItem(nextIndex, firstItem);

	delItem(index);
	insertItem(index, secondItem);

	m_pSourceList->setCurrentRow(selIndex);
	qobject_cast<SourceItem *>(m_pSourceList->itemWidget(m_pSourceList->item(selIndex)))->SetSelect(true);

	return true;
}

void LeftWidget::AddSourceItem(OBSSceneItem sceneitem, bool bFirst)
{
	SourceItem *sourceItem = new SourceItem(sceneitem, m_pSourceList);
	sourceItem->Init();
	QListWidgetItem *item = new QListWidgetItem();
	//设置 Item 的大小
	item->setSizeHint(QSize(0, sourceItem->height()));

	if (bFirst)
		m_pSourceList->insertItem(0, item);
	else
		m_pSourceList->addItem(item);

	m_pSourceList->setItemWidget(item, sourceItem);
}

SourceItem::SourceItem(OBSSceneItem sceneitem, QWidget *parent)
	: QWidget(parent),
	  m_obsSceneItem(sceneitem),
	  m_bSelect(false),
	  m_bHover(false),
	  m_pEditButton(nullptr),
	  m_pDeleteButton(nullptr),
	  m_pEnableCheckBox(nullptr),
	  m_pText(nullptr),
	  m_pScreenShotDialog(nullptr)
{
	setFixedHeight(46);
}

SourceItem::~SourceItem() {
	if (m_pScreenShotDialog) {
		m_pScreenShotDialog->close();
		delete m_pScreenShotDialog;
		m_pScreenShotDialog = nullptr;
	}
}

void SourceItem::Init() {
	obs_source_t *source = obs_sceneitem_get_source(m_obsSceneItem);

	QVBoxLayout *pVerLayout = new QVBoxLayout();
	pVerLayout->setContentsMargins(QMargins(0, 0, 0, 0));
	pVerLayout->setSpacing(0);

	QHBoxLayout *pHorLayout = new QHBoxLayout();
	pHorLayout->setContentsMargins(QMargins(5, 0, 10, 0));
	pHorLayout->setSpacing(10);

	QLabel *pIcon = new QLabel(this);
	const char *id = obs_source_get_id(source);
	QPixmap pix(common::GetSourceIcon(id));
	pIcon->setPixmap(pix);
	pIcon->setEnabled(obs_sceneitem_visible(m_obsSceneItem));
	pHorLayout->addWidget(pIcon);

	m_pText = new QLabel(this);
	const char *name = obs_source_get_name(source);
	m_strName = QString(name);
	m_pText->setText(m_strName);
	m_pText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_pText->setFixedHeight(45);
	m_pText->setStyleSheet(SOURCE_ITEM_TEXT);
	m_pText->setEnabled(obs_sceneitem_visible(m_obsSceneItem));
	pHorLayout->addWidget(m_pText);
	m_pText->setMinimumWidth(60);
	m_pText->setMaximumWidth(130);
	pHorLayout->addStretch();

	auto editclick = [&] {
		connect(this, &SourceItem::EditClicked, SignalProxyModel::GetInstance(), &SignalProxyModel::EditClicked, Qt::UniqueConnection);
		emit EditClicked((void *)(&m_obsSceneItem));
		if (m_pScreenShotDialog)
			m_pScreenShotDialog->show();
	};

	m_pEditButton = new QPushButton(this);
	m_pEditButton->setFixedSize(QSize(16, 16));
	m_pEditButton->setStyleSheet(SOURCE_ITEM_EDIT_BTN);
	m_pEditButton->setVisible(false);
	connect(m_pEditButton, &QPushButton::clicked, this, editclick);
	pHorLayout->addWidget(m_pEditButton);

	auto deleteclick = [&] {
		connect(this, &SourceItem::DeleteClicked, SignalProxyModel::GetInstance(), &SignalProxyModel::DeleteClicked, Qt::UniqueConnection);
		emit DeleteClicked((void *)(&m_obsSceneItem));
	};

	m_pDeleteButton = new QPushButton(this);
	m_pDeleteButton->setFixedSize(QSize(16, 16));
	m_pDeleteButton->setStyleSheet(SOURCE_ITEM_DELETE_BTN);
	m_pDeleteButton->setVisible(false);
	connect(m_pDeleteButton, &QPushButton::clicked, this, deleteclick);
	pHorLayout->addWidget(m_pDeleteButton);

	auto enableclick = [&, pIcon](bool checked) {
		pIcon->setEnabled(checked);
		m_pText->setEnabled(checked);
		obs_sceneitem_set_visible(m_obsSceneItem, checked);
		if (m_pScreenShotDialog) {
			if (checked)
				m_pScreenShotDialog->show();
			else
				m_pScreenShotDialog->hide();
		}
	};

	m_pEnableCheckBox = new QCheckBox(this);
	m_pEnableCheckBox->setFixedSize(QSize(16, 16));
	m_pEnableCheckBox->setStyleSheet(SOURCE_ITEM_CHECK_BOX);
	m_pEnableCheckBox->setChecked(obs_sceneitem_visible(m_obsSceneItem));
	m_pEnableCheckBox->setVisible(false);
	connect(m_pEnableCheckBox, &QCheckBox::clicked, this, enableclick);
	pHorLayout->addWidget(m_pEnableCheckBox);

	pVerLayout->addLayout(pHorLayout);

	QWidget *qSpace = new QWidget(this);
	qSpace->setFixedHeight(1);
	qSpace->setStyleSheet(DIVIDING_LINE_GRAY);
	pVerLayout->addWidget(qSpace);

	setLayout(pVerLayout);
	ElidedText();

	QString temp = StringConvert::GetInstance()->GetString(STR_SCREEN_SHOT);
	if (QString::compare(m_strName.left(3), temp) == 0){
		QDesktopWidget* desktopWidget = QApplication::desktop();
		QRect screenRect = desktopWidget->screenGeometry();
		obs_sceneitem_crop crop;
		obs_sceneitem_get_crop(m_obsSceneItem.Get(), &crop); 
		m_pScreenShotDialog = new ScreenShotDialog(QPoint(crop.left, crop.top), QPoint(screenRect.width() - crop.right, screenRect.height() - crop.bottom), m_obsSceneItem.Get(), this);
		m_pScreenShotDialog->Show();
	}
}

void SourceItem::SetSelect(bool select) {
	if (m_bSelect != select) {
		m_pEditButton->setVisible(select);
		m_pDeleteButton->setVisible(select);
		m_pEnableCheckBox->setVisible(select);
		m_bSelect = select;
	}
}

OBSSceneItem SourceItem::GetSceneItem()
{
	return m_obsSceneItem;
}

void SourceItem::paintEvent(QPaintEvent *event)
{
	QStyleOption option;
	option.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);
}

void SourceItem::enterEvent(QEvent *event)
{
	SetHover(true);
	QWidget::enterEvent(event);
}

void SourceItem::leaveEvent(QEvent *event)
{
	SetHover(false);
	QWidget::leaveEvent(event);
}

void SourceItem::SetHover(bool hover)
{
	if (m_bHover != hover && !m_bSelect) {
		m_pEditButton->setVisible(hover);
		m_pDeleteButton->setVisible(hover);
		m_pEnableCheckBox->setVisible(hover);
		m_bHover = hover;
		int w = hover ? 60 : 130;
		m_pText->setFixedWidth(w);
		ElidedText();
	}
}

void SourceItem::ElidedText() {
	QString text = m_pText->fontMetrics().elidedText(m_strName, Qt::ElideRight,
					  m_pText->width());
	m_pText->setText(text);
}
