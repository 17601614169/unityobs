#include "custom-slider.h"
#ifndef FADER_PRECISION
#define FADER_PRECISION 4096.0
#endif // !
#include <QBitmap>
#include "custom-stylesheet.h"

CustomSlider::CustomSlider(QWidget *parent)
	: TranslucenWidget(parent), m_pSlider(nullptr)
{
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint
		| Qt::Tool|Qt::WindowStaysOnTopHint|Qt::Popup);
	setAttribute(Qt::WA_TranslucentBackground);
	resize(49,159);
	QImage img;
	img.load(":/res/images/mic_bg.png);background:transparent;");
	SetPixmap(":/res/images/mic_bg.png");
	m_pSlider = new QSlider(this);
	m_pSlider->setGeometry(18,14,width()-36,height()-28);
	m_pSlider->setOrientation(Qt::Vertical);
	m_pSlider->setMinimum(0);
	m_pSlider->setMaximum(int(FADER_PRECISION));

	m_pSlider->setStyleSheet(CUSTOM_SLIDER);
}

CustomSlider::~CustomSlider() {

}

QSlider* CustomSlider::GetSlider()
{
	return m_pSlider;
}
