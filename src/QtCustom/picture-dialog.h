#pragma once

#include "source-base-dialog.h"

class QLineEdit;

class TailorWidget : public QWidget {
	Q_OBJECT
public:
	enum Direction {
		None = 0,
		Left,
		Top,
		Right,
		Bottom,
		LeftTop,
		RightTop,
		RightBottom,
		LeftBottom,
		Center,
	};

	explicit TailorWidget(QWidget *parent = nullptr);
	explicit TailorWidget(const QString &path, QWidget *parent = nullptr);
	virtual ~TailorWidget() override;

	void SetPath(const QString &path);
	//void GetCrop(int &left, int &top, int &right, int &bottom);

protected:
	virtual void paintEvent(QPaintEvent *event) override;
	//virtual void mousePressEvent(QMouseEvent *event);
	//virtual void mouseReleaseEvent(QMouseEvent *event);
	//virtual void mouseMoveEvent(QMouseEvent *event);

private:
	//Direction GetRegion(QPoint pt);
	//void OnMouseChange(int x, int y);
	//void OnMove(QPoint pt);

private:
	QPixmap m_pixmap;

	double m_dRatio;
	bool m_bLeftPress;
	QPoint m_pressPt;
	QPoint m_originPt;
	QRect m_rcTailor;
	QRect m_rcLimit;
	bool m_bFirstPicture;
	Direction m_curDirection;
	QString m_strPath;
};

class QTCUSTOM_EXPORT PictureDialog : public SourceBaseDialog {
	Q_OBJECT
public:
	explicit PictureDialog(OBSScene scene, const char *id, QWidget *parent = nullptr);
	explicit PictureDialog(OBSScene scene, obs_source_t *source, QWidget *parent = nullptr);
	virtual ~PictureDialog() override;
	
	void InitSource();

protected:
	virtual void InitContentLayout() override;
	virtual void SubInterface() override;
	virtual bool BeforeConfirm() override;

private:
	QString m_strPicturePath;
	QLineEdit *m_pLineEdit;
	TailorWidget *m_pTailorWidget;
};

