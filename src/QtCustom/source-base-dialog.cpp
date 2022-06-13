#include "source-base-dialog.h"

#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QScrollBar>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

#include "common.h"
#include "custom-stylesheet.h"
#include "signal-proxy_model.h"

using namespace std;

#define USER_ROLE Qt::UserRole + 1000

struct AddSourceData {
	obs_source_t *source;
	bool visible;
};

static void AddSource(void *_data, obs_scene_t *scene)
{
	AddSourceData *data = (AddSourceData *)_data;
	obs_sceneitem_t *sceneitem;

	sceneitem = obs_scene_add(scene, data->source);
	obs_sceneitem_set_visible(sceneitem, data->visible);
}

WindowCaptureItem::WindowCaptureItem(const QString &text, QWidget *parent)
	: QWidget(nullptr), m_pSelectBtn(nullptr)
{
	setFixedSize(347, 42);
	QHBoxLayout *mainLayout = new QHBoxLayout();
	mainLayout->setContentsMargins(8, 9, 12, 9);

	QLabel *label = new QLabel(this);
	label->setText(text);
	label->setStyleSheet(WINDOW_CAPTURE_ITEM_LABEL);
	mainLayout->addWidget(label);

	m_pSelectBtn = new QPushButton(this);
	m_pSelectBtn->setFixedSize(24, 24);
	m_pSelectBtn->setStyleSheet(WINDOW_CAPTURE_ITEM_SELECT);
	m_pSelectBtn->setVisible(false);
	mainLayout->addWidget(m_pSelectBtn);

	setLayout(mainLayout);
}
WindowCaptureItem::~WindowCaptureItem() {

}

void WindowCaptureItem::SetSelect(bool select) {
	m_pSelectBtn->setVisible(select);
}

SourceBaseDialog::SourceBaseDialog(OBSScene scene, const char *id, const QString &title, QWidget *parent)
	: MoveDialog(parent),
	  m_scene(scene),
	  m_pID(id),
	  m_pSource(nullptr),
	  m_properties(nullptr, obs_properties_destroy),
	  m_oldSettings(obs_data_create()),
	  m_pContentLayout(nullptr)
{

	m_bUpdate = false;
	InitBaseLayout(title);
	CreateSource();
	PropertiesReload();
}

SourceBaseDialog::SourceBaseDialog(OBSScene scene, obs_source_t *source, const QString &title, QWidget *parent) 
	: MoveDialog(parent),
	  m_scene(scene),
	  m_pID(nullptr),
	  m_pSource(source),
	  m_properties(nullptr, obs_properties_destroy),
	  m_oldSettings(obs_data_create()),
	  m_pContentLayout(nullptr)
{
	m_bUpdate = true;
	InitBaseLayout(title);
	PropertiesReload();
}

SourceBaseDialog::~SourceBaseDialog()
{
	if (!m_bUpdate)
		obs_source_release(m_pSource);
}

void SourceBaseDialog::InitBaseLayout(const QString &strTitle) {
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowModality(Qt::WindowModal);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(0, 44, 0, 67);
	mainLayout->setSpacing(0);

	QHBoxLayout *topLayout = new QHBoxLayout();
	topLayout->setContentsMargins(58,0,58,0);
	topLayout->setSpacing(10);
	mainLayout->addLayout(topLayout);

	QLabel *title = new QLabel(strTitle, this);
	title->setStyleSheet("QLabel {color:rgb(34, 34, 34); font-size:16px; font-weight: 600; font-family:Microsoft YaHei; background:transparent;}");
	topLayout->addWidget(title);
	topLayout->addStretch();
	auto closeClick = [&] {
		obs_source_update(m_pSource, m_oldSettings);

		done(DialogCode::Rejected);
	};

	QPushButton *closeBtn = new QPushButton(this);
	closeBtn->setFixedSize(18, 18);
	closeBtn->setStyleSheet(CLOSE_BTN);
	connect(closeBtn, &QPushButton::clicked, this, closeClick);
	topLayout->addWidget(closeBtn);

	m_pContentLayout = new QVBoxLayout();
	mainLayout->addLayout(m_pContentLayout);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setContentsMargins(0, 0, 0, 0);
	horLayout->setSpacing(15);
	mainLayout->addLayout(horLayout);

	horLayout->addStretch();
	QPushButton *confirmBtn = new QPushButton(QString(tr("add")), this);
	confirmBtn->setFixedSize(90, 32);
	confirmBtn->setStyleSheet("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px; font-size:14px; color:#222222;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");
	connect(confirmBtn, &QPushButton::clicked, this, &SourceBaseDialog::OnConfirmClick);
	horLayout->addWidget(confirmBtn);

	QPushButton *cancelBtn = new QPushButton(QString(tr("cancel")), this);
	cancelBtn->setFixedSize(90, 32);
	cancelBtn->setStyleSheet("QPushButton {background-color:rgb(255,255,255); border:1px solid #EBEBEF; font-size:14px; color:#222222; border-radius:6px; font-weight: 400; font-family:Microsoft YaHei;}"
			"QPushButton:hover {background-color:rgb(232, 229, 237);}");
	connect(cancelBtn, &QPushButton::clicked, this, closeClick);
	horLayout->addWidget(cancelBtn);
	horLayout->addStretch();

	setLayout(mainLayout);
}

void SourceBaseDialog::CreateSource() {
	std::string placeHolderText = common::GetSourceDisplayNameFromId(m_pID);
	std::string text = placeHolderText;
	int i = 2;
	while ((m_pSource = obs_get_source_by_name(text.data()))) {
		obs_source_release(m_pSource);
		char array[512] = "";
		std::sprintf(array, "%s %d", placeHolderText.data(), i++);
		text = array;
	}

	m_pSource = obs_get_source_by_name(text.data());
	if (!m_pSource) {
		const char *v_id = obs_get_latest_input_type_id(m_pID);
		m_pSource = obs_source_create(v_id, text.data(), NULL, nullptr);
	}
}

void SourceBaseDialog::PropertiesReload() {
	if (m_pSource) {
		obs_data_release(m_oldSettings);
		m_settings = obs_source_get_settings(m_pSource);
		obs_data_apply(m_oldSettings, m_settings);
		obs_data_release(m_settings);
		//
		m_properties.reset(obs_source_properties(m_pSource));
	}
}

void SourceBaseDialog::OnConfirmClick() {
	if (!m_scene)
		return;

	if (!BeforeConfirm())
		return;

	if (!m_bUpdate) {
		AddSourceData data;
		data.source = m_pSource;
		data.visible = true;

		obs_enter_graphics();
		obs_scene_atomic_update(m_scene, AddSource, &data);
		obs_leave_graphics();
	}
	SubInterface();

	done(DialogCode::Accepted);
}


WindowCaptureDialog::WindowCaptureDialog(OBSScene scene,const char *id, QWidget *parent)
	: SourceBaseDialog(scene, id, QString(tr("window_capture")), parent),
	  m_pListWidget(nullptr)
{
	setFixedSize(433, 517);
	//SetNinePixmap(":res/images/shader.png", 20, 20, 433, 517);
	InitContentLayout();
}

WindowCaptureDialog::WindowCaptureDialog(OBSScene scene, obs_source_t *source, QWidget *parent)
	: SourceBaseDialog(scene, source, QString(tr("window_capture")), parent), m_pListWidget(nullptr)
{
	setFixedSize(433, 517);
	//SetNinePixmap(":res/images/shader.png", 20, 20, 433, 517);
	InitContentLayout();
}

WindowCaptureDialog::~WindowCaptureDialog()
{
}

void WindowCaptureDialog::InitSource() {
	obs_property_t *property = obs_properties_first(m_properties.get());
	while (property) {
		obs_property_type type = obs_property_get_type(property);
		if (!obs_property_visible(property))
			break;

		if (type == OBS_PROPERTY_LIST) {
			const char *name = obs_property_name(property);
			obs_combo_format format = obs_property_list_format(property);
			if (strcmp(name, "window") == 0) {
				size_t count = obs_property_list_item_count(property);
				for (size_t i = 0; i < count; i++) {
					AddListItem(m_pListWidget, property, format, i);
				}

				string value = obs_data_get_string(m_settings, name);
				for (int i = 0; i < count; i++) {
					QListWidgetItem *item = m_pListWidget->item(i);
					if (item) {
						QVariant var = item->data(USER_ROLE);
						string str = var.toByteArray().constData();
						if (value == str) {
							m_pListWidget->setCurrentRow(i);
						}
					}
				}
			}
		}

		obs_property_next(&property);
	}
}

void WindowCaptureDialog::AddListItem(QListWidget *list, obs_property_t *prop,
				      obs_combo_format format, size_t idx)
{
	const char *name = obs_property_list_item_name(prop, idx);
	QVariant var;

	if (format == OBS_COMBO_FORMAT_STRING) {
		var = QByteArray(obs_property_list_item_string(prop, idx));
	}

	WindowCaptureItem *windowItem = new WindowCaptureItem(QString::fromUtf8(name), list);
	QListWidgetItem *item = new QListWidgetItem();
	//设置 Item 的大小
	item->setSizeHint(QSize(windowItem->width(), windowItem->height()));
	item->setData(USER_ROLE, var);
	list->addItem(item);

	list->setItemWidget(item, windowItem);
}

void WindowCaptureDialog::InitContentLayout()
{
	m_pContentLayout->setContentsMargins(35, 0, 35, 0);
	m_pContentLayout->setSpacing(10);

	QLabel *information = new QLabel(QString(tr("window_capture_info")), this);
	information->setFixedWidth(176);
	information->setStyleSheet(WINDOW_CAPTURE_INFO);
	m_pContentLayout->addWidget(information);

	m_pListWidget = new QListWidget(this);
	m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidget->setFixedWidth(363);
	m_pListWidget->setStyleSheet(WINDOW_CAPTURE_LIST);
	m_pListWidget->verticalScrollBar()->setStyleSheet(WINDOW_CAPTURE_SCROLL);
	connect(m_pListWidget, &QListWidget::currentItemChanged, this, &WindowCaptureDialog::OnCurrentItemChanged);
	m_pContentLayout->addWidget(m_pListWidget);

	InitSource();
}

void WindowCaptureDialog::SubInterface() {}

bool WindowCaptureDialog::BeforeConfirm()
{
	return true;
}

void WindowCaptureDialog::OnCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (current) {
		WindowCaptureItem *item = qobject_cast<WindowCaptureItem *>(m_pListWidget->itemWidget(current));
		item->SetSelect(true);

		QVariant var = current->data(USER_ROLE);
		obs_property_t *property = obs_properties_first(m_properties.get());
		while (property) {
			obs_property_type type = obs_property_get_type(property);
			if (!obs_property_visible(property))
				break;

			if (type == OBS_PROPERTY_LIST) {
				const char *name = obs_property_name(property);
				obs_combo_format format = obs_property_list_format(property);
				if (strcmp(name, "window") == 0) {
					if (format == OBS_COMBO_FORMAT_STRING) {
						obs_data_set_string(m_settings, name, var.toByteArray().constData());
					}
				}
			}

			obs_property_next(&property);
		}

		obs_source_update(m_pSource, m_settings);

	}

	if (previous) {
		WindowCaptureItem *item = qobject_cast<WindowCaptureItem *>(m_pListWidget->itemWidget(previous));
		item->SetSelect(false);
	}
}

CameraDialog::CameraDialog(OBSScene scene, const char* id, QWidget* parent)
	: SourceBaseDialog(scene, id, QString(tr("select_device")), parent), m_pComboBox(nullptr)
{
	setFixedSize(475, 504);
	SetColor("#FCFBFD", 6);
	InitContentLayout();
}

CameraDialog::CameraDialog(OBSScene scene, obs_source_t* source, QWidget* parent)
	: SourceBaseDialog(scene, source, QString(tr("select_device")), parent), m_pComboBox(nullptr)
{
	setFixedSize(475, 504);
	SetColor("#FCFBFD", 6);
	InitContentLayout();
}

CameraDialog::~CameraDialog() {
	killTimer(m_nTimerId);
}

void CameraDialog::InitSource() {
	obs_property_t *property = obs_properties_first(m_properties.get());
	while (property) {
		obs_property_type type = obs_property_get_type(property);
		if (!obs_property_visible(property))
			break;

		if (type == OBS_PROPERTY_LIST) {
			const char *name = obs_property_name(property);
			obs_combo_format format = obs_property_list_format(property);
			if (strcmp(name, "video_device_id") == 0) {
				size_t count = obs_property_list_item_count(property);
				for (size_t i = 0; i < count; i++) {
					const char *name = obs_property_list_item_name(property, i);
					QVariant var;
					if (format == OBS_COMBO_FORMAT_STRING) {
						var = QByteArray(obs_property_list_item_string(property, i));
					}

					m_pComboBox->addItem(QString::fromUtf8(name), var);
				}

				string value = obs_data_get_string(m_settings, name);
				int idx = m_pComboBox->findData(QByteArray(value.c_str()));
				if (idx != -1)
					m_pComboBox->setCurrentIndex(idx);
			}
		}


		obs_property_next(&property);
	}
}

void CameraDialog::InitContentLayout() {
	m_pContentLayout->setContentsMargins(15, 20, 10, 10);
	m_pContentLayout->setSpacing(10);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setContentsMargins(0, 0, 0, 0);
	horLayout->setSpacing(10);
	m_pContentLayout->addLayout(horLayout);

	m_pComboBox = new QComboBox(this);
	m_pComboBox->setFixedSize(300, 36);
	m_pComboBox->setStyleSheet(CAMERA_COMBOBOX);
	m_pComboBox->setView(new QListView());
	connect(m_pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCurrentIndexChanged(int)));
	horLayout->addWidget(m_pComboBox);

	auto click = [&] {
		m_pComboBox->clear();
		InitSource();
	};

	QPushButton *refreshBtn = new QPushButton(this);
	refreshBtn->setFixedSize(16, 16);
	refreshBtn->setStyleSheet(CAMERA_REFRESH_BTN);
	connect(refreshBtn, &QPushButton::clicked, click);
	horLayout->addWidget(refreshBtn);

	InitSource();
}

void CameraDialog::SubInterface() {}

bool CameraDialog::BeforeConfirm()
{
	return true;
}

void CameraDialog::timerEvent(QTimerEvent *event) {
	if(event->timerId() == m_nTimerId){  
		 QVariant var = m_pComboBox->currentData();
		 std::string strName = var.toByteArray().constData();
		 bool bEnable = false;
		/* bool result = SignalProxyModel::GetInstance()->GetVedioSource("dshow_input", strName, bEnable);
		 if (result) {
			 killTimer(m_nTimerId);
			 if (!bEnable)
				QMessageBox::information( NULL, QString("警告"), QString("该摄像头已被其他程序占用"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		 }*/
	} 
}

void CameraDialog::OnCurrentIndexChanged(int index) {
	killTimer(m_nTimerId);
	m_nTimerId = startTimer(100);

	QVariant var = m_pComboBox->itemData(index);
	obs_property_t *property = obs_properties_first(m_properties.get());
	while (property) {
		obs_property_type type = obs_property_get_type(property);
		if (!obs_property_visible(property))
			break;

		if (type == OBS_PROPERTY_LIST) {
			const char *name = obs_property_name(property);
			obs_combo_format format = obs_property_list_format(property);
			if (strcmp(name, "video_device_id") == 0) {
				if (format == OBS_COMBO_FORMAT_STRING) {
					obs_data_set_string(m_settings, name, var.toByteArray().constData());
				}
			}
		}

		obs_property_next(&property);
	}

	obs_source_update(m_pSource, m_settings);
}

ScreenShotDialog::ScreenShotDialog(QPoint beginPt, QPoint endPt, obs_sceneitem_t *item, QWidget *parent)
	: QDialog(parent)
	, m_curDirection(NONE)
	, m_bPressed(false)
	, m_originPt(QPoint(0,0))
	, m_pCloseBtn(nullptr)
	, m_beginPt(beginPt)
	, m_endPt(endPt)
	, m_reallyBeginPt(QPoint(0,0))
	, m_reallyEndPt(QPoint(0,0))
	, m_pSceneItem(item)
{
	setMouseTracking(true);

	int width = m_endPt.x() - m_beginPt.x() + 2 * SCREENSHOT_PADDING;
	int height = m_endPt.y() - m_beginPt.y() + SCREENSHOT_PADDING + SCREENSHOT_TOPHEIGHT;

	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | Qt::WindowStaysOnTopHint);
	resize(width, height);
	setAttribute(Qt::WA_TranslucentBackground);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0,0,0,0);
	mainLayout->setSpacing(0);

	QString qss = QString("QWidget {background-color:black;}");
	QWidget *top = new QWidget(this);
	top->setAttribute(Qt::WA_TransparentForMouseEvents,true);
	top->setFixedHeight(SCREENSHOT_TOPHEIGHT);
	top->setStyleSheet(qss);
	mainLayout->addWidget(top);

	auto click = [&]{
		emit DeleteClicked((void *)(&m_pSceneItem));
	};

	m_pCloseBtn = new QPushButton(this);
	m_pCloseBtn->setStyleSheet(TOP_WIDGET_CLOSE_BTN);
	connect(m_pCloseBtn, &QPushButton::clicked, this, click);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setContentsMargins(0,0,0,0);
	horLayout->setSpacing(0);
	mainLayout->addLayout(horLayout);

	QWidget *left = new QWidget(this);
	left->setAttribute(Qt::WA_TransparentForMouseEvents,true);
	left->setFixedWidth(SCREENSHOT_PADDING);
	left->setStyleSheet(qss);
	horLayout->addWidget(left);
	horLayout->addStretch();

	QWidget *right = new QWidget(this);
	right->setAttribute(Qt::WA_TransparentForMouseEvents,true);
	right->setFixedWidth(SCREENSHOT_PADDING);
	right->setStyleSheet(qss);
	horLayout->addWidget(right);

	QVBoxLayout *verLayout = new QVBoxLayout();
	verLayout->setContentsMargins(0,0,0,0);
	verLayout->setSpacing(SCREENSHOT_PADDING);
	mainLayout->addLayout(verLayout);

	QWidget *bottom = new QWidget(this);
	bottom->setAttribute(Qt::WA_TransparentForMouseEvents,true);
	bottom->setFixedHeight(SCREENSHOT_PADDING);
	bottom->setStyleSheet(qss);
	verLayout->addWidget(bottom);

	connect(this, &ScreenShotDialog::DeleteClicked, SignalProxyModel::GetInstance(), &SignalProxyModel::DeleteClicked, Qt::UniqueConnection);
}

ScreenShotDialog::~ScreenShotDialog() {}

void ScreenShotDialog::Show() {
	int x = m_beginPt.x() - SCREENSHOT_PADDING < 0 ? m_beginPt.x() - SCREENSHOT_PADDING + 2 : m_beginPt.x() - SCREENSHOT_PADDING;
	int y = m_beginPt.y() - SCREENSHOT_TOPHEIGHT < 0 ? m_beginPt.y() - SCREENSHOT_TOPHEIGHT + 2 : m_beginPt.y() - SCREENSHOT_TOPHEIGHT;
	this->move(x, y);
	this->show();
}

ScreenShotDialog::DIRECTION ScreenShotDialog::GetRegion(const QPoint &cursor)
{
	ScreenShotDialog::DIRECTION direction = NONE;
	QPoint ptTl = mapToGlobal(rect().topLeft());
	QPoint ptBr = mapToGlobal(rect().bottomRight());

	int x = cursor.x();
	int y = cursor.y();

	/// 获得鼠标当前所处窗口的边界方向
	if(ptTl.x() + SCREENSHOT_PADDING >= x && ptTl.x() <= x && ptTl.y() + SCREENSHOT_PADDING >= y && ptTl.y() <= y) {
		// 左上角
		direction = LEFTTOP;
		this->setCursor(QCursor(Qt::SizeFDiagCursor));
	} else if(x >= ptBr.x() - SCREENSHOT_PADDING && x <= ptBr.x() && y >= ptBr.y() - SCREENSHOT_PADDING && y <= ptBr.y()) {
		// 右下角
		direction = RIGHTBOTTOM;
		this->setCursor(QCursor(Qt::SizeFDiagCursor));
	} else if(x <= ptTl.x() + SCREENSHOT_PADDING && x >= ptTl.x() && y >= ptBr.y() - SCREENSHOT_PADDING && y <= ptBr.y()) {
		// 左下角
		direction = LEFTBOTTOM;
		this->setCursor(QCursor(Qt::SizeBDiagCursor));
	} else if(x <= ptBr.x() && x >= ptBr.x() - SCREENSHOT_PADDING && y >= ptTl.y() && y <= ptTl.y() + SCREENSHOT_PADDING) {
		// 右上角
		direction = RIGHTTOP;
		this->setCursor(QCursor(Qt::SizeBDiagCursor));
	} else if(x <= ptTl.x() + SCREENSHOT_PADDING && x >= ptTl.x() && y > ptTl.y() + SCREENSHOT_PADDING && y < ptBr.y() - SCREENSHOT_PADDING) {
		// 左边
		direction = LEFT;
		this->setCursor(QCursor(Qt::SizeHorCursor));
	} else if( x <= ptBr.x() && x >= ptBr.x() - SCREENSHOT_PADDING && y > ptTl.y() + SCREENSHOT_PADDING && y < ptBr.y() - SCREENSHOT_PADDING) {
		// 右边
		direction = RIGHT;
		this->setCursor(QCursor(Qt::SizeHorCursor));
	}else if(y >= ptTl.y() && y <= ptTl.y() + SCREENSHOT_PADDING && x > ptTl.x() + SCREENSHOT_PADDING && x < ptBr.x() - SCREENSHOT_PADDING){
		// 上边
		direction = TOP;
		this->setCursor(QCursor(Qt::SizeVerCursor));
	} else if(y <= ptBr.y() && y >= ptBr.y() - SCREENSHOT_PADDING && x > ptTl.x() + SCREENSHOT_PADDING && x < ptBr.x() - SCREENSHOT_PADDING) {
		// 下边
		direction = BOTTOM;
		this->setCursor(QCursor(Qt::SizeVerCursor));
	} else {
		// 默认
		direction = NONE;
		this->setCursor(QCursor(Qt::ArrowCursor));
	}

	return direction;
}

void ScreenShotDialog::OnMouseChange(int x, int y) {
	if (x < 0 || y < 0) 
		return;

	const int& rx = (x >= m_originPt.x()) ? m_originPt.x() : x;
	const int& ry = (y >= m_originPt.y()) ? m_originPt.y() : y;
	const int& rw = abs(x - m_originPt.x());
	const int& rh = abs(y - m_originPt.y());

	this->setGeometry(QRect(rx, ry, rw, rh));
	update();
}

void ScreenShotDialog::GetCrop(int &left, int &top, int &right, int &bottom) {
	QDesktopWidget* desktopWidget = QApplication::desktop();
	QRect screenRect = desktopWidget->screenGeometry();

	QPoint tlPt = this->mapToGlobal(rect().topLeft());
	QPoint brPt = this->mapToGlobal(rect().bottomRight());
	left = tlPt.x() + SCREENSHOT_PADDING;
	top = tlPt.y() + SCREENSHOT_TOPHEIGHT;
	right = screenRect.width() - brPt.x() - SCREENSHOT_PADDING;
	bottom = screenRect.height() - brPt.y() - SCREENSHOT_PADDING;
}

void ScreenShotDialog::resizeEvent(QResizeEvent *event) {
	if (m_pCloseBtn) 
		m_pCloseBtn->setGeometry(rect().topRight().x() - 23, rect().topRight().y() + 6, 18, 18);

	QDialog::resizeEvent(event);
}

void ScreenShotDialog::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		m_bPressed = true;
		m_pressedPt = event->globalPos();
		if (m_curDirection != NONE)
			this->mouseGrabber();
	}
	QDialog::mousePressEvent(event);
}

void ScreenShotDialog::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		m_bPressed = false;
	}
	QDialog::mouseReleaseEvent(event);
}

void ScreenShotDialog::mouseMoveEvent(QMouseEvent *event) {
	QPoint gloPoint = mapToGlobal(event->pos());
	QPoint tlPt = mapToGlobal(rect().topLeft());
	QPoint blPt = mapToGlobal(rect().bottomLeft());
	QPoint brPt = mapToGlobal(rect().bottomRight());
	QPoint trPt = mapToGlobal(rect().topRight());

	if (!m_bPressed) {
		m_curDirection = GetRegion(gloPoint);
		// 根据方位判断拖拉对应支点
		switch (m_curDirection) {
		case NONE:
		case RIGHT:
		case RIGHTBOTTOM:
			m_originPt = tlPt;
			break;
		case RIGHTTOP:
			m_originPt = blPt;
			break;
		case LEFT:
		case LEFTBOTTOM:
			m_originPt = trPt;
			break;
		case LEFTTOP:
		case TOP:
			m_originPt = brPt;
			break;
		case BOTTOM:
			m_originPt = tlPt;
			break;
		default:
			break;
		}

	} else {
		if (m_curDirection != NONE) {
			switch (m_curDirection) {
			case LEFT:
				OnMouseChange(gloPoint.x(), blPt.y() + 1);
				break;
			case RIGHT:
				OnMouseChange(gloPoint.x(), brPt.y() + 1);
				break;
			case TOP:
				OnMouseChange(tlPt.x(), gloPoint.y());
				break;
			case BOTTOM:
				OnMouseChange(brPt.x() + 1, gloPoint.y());
				break;
			case LEFTTOP:
			case RIGHTTOP:
			case LEFTBOTTOM:
			case RIGHTBOTTOM:
				OnMouseChange(gloPoint.x(), gloPoint.y());
				break;
			default:
				break;
			}

		} else {
			m_movePt = event->globalPos();
			this->move(this->pos() + m_movePt - m_pressedPt);
			m_pressedPt = m_movePt;
		}
	}

	QDialog::mouseMoveEvent(event);
}

void ScreenShotDialog::moveEvent(QMoveEvent *event) {
	int left=0, top=0, right=0, bottom=0;
	GetCrop(left, top, right, bottom);
	if (m_pSceneItem) {
		obs_sceneitem_defer_update_begin(m_pSceneItem);
		obs_sceneitem_crop crop{left,top,right,bottom};
		obs_sceneitem_set_crop(m_pSceneItem, &crop);
		obs_sceneitem_defer_update_end(m_pSceneItem);
	}

	QDialog::moveEvent(event);
}
