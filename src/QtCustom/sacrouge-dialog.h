#pragma once

#include <QDialog>
#include <QtNetwork/QNetworkReply>
#include "qtcustom_global.h"
#include "move-dialog.h"
#include "base-define.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QNetworkReply;
class CallbackThread;

class QTCUSTOM_EXPORT SacrougeDialog : public MoveDialog {
	Q_OBJECT
public:
	explicit SacrougeDialog(const STBeginRsp &stBeginRsp, const std::string &strToken,
				QWidget *parent = nullptr);
	virtual ~SacrougeDialog() override;

private:
	void GetAccount();
	void GeAccountCallback(const QByteArray &bytes, const QNetworkReply::NetworkError &error);

public slots:
	void OnMontantChanged(const QString &text);
	void OnCountFinish();
	void OnHandOutRedenvel();

private:
	QLineEdit *m_pMontantEdit;
	QLineEdit *m_pCountEdit;
	QLabel *m_pTicketCount;

	bool m_bHandOut;
	std::string m_strToken;
	STBeginRsp m_stBeginRsp;

	CallbackThread *m_pGetAccountThread;
};

class QTCUSTOM_EXPORT RedDetailsItem : public QWidget {
	Q_OBJECT
public:
	explicit RedDetailsItem(STPersonRedDetails stPerson, QWidget *parent = nullptr);
	virtual ~RedDetailsItem() override;
};

class QTCUSTOM_EXPORT RedDetailsDialog : public MoveDialog {
	Q_OBJECT
public:
	explicit RedDetailsDialog(int redID, const std::string &strToken, QWidget *parent = nullptr);
	virtual ~RedDetailsDialog() override;

private:
	void GetDetails(int page);
	void GeDetailsCallback(const QByteArray &bytes, const QNetworkReply::NetworkError &error);
	void AddItem(STPersonRedDetails stPerson);

private:
	QLabel *m_pAmount;
	QLabel *m_pCount;
	QLabel *m_pTotal;
	QListWidget *m_pListWidget;

	int m_nID;
	std::string m_strToken;

	CallbackThread *m_pRedDetailsThread;
};
