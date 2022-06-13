#include "url-label.h"
#include "custom-config-ini.h"
#include "md5.h"
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QTimer>
#include <QBitmap>
#include <QPixmap>

#define RADIUS	8.0

QUrlLabel::QUrlLabel(QWidget *parent,bool round)
	: QLabel(parent), m_pRequest(nullptr), m_bRound(round)
{
	check = new QTimer(this);
	check->setInterval(1000);
	connect(check, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
}

void QUrlLabel::OnTimeOut() {
	Update();
}

QUrlLabel::~QUrlLabel() {

}

void QUrlLabel::SetTask(const RequestTask &task) {
	m_task = task;
	MD5 md5(task.url.toStdString().c_str());
	std::string name = md5.toString();
	std::string imagePath = CustomConfig::GetImagePath();
	std::string path = imagePath + "/" + name;
	m_strFile = QString::fromStdString(path);
	if (QFile::exists(m_strFile)) {
		if (Update())
			return;
		else
			check->start();
	}

	if (m_pRequest.get()) {
		m_pRequest.reset();
	}
	m_pRequest = NetworkRequestFactory::create(TYPE_DOWNLOAD);
	connect(m_pRequest.get(), &NetRequest::requestFinished, this,
		&QUrlLabel::OnRequestFinished);
	m_pRequest->SetTask(task);
	m_pRequest->Start();
}

void QUrlLabel::SetNetPath(const QString &url) {
	RequestTask task;
	task.url = url;
	task.type = TYPE_DOWNLOAD;
	SetTask(task);
}

QString &QUrlLabel::GetMd5NetPath()
{
	return m_strFile;
}

void QUrlLabel::OnRequestFinished(bool success, const QByteArray &, const QString &) {
	if (success) {
		Update();
	}
}


bool QUrlLabel::Update()
{
	QPixmap pixmap;
	bool bLoad = pixmap.load(m_strFile);
	pixmap = pixmap.scaled(width(), height(), Qt::IgnoreAspectRatio,
			       Qt::SmoothTransformation);
	
	if (bLoad) {
		setPixmap(pixmap);
		check->stop();
	}
	else
		check->start();
	return bLoad;
}

void QUrlLabel::paintEvent(QPaintEvent *event) {
	if (m_bRound) {
		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing |
				       QPainter::SmoothPixmapTransform);
		QPainterPath path;
		int round = qMin(width(), height());
		path.addEllipse(0, 0, round, round);
		painter.setClipPath(path);
		painter.drawPixmap(0, 0, width(), height(), *pixmap());
	} else {
		QLabel::paintEvent(event);
	}
}

UrlRadiusLabel::UrlRadiusLabel(int type, QWidget *parent)
	: QUrlLabel(parent)
	, m_bSelect(false)
	, m_ntype(type)
{
	m_bHover = false;
	setAttribute(Qt::WA_TranslucentBackground);
}

UrlRadiusLabel::~UrlRadiusLabel() {

}

void UrlRadiusLabel::SetSelect(bool select) {
	m_bSelect = select;
	update();
}

int UrlRadiusLabel::GetType()
{
	return m_ntype;
}

QPixmap UrlRadiusLabel::Round(const QPixmap*pixmap, qreal radius)
{
	if (pixmap==nullptr)
		return QPixmap();
	if (pixmap->isNull())
		return QPixmap();

	QSize size(pixmap->size());
	QBitmap mask(size);
	QPainter painter(&mask);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.fillRect(mask.rect(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));

	QRectF rect = QRect(0, 0, size.width(), size.height());
	QPainterPath path;
	path.moveTo(rect.bottomRight() - QPointF(0, radius));
	path.lineTo(rect.topRight() + QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(radius * 2, 0)), QSize(radius * 2, radius *2)), 0, 90);
	path.lineTo(rect.topLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
	path.lineTo(rect.bottomLeft() - QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, radius * 2)), QSize(radius * 2, radius * 2)), 180, 90);
	path.lineTo(rect.bottomLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(radius * 2, radius * 2)), QSize(radius * 2, radius * 2)), 270, 90);
	painter.drawPath(path);

	QPixmap image = *pixmap;
	image.setMask(mask);
	return image;
}

void UrlRadiusLabel::enterEvent(QEvent *event) {
	m_bHover = true;
	update();
	QLabel::enterEvent(event);
}

void UrlRadiusLabel::leaveEvent(QEvent *event) {
	m_bHover = false;
	update();
	QLabel::leaveEvent(event);
}

void UrlRadiusLabel::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QRectF rect = QRect(0, 0, width(), height());
	QPainterPath path;
	path.moveTo(rect.bottomRight() - QPointF(0, RADIUS));
	path.lineTo(rect.topRight() + QPointF(0, RADIUS));
	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(RADIUS * 2, 0)), QSize(RADIUS * 2, RADIUS *2)), 0, 90);
	path.lineTo(rect.topLeft() + QPointF(RADIUS, 0));
	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(RADIUS * 2, RADIUS * 2)), 90, 90);
	path.lineTo(rect.bottomLeft() - QPointF(0, RADIUS));
	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, RADIUS * 2)), QSize(RADIUS * 2, RADIUS * 2)), 180, 90);
	path.lineTo(rect.bottomLeft() + QPointF(RADIUS, 0));
	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(RADIUS * 2, RADIUS * 2)), QSize(RADIUS * 2, RADIUS * 2)), 270, 90);

	if (m_bHover | m_bSelect) {
		painter.setPen(QPen(QBrush(QColor(255, 185, 0)), 4, Qt::SolidLine));
		painter.drawPath(path);

		if (m_radPixmap.isNull())
			m_radPixmap = Round(this->pixmap(), 16);

		painter.drawPixmap(6, 6, width() -12, height() - 12, m_radPixmap.scaled(width() - 12, height() - 12, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	} else {
		painter.setPen(QPen(QBrush(QColor(238,238,238)), 1, Qt::SolidLine));
		painter.drawPath(path);
		if (m_radPixmap.isNull())
			m_radPixmap = Round(this->pixmap(), 16);

		painter.drawPixmap(1, 1, width() - 2, height() - 2, m_radPixmap.scaled(width() - 2, height() - 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	}
}

void UrlRadiusLabel::mouseReleaseEvent(QMouseEvent *ev) {
	if (ev->button() == Qt::LeftButton) {
		emit click(this);
	}
}

NoneStateLabel::NoneStateLabel(QWidget *parent)
	: QLabel(parent)
	, m_bHover(false)
	, m_bSelect(false)
{
	setAttribute(Qt::WA_TranslucentBackground);
}

NoneStateLabel::~NoneStateLabel() {}

void NoneStateLabel::SetSelect(bool bSelect) {
	m_bSelect = bSelect;
	update();
}

void NoneStateLabel::enterEvent(QEvent *event) {
	m_bHover = true;
	update();
	QLabel::enterEvent(event);
}

void NoneStateLabel::leaveEvent(QEvent *event) {
	m_bHover = false;
	update();
	QLabel::leaveEvent(event);
}

void NoneStateLabel::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QRectF rect = QRect(0, 0, width(), height());
	QPainterPath path;
	path.moveTo(rect.bottomRight() - QPointF(0, RADIUS));
	path.lineTo(rect.topRight() + QPointF(0, RADIUS));
	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(RADIUS * 2, 0)), QSize(RADIUS * 2, RADIUS *2)), 0, 90);
	path.lineTo(rect.topLeft() + QPointF(RADIUS, 0));
	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(RADIUS * 2, RADIUS * 2)), 90, 90);
	path.lineTo(rect.bottomLeft() - QPointF(0, RADIUS));
	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, RADIUS * 2)), QSize(RADIUS * 2, RADIUS * 2)), 180, 90);
	path.lineTo(rect.bottomLeft() + QPointF(RADIUS, 0));
	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(RADIUS * 2, RADIUS * 2)), QSize(RADIUS * 2, RADIUS * 2)), 270, 90);

	if (m_bHover | m_bSelect) {
		painter.setPen(QPen(QBrush(QColor(255, 185, 0)), 4, Qt::SolidLine));
		painter.drawPath(path);

	} else {
		painter.setPen(QPen(QBrush(QColor(238,238,238)), 1, Qt::SolidLine));
		painter.drawPath(path);
	}

	if(pixmap()) {
		painter.drawPixmap(30, 25, pixmap()->width(), pixmap()->height(), *pixmap());
	}
	QFont font("Microsoft YaHei");
	font.setPixelSize(14);
	painter.setFont(font);
	painter.setPen(QPen(QColor(153, 153, 153)));
	painter.drawText(35, 70, QString(tr("none")));
}

void NoneStateLabel::mouseReleaseEvent(QMouseEvent *ev) {
	if (ev->button() == Qt::LeftButton) {
		emit click(this);
	}
}

MaskFilterLabel::MaskFilterLabel(QWidget *parent)
	: QLabel(parent)
	, m_bHover(false)
	, m_bSelect(false)
	, m_type(Mask_Toushi_0)
{
	setAttribute(Qt::WA_TranslucentBackground);
}

MaskFilterLabel::~MaskFilterLabel() {}

void MaskFilterLabel::SetSelect(bool bSelect) {
	m_bSelect = bSelect;
	update();
}

void MaskFilterLabel::SetData(Mask_Type type) {
	m_type = type;
}

Mask_Type MaskFilterLabel::GetData()
{
	return m_type;
}

void MaskFilterLabel::enterEvent(QEvent *event) {
	m_bHover = true;
	update();
	QLabel::enterEvent(event);
}

void MaskFilterLabel::leaveEvent(QEvent *event) {
	m_bHover = false;
	update();
	QLabel::leaveEvent(event);
}

void MaskFilterLabel::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QRectF rect = QRect(0, 0, width(), height());
	QPainterPath path;
	path.moveTo(rect.bottomRight() - QPointF(0, RADIUS));
	path.lineTo(rect.topRight() + QPointF(0, RADIUS));
	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(RADIUS * 2, 0)), QSize(RADIUS * 2, RADIUS *2)), 0, 90);
	path.lineTo(rect.topLeft() + QPointF(RADIUS, 0));
	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(RADIUS * 2, RADIUS * 2)), 90, 90);
	path.lineTo(rect.bottomLeft() - QPointF(0, RADIUS));
	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, RADIUS * 2)), QSize(RADIUS * 2, RADIUS * 2)), 180, 90);
	path.lineTo(rect.bottomLeft() + QPointF(RADIUS, 0));
	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(RADIUS * 2, RADIUS * 2)), QSize(RADIUS * 2, RADIUS * 2)), 270, 90);

	if (m_bHover | m_bSelect) {
		painter.setPen(QPen(QBrush(QColor(255, 185, 0)), 4, Qt::SolidLine));
		painter.drawPath(path);

	} else {
		painter.setPen(QPen(QBrush(QColor(238,238,238)), 1, Qt::SolidLine));
		painter.drawPath(path);
	}

	if(pixmap()) {
		painter.drawPixmap(6, 6, width() -12, height() - 12, pixmap()->scaled(width() - 12, height() - 12, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
	}
}

void MaskFilterLabel::mouseReleaseEvent(QMouseEvent *ev) {
	if (ev->button() == Qt::LeftButton) {
		emit click(this);
	}
}
