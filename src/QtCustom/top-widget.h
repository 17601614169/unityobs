#ifndef TOPWIDGET_H
#define TOPWIDGET_H

#include <QLabel>
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include <QTimer>

#include "qtcustom_global.h"
#include "base-widget.h"
#include "base-define.h"
#include "asyn-request.h"

#include <memory>
#include <functional>

class QUrlLabel;

enum TitleButtonType {
	TYPE_MIN = 0,
	TYPE_MAX,
	TYPE_RESTORE,
	TYPE_CLOSE,
	TYPE_MESSAGE
};

class MessageButton : public QPushButton {
	Q_OBJECT
public:
	explicit MessageButton(QWidget *parent = nullptr);
	virtual ~MessageButton() override;
	
	void SetNoteCount(int count);
private:
	QLabel *m_pCount;
};

Q_DECLARE_METATYPE(STCsUser);
class QTCUSTOM_EXPORT TopWidget : public BaseWidget {
	Q_OBJECT
public:
	TopWidget(STLoginRsp rsp, QWidget *parent = nullptr);
	~TopWidget();

	void SetMax(bool bMax);
	void SetMoveCallback(const std::function<void(QPoint)> &fun);
	QPoint GetSetPosition();

protected:
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
signals:
	void ButtonGroupSignals(int);
	void CloseWindow();
	void UpdateLoginInfo(STCsUser csuser);
	void ExitWindow();

public slots:
	void OnButtonGroupSlot(int type);
	void OnTimeout();
	void OnReadyData(const STRspData &stRsp);
	void OnOperateClick();
	void OnChangePassword();
	void OnEssentialInfo();
	void OnExit();

private:
	bool m_bLeftButtonPressed;
	QPoint m_startPt;

	std::function<void(QPoint)> m_func;
	
	QUrlLabel *m_pHead;
	QLabel *m_pName;
	QLabel *m_pCompany;
	QPushButton* m_pDownBtn;

	QButtonGroup *m_pButtonGroup;
	MessageButton *m_pMessageBtn;
	QPushButton *m_pMinimumBtn;
	QPushButton *m_pMaximumBtn;
	QPushButton *m_pRestoreBtn;
	QPushButton *m_pCloseBtn;

	std::unique_ptr<QTimer> m_timerNote;
	std::string m_strToken;
	STUser m_stUser;
};

#endif // !TOPWIDGET_H
