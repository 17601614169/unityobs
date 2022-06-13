#ifndef SOURCEWIDGET_H
#define SOURCEWIDGET_H

#include <QWidget>
#include <QPushButton>

#include "qtcustom_global.h"
#include "base-define.h"
#include "common.h"

class QLabel;

class QTCUSTOM_EXPORT AddSourceButton : public QPushButton {
	Q_OBJECT
public:
	explicit AddSourceButton(QWidget *parent = nullptr);
	virtual ~AddSourceButton();

public:
	void SetButtonType(SourceType type);

signals:
	void AddSourceClicked(QString str);

private:
	SourceType m_type;

	QLabel *m_pIcon;
	QLabel *m_pText;
};

class QTCUSTOM_EXPORT AddSourceWidget : public QWidget {
	Q_OBJECT
public:
	explicit AddSourceWidget(QWidget *parent = nullptr);
	virtual ~AddSourceWidget();

protected:
	virtual void paintEvent(QPaintEvent *event) override;
};

#endif // !SOURCEWIDGET_H
