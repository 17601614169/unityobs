#pragma once

#include "qtcustom_global.h"
#include "move-dialog.h"
#include "base-define.h"

#include <QPainter>
#include <QProxyStyle>
#include <QPushButton>
#include <QVariant>

class QSlider;
class QLineEdit;
class QListWidget;
class QListWidgetItem;

class TabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const;
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
};

class SliderEdit : public QWidget {
	Q_OBJECT
public:
	SliderEdit(const QString &name, QWidget *parent = nullptr, int value = 0);
	~SliderEdit();

	void SetData(QVariant variant);
	QVariant GetData();

signals:
	void valueChange(int value);

public slots:
	void SetValue(int value);

private:
	QSlider *m_pSlider;
	QLineEdit *m_pLineEdit;

	QVariant m_variant;
};

class BeautyFaceWidget : public QWidget {
	Q_OBJECT
public:
	BeautyFaceWidget(QMap<BeautyFace_Type, int> &beautyData, QWidget *parent = nullptr);
	~BeautyFaceWidget();

signals:
	void ChangeBeautyFace(BeautyFace_Type type, int value);

public slots:
	void OnValueChange(int value);
	void Reset();
	
protected:
	virtual void paintEvent(QPaintEvent *event) override;

private:
	SliderEdit *m_pSe1;
	SliderEdit *m_pSe2;
	SliderEdit *m_pSe3;
	SliderEdit *m_pSe4;
	SliderEdit *m_pSe5;
	SliderEdit *m_pSe6;
	SliderEdit *m_pSe7;
};

class FilterButton : public QPushButton {
	Q_OBJECT
public:
	FilterButton(const QString &path, QWidget *parent = nullptr);
	~FilterButton();

	void SetSelect(bool select);

private:
	QLabel *m_pCheckLabel;
};

class FilterItem : public QWidget {
	Q_OBJECT
public:
	FilterItem(const QString &name, const QString &path, QWidget *parent = nullptr);
	~FilterItem();

	void SetSelect(bool bSelect);
	void SetData(QVariant variant);
	QVariant GetData();

signals:
	void clicked(FilterItem *item);

private:
	FilterButton *m_pFilterButton;
	QVariant m_varData;
};

class FilterWidget : public QWidget {
	Q_OBJECT
public:
	FilterWidget(Filter_Type type, QWidget *parent = nullptr);
	~FilterWidget();

private:
	void GetData(Filter_Type type, QString &name, QString &path);
	void AddItem(int i);

signals:
	void FilterChange(int type);
	void Confirm();

public slots:
	void Reset();
	void OnItemClicked(FilterItem *item);

protected:
	virtual void paintEvent(QPaintEvent *event) override;

private:
	QListWidget *m_pListWidget;
	Filter_Type m_type;
};

class QTCUSTOM_EXPORT LivePartner : public MoveDialog {
	Q_OBJECT
public:
	explicit LivePartner(Filter_Type type, QMap<BeautyFace_Type, int> &beautyData, QWidget *parent = nullptr);
	virtual ~LivePartner();

public slots:
	void OnCloseClick();

private:
	FilterWidget *m_pFilterWidget;
	Filter_Type m_type;
};
