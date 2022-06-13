#include "picture-dialog.h"
#include "custom-stylesheet.h"

#include <QPainter>
#include <QPixmap>
#include <QMouseEvent>
#include <QFileDialog>
#include <QLineEdit>
#include <QDebug>

#include "common.h"
#include "msgbox.h"
#include "string-convert.h"

static double aspect_ratio = 16.0 / 9.0;

#define PADDING 3

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

TailorWidget::TailorWidget(QWidget *parent)
	: QWidget(parent)
	, m_dRatio(0.00)
	, m_bLeftPress(false)
	, m_bFirstPicture(false)
	, m_curDirection(None)
	, m_originPt(0,0)
	, m_pressPt(0,0)
{
	setMouseTracking(true);
}

TailorWidget::TailorWidget(const QString &path, QWidget *parent)
	: TailorWidget(parent)
{
	SetPath(path);
}

TailorWidget::~TailorWidget() {}

void TailorWidget::SetPath(const QString &path) {
	m_strPath = path;
	if (m_strPath.isEmpty())
		m_pixmap = QPixmap(":res/images/photo.png");
	else 
		m_pixmap = QPixmap(path);
	m_dRatio = (double)m_pixmap.width() / (double)m_pixmap.height();
	m_bFirstPicture = true;
}

//void TailorWidget::GetCrop(int &left, int &top, int &right, int &bottom)
//{
//	double scaleRate = 0.0;
//	if (m_dRatio > aspect_ratio) {
//		scaleRate = (double)m_pixmap.width() / (double)width();
//	} else {
//		scaleRate = (double)m_pixmap.height() / (double)height();
//	}
//
//	left = (m_rcTailor.left() - m_rcLimit.left()) * scaleRate;
//	top = (m_rcTailor.top() - m_rcLimit.top()) * scaleRate;
//	right = (m_rcLimit.right() - m_rcTailor.right()) * scaleRate;
//	bottom = (m_rcLimit.bottom() - m_rcTailor.bottom()) * scaleRate;
//}

void TailorWidget::paintEvent(QPaintEvent *event){
	QPainter paint(this);

	paint.fillRect(QRect(0,0,width(),height()), QColor(238, 238, 238));

	QPoint point(0,0);
	QPixmap pixmap;
	//
	if (!m_pixmap.isNull()) {
		if (m_strPath.isEmpty()) {
			pixmap = m_pixmap;
			point = QPoint((width() - pixmap.width()) / 2, (height() - pixmap.height()) / 2);
			paint.drawPixmap(point, pixmap);
		} else {
			if (m_dRatio > aspect_ratio) {
				pixmap = m_pixmap.scaledToWidth(width(), Qt::SmoothTransformation);
				point = QPoint(0, (height() - pixmap.height()) / 2);
				paint.drawPixmap(point, pixmap);
			} else {
				pixmap = m_pixmap.scaledToHeight(height(), Qt::SmoothTransformation);
				point = QPoint((width() - pixmap.width()) / 2, 0);
				paint.drawPixmap(point, pixmap);
			}
		}
	}

	//if (m_bFirstPicture) {
	//	m_rcTailor = QRect(point.x(), point.y(), pixmap.width(), pixmap.height());
	//	m_rcLimit = m_rcTailor;
	//	m_bFirstPicture = false;
	//}

	//// 
	//QPen pen(QColor(255,255,255));
	//paint.setPen(pen);
	//paint.drawRect(m_rcTailor.x(), m_rcTailor.y(), m_rcTailor.width() - 1, m_rcTailor.height() - 1);

	////
	//QRect rcArray[12] = {QRect(m_rcTailor.left(),m_rcTailor.top(),PADDING,PADDING), QRect(m_rcTailor.right()-PADDING+1,m_rcTailor.top(),PADDING,PADDING),
	//	QRect(m_rcTailor.right()-PADDING+1,m_rcTailor.bottom()-PADDING+1,PADDING,PADDING), QRect(m_rcTailor.left(),m_rcTailor.bottom()-PADDING+1,PADDING,PADDING),
	//	QRect(m_rcTailor.left()+PADDING,m_rcTailor.top(),7,PADDING), QRect(m_rcTailor.left(),m_rcTailor.top()+PADDING,PADDING,7),
	//	QRect(m_rcTailor.right()-10+1,m_rcTailor.top(),7,PADDING), QRect(m_rcTailor.right()-PADDING+1,m_rcTailor.top()+PADDING,PADDING,7),
	//	QRect(m_rcTailor.right()-PADDING+1,m_rcTailor.bottom()-10+1,PADDING,7), QRect(m_rcTailor.right()-10+1,m_rcTailor.bottom()-PADDING+1,7,PADDING),
	//	QRect(m_rcTailor.left()+PADDING,m_rcTailor.bottom()-PADDING+1,7,PADDING), QRect(m_rcTailor.left(),m_rcTailor.bottom()-10+1,PADDING,7)};

	//for(auto rc : rcArray) {
	//	paint.fillRect(rc, QColor(255,184,0));
	//}
}

//void TailorWidget::mousePressEvent(QMouseEvent *event) {
//	if (event->button() == Qt::LeftButton) {
//		m_bLeftPress = true;
//		m_pressPt = event->pos();
//	}
//}

//void TailorWidget::mouseReleaseEvent(QMouseEvent *event) {
//	if (event->button() == Qt::LeftButton) {
//		m_bLeftPress = false;
//		m_pressPt = QPoint(0,0);
//	}
//}

//void TailorWidget::mouseMoveEvent(QMouseEvent *event) {
//	QPoint curpt = event->pos();
//	QPoint tlPt = m_rcTailor.topLeft();
//	QPoint blPt = QPoint(m_rcTailor.left(), m_rcTailor.top() + m_rcTailor.height());
//	QPoint brPt = QPoint(m_rcTailor.left() + m_rcTailor.width(), m_rcTailor.top() + m_rcTailor.height());
//	QPoint trPt = QPoint(m_rcTailor.left() + m_rcTailor.width(), m_rcTailor.top());
//
//	if (!m_bLeftPress) {
//		m_curDirection = GetRegion(event->pos());
//		switch (m_curDirection) {
//		case Right:
//		case RightBottom:
//			m_originPt = tlPt;
//			break;
//		case RightTop:
//			m_originPt = blPt;
//			break;
//		case Left:
//		case LeftBottom:
//			m_originPt = trPt;
//			break;
//		case LeftTop:
//		case Top:
//			m_originPt = brPt;
//			break;
//		case Bottom:
//			m_originPt = tlPt;
//			break;
//		default:
//			break;
//		}
//	} else {
//		if (m_curDirection != None) {
//			switch (m_curDirection) {
//			case Center:
//				OnMove(curpt);
//				break;
//			case Left:
//				OnMouseChange(curpt.x(), blPt.y());
//				break;
//			case Right:
//				OnMouseChange(curpt.x(), brPt.y());
//				break;
//			case Top:
//				OnMouseChange(tlPt.x(), curpt.y());
//				break;
//			case Bottom:
//				OnMouseChange(brPt.x(), curpt.y());
//				break;
//			case LeftTop:
//			case RightTop:
//			case LeftBottom:
//			case RightBottom:
//				OnMouseChange(curpt.x(), curpt.y());
//				break;
//			default:
//				break;
//			}
//		}
//	}
//}

//TailorWidget::Direction TailorWidget::GetRegion(QPoint pt)
//{
//	Direction direction = None;
//	QPoint ltpt = m_rcTailor.topLeft();
//	QPoint rbpt = m_rcTailor.bottomRight();
//
//	int x = pt.x();
//	int y = pt.y();
//
//	if (m_rcTailor.contains(pt)) {
//		if(ltpt.x() + PADDING >= x && ltpt.x() <= x && ltpt.y() + PADDING >= y && ltpt.y() <= y) {
//			direction = LeftTop;
//			this->setCursor(QCursor(Qt::SizeFDiagCursor));
//		} else if(x >= rbpt.x() - PADDING && x <= rbpt.x() && y >= rbpt.y() - PADDING && y <= rbpt.y()) {
//			direction = RightBottom;
//			this->setCursor(QCursor(Qt::SizeFDiagCursor));
//		} else if(x <= ltpt.x() + PADDING && x >= ltpt.x() && y >= rbpt.y() - PADDING && y <= rbpt.y()) {
//			direction = LeftBottom;
//			this->setCursor(QCursor(Qt::SizeBDiagCursor));
//		} else if(x <= rbpt.x() && x >= rbpt.x() - PADDING && y >= ltpt.y() && y <= ltpt.y() + PADDING) {
//			direction = RightTop;
//			this->setCursor(QCursor(Qt::SizeBDiagCursor));
//		} else if(x <= ltpt.x() + PADDING && x >= ltpt.x() && y > ltpt.y() + PADDING && y < rbpt.y() - PADDING) {
//			direction = Left;
//			this->setCursor(QCursor(Qt::SizeHorCursor));
//		} else if( x <= rbpt.x() && x >= rbpt.x() - PADDING && y > ltpt.y() + PADDING && y < rbpt.y() - PADDING) {
//			direction = Right;
//			this->setCursor(QCursor(Qt::SizeHorCursor));
//		}else if(y >= ltpt.y() && y <= ltpt.y() + PADDING && x > ltpt.x() + PADDING && x < rbpt.x() - PADDING){
//			direction = Top;
//			this->setCursor(QCursor(Qt::SizeVerCursor));
//		} else if(y <= rbpt.y() && y >= rbpt.y() - PADDING && x > ltpt.x() + PADDING && x < rbpt.x() - PADDING) {
//			direction = Bottom;
//			this->setCursor(QCursor(Qt::SizeVerCursor));
//		} else {
//			direction = Center;
//			this->setCursor(QCursor(Qt::SizeAllCursor));
//		}
//	} else {
//		this->setCursor(QCursor(Qt::ArrowCursor));
//	}
//
//	return direction;
//}

//void TailorWidget::OnMouseChange(int x, int y) {
//	if (x < 0 || y < 0 || x < m_rcLimit.x() || x > m_rcLimit.x() + m_rcLimit.width()
//		|| y < m_rcLimit.y() || y > m_rcLimit.y() + m_rcLimit.height())
//		return;
//
//	const int& rx = (x >= m_originPt.x()) ? m_originPt.x() : x;
//	const int& ry = (y >= m_originPt.y()) ? m_originPt.y() : y;
//	const int& rw = abs(x - m_originPt.x());
//	const int& rh = abs(y - m_originPt.y());
//	
//	if (rw < 50 || rh < 50 || rw > m_rcLimit.width() - (rx - m_originPt.x())
//		|| rh > m_rcLimit.height() - (ry - m_originPt.y()))
//		return;
//
//	m_rcTailor.setX(rx);
//	m_rcTailor.setY(ry);
//	m_rcTailor.setWidth(rw);
//	m_rcTailor.setHeight(rh);
//	update();
//}

//void TailorWidget::OnMove(QPoint pt) {
//	QPoint tempPt = pt - m_pressPt;
//	QPoint lt = m_rcTailor.topLeft() + tempPt;
//	lt.setX(lt.x() < m_rcLimit.x() ? m_rcLimit.x() : lt.x());
//	lt.setY(lt.y() < m_rcLimit.y() ? m_rcLimit.y() : lt.y());
//
//	int w = m_rcTailor.width();
//	int h = m_rcTailor.height();
//
//	if (lt.x() + w > m_rcLimit.x() + m_rcLimit.width())
//		lt.setX(m_rcLimit.x() + m_rcLimit.width() - w);
//
//	if (lt.y() + h > m_rcLimit.y() + m_rcLimit.height())
//		lt.setY(m_rcLimit.y() + m_rcLimit.height() - h);
//
//	m_rcTailor.setTopLeft(lt);
//	m_rcTailor.setWidth(w);
//	m_rcTailor.setHeight(h);
//	m_pressPt = pt;
//	update();
//}

PictureDialog::PictureDialog(OBSScene scene, const char *id, QWidget *parent)
	: SourceBaseDialog(scene, id, QString(tr("add_picture_title")), parent)
	, m_pLineEdit(nullptr)
	, m_pTailorWidget(nullptr)
{
	setFixedSize(503, 491);
	SetPixmap(":/res/images/picture_mask.png");
	InitContentLayout();
}

PictureDialog::PictureDialog(OBSScene scene, obs_source_t *source, QWidget *parent)
	: SourceBaseDialog(scene, source, QString(tr("update_picture_title")), parent)
	, m_pLineEdit(nullptr)
	, m_pTailorWidget(nullptr)
{
	setFixedSize(503, 491);
	SetPixmap(":/res/images/picture_mask.png");
	InitContentLayout();
}

PictureDialog::~PictureDialog() {}

void PictureDialog::InitSource() {
	obs_property_t *property = obs_properties_first(m_properties.get());
	while (property) {
		obs_property_type type = obs_property_get_type(property);
		if (!obs_property_visible(property))
			break;

		if (type == OBS_PROPERTY_PATH) {
			const char *name = obs_property_name(property);
			if (strcmp(name, "file") == 0) {
				std::string value = obs_data_get_string(m_settings, name);
				if (!value.empty()) {
					m_strPicturePath = QString::fromUtf8(value.c_str());
				}
			}
		}

		obs_property_next(&property);
	}
}

void PictureDialog::InitContentLayout() {
	InitSource();

	m_pContentLayout->setContentsMargins(58, 20, 58, 20);
	m_pContentLayout->setSpacing(10);

	QHBoxLayout *horlayout = new QHBoxLayout();
	horlayout->setContentsMargins(0,0,0,0);
	horlayout->setSpacing(0);
	m_pContentLayout->addLayout(horlayout);

	QLabel *label = new QLabel(QString(tr("picture_path")), this);
	label->setStyleSheet("QLabel{color:rgb(68, 68, 68);font-size:12px;background-color:transparent; font-family:Microsoft YaHei;}");
	horlayout->addWidget(label);

	m_pLineEdit = new QLineEdit(this);
	m_pLineEdit->setReadOnly(true);
	m_pLineEdit->setFixedSize(257,36);
	m_pLineEdit->setStyleSheet("QLineEdit {border:1px solid rgb(217, 212, 227); border-top-left-radius:4px; border-bottom-left-radius:4px; color:rgb(68, 68, 68);font-size:12px;background-color:transparent; font-family:Microsoft YaHei; padding-left:10px;}");
	horlayout->addWidget(m_pLineEdit);

	auto click = [&] {
		QString path = QFileDialog::getOpenFileName(this, QString(tr("open_image")), m_strPicturePath, "Images (*.bmp;*.jpg;*.jpeg;*.tga;*.gif;*.png)");
		if (!path.isEmpty() && QFile(path).exists()) {
			m_strPicturePath = path;
			m_pLineEdit->setText(m_strPicturePath);
			m_pTailorWidget->SetPath(path);
		}
	};

	QPushButton *pushBtn = new QPushButton(QString(tr("change")), this);
	pushBtn->setFixedSize(60,36);
	pushBtn->setStyleSheet("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00); font-size:14px; color:#222222; border-top-right-radius:4px;border-bottom-right-radius:4px;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");
	connect(pushBtn, &QPushButton::clicked, click);
	horlayout->addWidget(pushBtn);
	horlayout->addStretch();

	QLabel *label1 = new QLabel(QString(tr("picture_preview")), this);
	label1->setStyleSheet("QLabel{color:rgb(68, 68, 68);font-size:12px;background-color:transparent; font-family:Microsoft YaHei;}");
	m_pContentLayout->addWidget(label1);

	m_pTailorWidget = new TailorWidget("", this);
	m_pTailorWidget->setFixedSize(384, 216);
	m_pContentLayout->addWidget(m_pTailorWidget);
	m_pContentLayout->addStretch();
}

void PictureDialog::SubInterface() {
	obs_property_t *property = obs_properties_first(m_properties.get());
	while (property) {
		obs_property_type type = obs_property_get_type(property);
		if (!obs_property_visible(property))
			break;

		if (type == OBS_PROPERTY_PATH) {
			const char *name = obs_property_name(property);
			if (strcmp(name, "file") == 0) {
				obs_data_set_string(m_settings, name, m_strPicturePath.toUtf8().constData());
			}
		}

		obs_property_next(&property);
	}

	obs_source_update(m_pSource, m_settings);

	/*int left=0, top=0, right=0, bottom=0;
	if (m_pTailorWidget)
		m_pTailorWidget->GetCrop(left, top, right, bottom);
	const char *name = obs_source_get_name(m_pSource);
	obs_sceneitem_t *sceneitem = obs_scene_find_source(m_scene, name);
	if (sceneitem) {
		obs_sceneitem_defer_update_begin(sceneitem);
		obs_sceneitem_crop crop{left,top,right,bottom};
		obs_sceneitem_set_crop(sceneitem, &crop);
		obs_sceneitem_defer_update_end(sceneitem);
	}*/
}

bool PictureDialog::BeforeConfirm()
{
	if (m_strPicturePath.isEmpty())
		MsgBox::information(nullptr, StringConvert::GetInstance()->GetString(STR_PROCESS), StringConvert::GetInstance()->GetString(STR_IMAGE_NOEMPTY));

	return !m_strPicturePath.isEmpty();
}
