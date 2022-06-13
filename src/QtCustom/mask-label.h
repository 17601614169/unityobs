#ifndef MASKLABEL_H
#define MASKLABEL_H

#include <QLabel>
class QNetworkReply;
class QNetworkAccessManager;
#include "qtcustom_global.h"

// 圆形label 支持网络路径

class QTCUSTOM_EXPORT MaskLabel : public QLabel {
	Q_OBJECT
public:
	MaskLabel(QWidget *parent = nullptr);
	~MaskLabel();

	void SetNetPath(const QString &path);

protected:
	virtual void paintEvent(QPaintEvent *event) override;
public slots:
	void OnReadyRead();
	void OnFinished();
private:
	QNetworkAccessManager* manager;
	QNetworkReply *reply;
};

#endif // !MASKLABEL_H
