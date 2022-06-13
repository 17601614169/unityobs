#include "move-dialog.h"
#include <QMouseEvent>

MoveDialog::MoveDialog(QWidget *parent) : TranslucenWidget(parent) {
	m_pressed = false;
	m_bEnabled = true;
}

MoveDialog::~MoveDialog() {

}

void MoveDialog::paintEvent(QPaintEvent* event) {
	TranslucenWidget::paintEvent(event);
}

void MoveDialog::mousePressEvent(QMouseEvent *event) {
	if (!m_bEnabled)
		return;
	if (event->button() == Qt::LeftButton) {
		m_pressPoint = event->globalPos();
		m_pressed = true;
	}
	//event->ignore();
	TranslucenWidget::mousePressEvent(event);
}

void MoveDialog::SetMoveEnabled(bool enable) {
	m_bEnabled = enable;
}

void MoveDialog::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		m_pressed = false;
	}
	//event->ignore();
	TranslucenWidget::mouseReleaseEvent(event);
}

void MoveDialog::mouseMoveEvent(QMouseEvent *event) {
	if (m_pressed) {
		m_movePoint = event->globalPos();
		this->move(this->pos() + m_movePoint - m_pressPoint);
		m_pressPoint = m_movePoint;
	}
	//event->ignore();
	TranslucenWidget::mouseMoveEvent(event);
}
