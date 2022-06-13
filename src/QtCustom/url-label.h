#ifndef URL_LABEL_H
#define URL_LABEL_H
#include<QLabel>
#include "base-define.h"
#include "network/base-request.h"
#include "qtcustom_global.h"
#include <QMouseEvent>

class QTimer;
class QTCUSTOM_EXPORT QUrlLabel : public QLabel {
	Q_OBJECT
public:
	QUrlLabel(QWidget *parent = 0, bool round = false);
	~QUrlLabel();
	void SetTask(const RequestTask& task);
	bool Update();
	void SetNetPath(const QString & url);
	QString &GetMd5NetPath();
public slots:
	void OnRequestFinished(bool, const QByteArray &, const QString &);
	void OnTimeOut();
protected:
	void paintEvent(QPaintEvent *event);
private:
	std::unique_ptr<NetRequest> m_pRequest;
	RequestTask m_task;
	QString m_strFile;
	bool m_bRound;
	QTimer *check;
};

class QTCUSTOM_EXPORT UrlRadiusLabel : public QUrlLabel {
	Q_OBJECT
public:
	UrlRadiusLabel(int type, QWidget *parent = nullptr);
	~UrlRadiusLabel();
	void SetSelect(bool select);
	int GetType();

signals:
	void click(UrlRadiusLabel *label);

private:
	QPixmap Round(const QPixmap* pixmap, qreal radius);

protected:
	virtual void enterEvent(QEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
	bool m_bHover;
	QPixmap m_radPixmap;
	bool m_bSelect;
	int m_ntype;
};

class QTCUSTOM_EXPORT NoneStateLabel : public QLabel {
	Q_OBJECT
public:
	NoneStateLabel(QWidget *parent = nullptr);
	~NoneStateLabel();
	void SetSelect(bool bSelect);

signals:
	void click(NoneStateLabel *label);

protected:
	virtual void enterEvent(QEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
	bool m_bHover;
	bool m_bSelect;
};

class QTCUSTOM_EXPORT MaskFilterLabel : public QLabel {
	Q_OBJECT
public:
	MaskFilterLabel(QWidget *parent = nullptr);
	~MaskFilterLabel();
	void SetSelect(bool bSelect);
	void SetData(Mask_Type type);
	Mask_Type GetData();

signals:
	void click(MaskFilterLabel *label);

protected:
	virtual void enterEvent(QEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *ev) override;

private:
	bool m_bHover;
	bool m_bSelect;
	Mask_Type m_type;
};
#endif // !URL_LABEL_H
