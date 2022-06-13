#include "translucent-widget.h"
#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>

TranslucenWidget::TranslucenWidget(QWidget *parent)
	: QDialog(parent)
{
	m_nRadius = 0;
}

void TranslucenWidget::SetPixmap(const QString &strPixmap)
{
	bool bLoad = pixmap.load(strPixmap);
	QSize size = pixmap.size();
	resize(pixmap.size());
	if (bLoad) {
		setMask(pixmap.mask());
	}
}

void TranslucenWidget::SetColor(const QString &color, int radius,bool border,int w,QString strColor) {
	pixmap = QPixmap(width(), height());
	pixmap.fill(Qt::transparent);
	QPainter painter;
	painter.begin(&pixmap);
	painter.setRenderHint(QPainter::Antialiasing, true);
	if (border) {
		QPen pen(strColor);
		pen.setStyle(Qt::SolidLine);
		pen.setWidth(w);
		painter.setPen(pen);
	} else {
		painter.setPen(Qt::NoPen);
	}
	painter.setBrush(QColor(color));
	painter.drawRect(0, 0, width(),height());
	painter.end();
	m_nRadius = radius;
}

void TranslucenWidget::SetNinePixmap(const QString &picName, int iHorzSplit,
				     int iVertSplit, int DstWidth,
				     int DstHeight)
{
	QPixmap *pix = NinePatch(picName, iHorzSplit, iVertSplit, DstWidth, DstHeight);
	if (pix && !pix->isNull()) {
		pixmap = *pix;
		resize(pixmap.size());
	}
}

QPixmap *TranslucenWidget::NinePatch(const QString &picName, int iHorzSplit,
				     int iVertSplit, int DstWidth,
				     int DstHeight)
{
	QPixmap* pix = new QPixmap(picName);
	int pixWidth = pix->width();
	int pixHeight = pix->height();
 
	QPixmap pix_1 = pix->copy(0, 0, iHorzSplit, iVertSplit);
	QPixmap pix_2 = pix->copy(iHorzSplit, 0, pixWidth-iHorzSplit*2, iVertSplit);
	QPixmap pix_3 = pix->copy(pixWidth-iHorzSplit, 0, iHorzSplit, iVertSplit);
 
	QPixmap pix_4 = pix->copy(0, iVertSplit, iHorzSplit, pixHeight-iVertSplit*2);
	QPixmap pix_5 = pix->copy(iHorzSplit, iVertSplit, pixWidth-iHorzSplit*2, pixHeight-iVertSplit*2);
	QPixmap pix_6 = pix->copy(pixWidth-iHorzSplit, iVertSplit, iHorzSplit, pixHeight-iVertSplit*2);
 
	QPixmap pix_7 = pix->copy(0, pixHeight-iVertSplit, iHorzSplit, iVertSplit);
	QPixmap pix_8 = pix->copy(iHorzSplit, pixHeight-iVertSplit, pixWidth-iHorzSplit*2, pixWidth - iHorzSplit*2);
	QPixmap pix_9 = pix->copy(pixWidth-iHorzSplit, pixHeight-iVertSplit, iHorzSplit, iVertSplit);
 
	pix_2 = pix_2.scaled(DstWidth-iHorzSplit*2,iVertSplit, Qt::IgnoreAspectRatio);
	pix_4 = pix_4.scaled(iHorzSplit, DstHeight-iVertSplit*2, Qt::IgnoreAspectRatio);
	pix_5 = pix_5.scaled(DstWidth-iHorzSplit*2,DstHeight-iVertSplit*2, Qt::IgnoreAspectRatio);
	pix_6 = pix_6.scaled(iHorzSplit, DstHeight-iVertSplit*2, Qt::IgnoreAspectRatio);
	pix_8 = pix_8.scaled(DstWidth-iHorzSplit*2, iVertSplit);
 
	QPixmap* resultImg =new QPixmap(DstWidth, DstHeight);
	resultImg->fill(Qt::transparent);
	QPainter* painter = new QPainter(resultImg);
	if (!resultImg->isNull()) {
		painter->drawPixmap(0,0,pix_1);
		painter->drawPixmap(iHorzSplit, 0, pix_2);
		painter->drawPixmap(DstWidth-iHorzSplit,0,pix_3);
 
		painter->drawPixmap(0, iVertSplit, pix_4);
		painter->drawPixmap(iHorzSplit, iVertSplit, pix_5);
		painter->drawPixmap(DstWidth-iHorzSplit, iVertSplit, pix_6);
 
		painter->drawPixmap(0, DstHeight-iVertSplit, pix_7);
		painter->drawPixmap(iHorzSplit, DstHeight-iVertSplit, pix_8);
		painter->drawPixmap(DstWidth-iHorzSplit, DstHeight-iVertSplit, pix_9);
		painter->end();
	}
	return resultImg;
}

void TranslucenWidget::paintEvent(QPaintEvent *event)
{
	if (!pixmap.isNull()) {
		QPainter painter(this);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.drawPixmap(0, 0, pixmap);
		if (m_nRadius > 0) {
			QSize size = this->size();
			QBitmap bmp(this->size());
			bmp.fill();
			QPainter p(&bmp);
			p.setPen(Qt::NoPen);
			p.setBrush(Qt::black);
			p.drawRoundedRect(bmp.rect(), m_nRadius, m_nRadius);
			setMask(bmp);
		}
	}
	QDialog::paintEvent(event);
}
