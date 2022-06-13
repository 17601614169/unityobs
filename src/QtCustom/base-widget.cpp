#include "base-widget.h"

#include <QPainter>
#include <QStyleOption>

BaseWidget::BaseWidget(QWidget *parent) : QWidget(parent) {}

BaseWidget::~BaseWidget() {}

void BaseWidget::paintEvent(QPaintEvent *event) {
	QStyleOption option;
	option.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);
}
