#ifndef MOVE_DIALOG_H
#define MOVE_DIALOG_H
#include "qtcustom_global.h"
#include "translucent-widget.h"


class QTCUSTOM_EXPORT MoveDialog : public TranslucenWidget {
	Q_OBJECT
public:
	MoveDialog(QWidget *parent);
	~MoveDialog();
	void SetMoveEnabled(bool enable);
protected:
	virtual void paintEvent(QPaintEvent *event);
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;

private:
	QPoint m_pressPoint;
	QPoint m_movePoint;
	bool m_pressed;
	bool m_bEnabled;
};

#endif
