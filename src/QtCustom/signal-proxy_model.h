#ifndef SIGNAL_PROXY_MODEL_H
#define SIGNAL_PROXY_MODEL_H

#include <QObject>
#include <tuple>

#include "qtcustom_global.h"
#include "base-define.h"

class QTCUSTOM_EXPORT SignalProxyModel : public QObject {
	Q_OBJECT
public:
	~SignalProxyModel();
	SINGLETON_DEFINE(SignalProxyModel);

	/*
	*摄像头资源是否可用
	*/
	void SetVedioSource(const std::string &id, const std::string &name, bool enable);
	bool GetVedioSource(const std::string &id, const std::string &name, bool &enable);

signals:
	void AddSourceClicked(QString str);
	void MoveSourceItem(void *scene, int move);
	void SelectSourceItem(void *currentScene, void *previousScene);
	void EditClicked(void *scene);
	void DeleteClicked(void *scene);
	void FilterChange(int type);
	void ChangeBeautyFace(int type, int value);

private:
	SignalProxyModel();

private:
	std::tuple<std::string, std::string, bool> m_tupleVedioEnable;
};

#endif // !SIGNAL_PROXY_MODEL_H
