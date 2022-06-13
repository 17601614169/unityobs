#ifndef CUSTOMSLIDER_H
#define CUSTOMSLIDER_H
#include <QSlider>
#include <QDialog>
#include "qtcustom_global.h"
#include "translucent-widget.h"

class QTCUSTOM_EXPORT CustomSlider : public TranslucenWidget {
	Q_OBJECT
public:
	CustomSlider(QWidget *parent);
	~CustomSlider();
	QSlider *GetSlider();
private:
	QSlider *m_pSlider;
};

#endif // !1
