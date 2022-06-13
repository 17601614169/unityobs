#pragma once

#include <QVBoxLayout>
#include <QListWidget>
#include <QTimerEvent>

#include "qtcustom_global.h"
#include "move-dialog.h"
#include "obs-properties.h"
#include "obs.h"
#include <obs.hpp>

#include <memory>

class QPushButton;
class QComboBox;

class QTCUSTOM_EXPORT WindowCaptureItem : public QWidget {
	Q_OBJECT
public:
	explicit WindowCaptureItem(const QString &text, QWidget *parent = nullptr);
	virtual ~WindowCaptureItem();

	void SetSelect(bool select);

private:
	QPushButton *m_pSelectBtn;
};

class QTCUSTOM_EXPORT SourceBaseDialog : public MoveDialog {
	Q_OBJECT
	using properties_delete_t = decltype(&obs_properties_destroy);
	using properties_t =
		std::unique_ptr<obs_properties_t, properties_delete_t>;

public:
	explicit SourceBaseDialog(OBSScene scene, const char *id, const QString &title, QWidget *parent = nullptr);
	explicit SourceBaseDialog(OBSScene scene, obs_source_t *source, const QString &title, QWidget *parent = nullptr);
	virtual ~SourceBaseDialog() override;

protected:
	virtual void InitContentLayout() = 0;
	virtual void SubInterface() = 0;
	virtual bool BeforeConfirm() = 0;

private:
	void InitBaseLayout(const QString &strTitle);
	void CreateSource();
	void PropertiesReload();

public slots:
	void OnConfirmClick();

protected:
	OBSScene m_scene;
	const char *m_pID;
	obs_source_t *m_pSource;
	properties_t m_properties;
	OBSData m_settings;
	OBSData m_oldSettings;

	QVBoxLayout *m_pContentLayout;

	bool m_bUpdate;
};


class QTCUSTOM_EXPORT WindowCaptureDialog : public SourceBaseDialog {
	Q_OBJECT
public:
	explicit WindowCaptureDialog(OBSScene scene, const char *id, QWidget *parent = nullptr);
	explicit WindowCaptureDialog(OBSScene scene, obs_source_t *source, QWidget *parent = nullptr);
	virtual ~WindowCaptureDialog() override;

	void InitSource();
	void AddListItem(QListWidget *list, obs_property_t *prop, obs_combo_format format, size_t idx);

protected:
	virtual void InitContentLayout() override;
	virtual void SubInterface() override;
	virtual bool BeforeConfirm() override;

public slots:
	void OnCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
	QListWidget *m_pListWidget;
};

class QTCUSTOM_EXPORT CameraDialog : public SourceBaseDialog {
	Q_OBJECT
public:
	explicit CameraDialog(OBSScene scene, const char *id, QWidget *parent = nullptr);
	explicit CameraDialog(OBSScene scene, obs_source_t *source, QWidget *parent = nullptr);
	virtual ~CameraDialog() override;

	void InitSource();

protected:
	virtual void InitContentLayout() override;
	virtual void SubInterface() override;
	virtual bool BeforeConfirm() override;
	virtual void timerEvent(QTimerEvent *event) override;  

public slots:
	void OnCurrentIndexChanged(int index);

private:
	QComboBox *m_pComboBox;

	int m_nTimerId;
};

class QTCUSTOM_EXPORT ScreenShotDialog : public QDialog
{
	Q_OBJECT
public:
	// 方位枚举
	enum DIRECTION {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		LEFTTOP,
		LEFTBOTTOM,
		RIGHTTOP,
		RIGHTBOTTOM,
		NONE
	};
	explicit ScreenShotDialog(QPoint beginPt, QPoint endPt, obs_sceneitem_t *item, QWidget *parent = nullptr);
	virtual ~ScreenShotDialog() override;
	void Show();

private:
	DIRECTION GetRegion(const QPoint &cursor);
	void OnMouseChange(int x, int y);
	void GetCrop(int &left, int &top, int &right, int &bottom);

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void moveEvent(QMoveEvent *event);

signals:
	void DeleteClicked(void *);

private:
	DIRECTION m_curDirection;
	bool m_bPressed;
	QPoint m_pressedPt;
	QPoint m_movePt;
	QPoint m_originPt;
	QPoint m_beginPt;
	QPoint m_endPt;
	QPoint m_reallyBeginPt;
	QPoint m_reallyEndPt;
	obs_sceneitem_t *m_pSceneItem;

	QPushButton *m_pCloseBtn;
};
