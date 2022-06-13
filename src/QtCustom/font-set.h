/**
* 文字样式设置
* family,size,文字颜色，背景颜色，粗体，斜体，下划线
*/
#ifndef FONTSET_H
#define FONTSET_H
#include "move-dialog.h"
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QTimer>
#include <qbuttongroup.h>
#include "qtcustom_global.h"
#include "obs.h"
#include <QMap>
#include <QListWidget>
class QTCUSTOM_EXPORT ColorControl : public QPushButton {
	Q_OBJECT
public:
	ColorControl(QWidget* parent=nullptr);
	~ColorControl();
	void SetStyle(const QString & icon,const QString & arrow);
public slots:
	void SetColor(const QString & color);
private:
	QLabel* m_pIcon;
	QLabel* m_pArrow;
	QLabel* m_pColor;
};

class ColorItem : public QPushButton {
	Q_OBJECT
public:
	explicit ColorItem(QWidget* parent=nullptr);
	~ColorItem();
	void SetColor(const QString & color);

protected:
	void enterEvent(QEvent *event);
	void leaveEvent(QEvent *event);
private:
	QLabel* m_pColor;
	QLabel* m_pEnterColor;
	
};
struct STTxtSet {
	char name[32];
	QString text;
	QString family;
	QString font_size;
	QString txt_color;
	QString bg_color;
	bool bold;
	bool italic;
	bool underline;
	STTxtSet() {
		memset(name,0,_countof(name));
		bold = false;
		italic = false;
		underline = false;
	}
};
class QTCUSTOM_EXPORT ColorDialog : public MoveDialog {
	Q_OBJECT
public:
	ColorDialog(QWidget* parent=nullptr);
	~ColorDialog();
	void SetText(const QString & text);
	void SetInitColor(const QString & color);
Q_SIGNALS:
	void colorSelect(const QString & color);
public slots:
	void OnButtonClicked(int );
private:
	QButtonGroup* m_pGroup;
	QLabel *m_pTxt;
	QMap<int,QString> m_mapId2Color;
};

class QTCUSTOM_EXPORT FontSet : public MoveDialog {
	Q_OBJECT
public:
	struct STText {
		QString text;
	};
	FontSet(obs_source_t * source = nullptr,QWidget* parent=nullptr);
	~FontSet();

	void ResetColorDialogPos() ;
	void HideColorDialog() ;
	void InitSource(obs_source_t * source);
Q_SIGNALS:
	void addTxtSignal(const STTxtSet & set);
public slots:
	void OnAddClicked() ;
	void OnTextChanged(const QString&);
	void OnCurrentFamilyIndexChanged(int index);

	void OnSizeTetxChanged(const QString&);
	void OnCurrentSizeIndexChanged(int index);

	void OnColorSelect(const QString & color) ;
	void OnBkColorSelect(const QString & color);
	void OnSetChanged();
	void OnTextChanged() ;
protected:
	bool eventFilter(QObject* obj,QEvent* event);
private:
	QLabel* m_pTitle;
	QPushButton* m_pClose;
	QLineEdit* m_pFamilyText ;
	QLineEdit* m_pSizeText ;
	QComboBox* m_pFamilyCombo;

	QListWidget *m_pFamilyList;
	
	QComboBox* m_pSizeCombo;
	QListWidget *m_pSizeList;
	ColorControl* m_pTxtColor;
	bool m_bEnterColor;
	ColorDialog* m_pColorDialog;
	bool m_bEnterBgColor;
	ColorControl* m_pTxtBgColor;
	ColorDialog* m_pBgDialog;
	QPushButton* m_pBold;
	QPushButton* m_pItalic;
	QPushButton* m_pUnderLine;
	QTextEdit* m_pTxtEdit;
	QLabel* m_pTxtCount;
	QLabel* m_nCountTips;
	QPushButton* m_pOk;
	QPushButton* m_pCancle;
	bool m_bBold;
	bool m_bItalic;
	bool m_bUnderline;
	QTimer *m_pPosTimer;
	STTxtSet m_stTxtSet;
	obs_source_t * m_pSource;
	QMap<int,QString> m_mapIndex2Family;
	QMap<int,QString> m_mapIndex2Size;
	bool m_bInit;
};
#endif
