#include "mask-label.h"
#include <QPainter>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QNetworkReply>

MaskLabel::MaskLabel(QWidget *parent)
	: QLabel(parent), manager(nullptr), reply(nullptr)
{
	manager = new QNetworkAccessManager;
}

MaskLabel::~MaskLabel() {
	if (manager != nullptr) {
		manager->deleteLater();
		manager = nullptr;
	}
	if (reply != nullptr) {
		reply->deleteLater();
		reply = nullptr;
	}
}

void MaskLabel::SetNetPath(const QString &path) {
	if (!path.isEmpty()) {
		reply = manager->get(QNetworkRequest(path));
		connect(reply, SIGNAL(readyRead()), this, SLOT(OnReadyRead()));
		connect(reply, SIGNAL(finished()), this, SLOT(OnFinished()));
	}
}

void MaskLabel::OnReadyRead() {

}

void MaskLabel::OnFinished() {
	if (reply != nullptr) {
		QByteArray jpegData = reply->readAll();
		QPixmap pixmap;
		pixmap.loadFromData(jpegData);
		pixmap = pixmap.scaled(width(), height(), Qt::IgnoreAspectRatio,
				       Qt::SmoothTransformation);
		setPixmap(pixmap);
		update();
	}
}

void MaskLabel::paintEvent(QPaintEvent *event) {
	if (pixmap()) {
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
