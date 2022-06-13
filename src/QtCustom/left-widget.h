#ifndef LEFTWIDGET_H
#define LEFTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPushButton>

#include "qtcustom_global.h"
#include "base-define.h"
#include "common.h"
#include "base-widget.h"

#include <obs.h>
#include "obs.hpp"

#include <functional>

class QListWidget;
class QLabel;
class QCheckBox;
class ScreenShotDialog;

bool QTCUSTOM_EXPORT EnumItem(obs_scene_t *, obs_sceneitem_t *item, void *ptr);
using GetSceneCallback = std::function<OBSScene()>;

class QTCUSTOM_EXPORT LeftWidget : public BaseWidget {
	Q_OBJECT
public:
	explicit LeftWidget(const GetSceneCallback &getScene, QWidget *parent = nullptr);
	virtual ~LeftWidget();

	void ReorderItems(QVector<OBSSceneItem> &items);
	void SelectItem(OBSSceneItem sceneitem, bool select);

signals:	
	void MoveSourceItem(void *scene, int move);
	void SelectSourceItem(void *currentScene, void *previousScene);

public slots:
	void AddSourceItem(OBSSceneItem sceneitem, bool bFirst = false);
	void DeleteSourceItem(OBSSceneItem sceneitem);

	void OnClickAdd();

private:
	bool ChangeItemPos(int index, int step);

private:
	OBSSceneItem previousItem;
	QListWidget *m_pSourceList;
	GetSceneCallback m_getScene;
};

class QTCUSTOM_EXPORT SourceItem : public QWidget {
	Q_OBJECT
public:
	explicit SourceItem(OBSSceneItem sceneitem, QWidget *parent = nullptr);
	virtual ~SourceItem();

	void Init();
	void SetSelect(bool select);
	OBSSceneItem GetSceneItem();
	void ElidedText();
protected:
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void enterEvent(QEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;

signals:
	void EditClicked(void *scene);
	void DeleteClicked(void *scene);

private:
	void SetHover(bool hover);

private:
	OBSSceneItem m_obsSceneItem;
	bool m_bHover;
	bool m_bSelect;

	QLabel *m_pText;
	QPushButton *m_pEditButton;
	QPushButton *m_pDeleteButton;
	QCheckBox *m_pEnableCheckBox;
	QString m_strName;
	ScreenShotDialog *m_pScreenShotDialog;
};

#endif // !LEFTWIDGET_H
