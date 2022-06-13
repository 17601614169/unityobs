#ifndef RecordSaveTip_h
#define RecordSaveTip_h

#include <QWidget>
#include "translucent-widget.h"
#include "qtcustom_global.h"
#include <QTimer>
#include <QScopedPointer>
#include <memory>
class QTCUSTOM_EXPORT RecordSaveTip : public TranslucenWidget {
	Q_OBJECT
public:
	enum BtnType {
		TYPE_VIEW	= 0,
		TYPE_SET	= 1
	};
	RecordSaveTip(QWidget *parent = Q_NULLPTR);
	~RecordSaveTip();
	void ResetPos(const QPoint &pos, int w, int h);
	void ShowWithLimitTime();
signals:
	void btnSignal(BtnType type);

private:
	QLabel *m_pTip;
	QPushButton *m_pViewDetail;
	QPushButton *m_pSet;
	std::unique_ptr<QTimer> m_pDiplayTimer;
};

#endif
