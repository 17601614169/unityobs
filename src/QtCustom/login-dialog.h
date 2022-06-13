#pragma once

#include <QDialog>
#include "qtcustom_global.h"
#include "move-dialog.h"
#include "asyn-request.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include<QTimer>
#define LOGIN_CLICK 0
#define LOGIN_CLOSE_CLICK 1
class QTCUSTOM_EXPORT LoginDialog : public MoveDialog {
	Q_OBJECT

public:
	LoginDialog(QWidget *parent = Q_NULLPTR);
	~LoginDialog();

	void WhiteDetail();
	void WhiteAuthCheck(const QString &shopCode, const QString &userName, const QString &password);
	void SetApplyStatus(int status);
	void SetOperate(bool operate);
	bool IsOperate();
signals:
	void loginSignal(int, QByteArray& bytes);
public slots:
	void OnLoginBtnClicked();
	void OnReadyData(const STRspData &stRsp);
	void OnReqAuthClicked();
	void OnTimeOut();
protected:
	virtual void keyPressEvent(QKeyEvent *event) override;
private:
	QPushButton *m_pClose;
	QLabel *m_pIcon;
	QLabel *m_pAppName;

	//商户编码
	QLabel *m_pShopCodeIcon;
	QLineEdit *m_pShopCode;
	QLabel *m_pLine;

	QLabel *m_pUserNameIcon;
	QLineEdit *m_pUserName;
	QLabel *m_pLine1;

	QLabel *m_pPasswordIcon;
	QLineEdit *m_pPassword;
	QPushButton* m_pEye;

	QLabel *m_pLine2;
	QPushButton *m_pLoginBtn;

	QLabel* m_pUniqueCode;
	QPushButton* m_pReqAuth;

	QLabel* m_pAuthIcon;
	QLabel* m_pAuthTxt;

	QLabel* m_pUnAuthIcon;
	QLabel* m_pUnAuthTxt;

	QString m_strName;
	QString m_strPassword;
	std::string m_strMacMd5;
	QTimer* m_pCheckTimer;
	bool m_bOperate;
	bool m_bSuccess;
};
