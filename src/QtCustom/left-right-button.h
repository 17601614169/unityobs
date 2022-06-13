#pragma once

#include <QPushButton>

#include "qtcustom_global.h"

class QHBoxLayout;
class QLabel;
class QMargins;

class QTCUSTOM_EXPORT LeftRightButton : public QPushButton {
	Q_OBJECT
public:
	explicit LeftRightButton(const QString &strImage, const QString &text,
				 QWidget *parent = nullptr);
	virtual ~LeftRightButton() override;

	void SetMargins(QMargins margins);
	void SetSpace(int space);
	void SetTextQss(const QString &qss);
	void SetLabelText(const QString & text);
private:
	QHBoxLayout *m_pLayout;
	QLabel *m_pImage;
	QLabel *m_pText;

	QMargins m_margins;
	int m_space;
	QString m_textQss;
};
