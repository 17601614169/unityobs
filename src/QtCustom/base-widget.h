#pragma once

#include <QWidget>

#include "qtcustom_global.h"


class QTCUSTOM_EXPORT BaseWidget : public QWidget {
	Q_OBJECT
public:
	explicit BaseWidget(QWidget *parent = nullptr);
	virtual ~BaseWidget();

protected:
	virtual void paintEvent(QPaintEvent *event) override;
};
