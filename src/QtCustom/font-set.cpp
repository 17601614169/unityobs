#include "font-set.h"
#include "custom-stylesheet.h"
#include "common.h"
#include "characters-convert.h"
#include <qfontdatabase.h>
#include <QDebug>
#include <QListWidget>
#include <QScrollBar>
#include <QEvent>
#include <QTextCursor>
#include <QMouseEvent>
#include <qmetatype.h>
#include "msgbox.h"
#include "string-convert.h"
Q_DECLARE_METATYPE(FontSet::STText);
ColorControl::ColorControl(QWidget* parent)
	:QPushButton(parent)
	,m_pIcon(nullptr)
	,m_pArrow(nullptr)
	,m_pColor(nullptr)
{
	setStyleSheet("QPushButton{background: #FFFFFF;border-radius: 4px;border: 1px solid #E1DEE6;}"
		"QPushButton::hover{background: #FFFFFF;border-radius: 4px;border: 1px solid #A59CB2;}"
		"QPushButton::pressed{background: #FFFFFF;border-radius: 4px;border: 1px solid #A59CB2;}");
	setFixedSize(45,28);
	m_pIcon=new QLabel(this);
	m_pIcon->setGeometry(7,5,16,16);
	m_pArrow=new QLabel(this);
	m_pArrow->setGeometry(30,10,9,6);
	m_pColor=new QLabel(this);
	m_pColor->setGeometry(9,21,12,2);
}

ColorControl::~ColorControl() {

}

void ColorControl::SetStyle(const QString & icon,const QString & arrow) {
	m_pIcon->setStyleSheet(icon);
	m_pArrow->setStyleSheet(arrow);
}

void ColorControl::SetColor(const QString& color) {
	QString strColor = QString("background:%1").arg(color);
	m_pColor->setStyleSheet(strColor);
}

ColorItem::ColorItem(QWidget* parent)
	:QPushButton(parent)
{
	setCheckable(true);
	setFixedSize(22,22);
	m_pColor = new QLabel(this);
	m_pColor->setGeometry(3,3,16,16);

	m_pEnterColor = new QLabel(this);
	m_pEnterColor->setGeometry(1,1,20,20);
	m_pEnterColor->hide();
}

ColorItem::~ColorItem() {

}

void ColorItem::SetColor(const QString& color) {
	QString strStyle = QString("border:none;background-color:%1").arg(color);
	m_pColor->setStyleSheet(strStyle);
	m_pEnterColor->setStyleSheet(strStyle);
}

void ColorItem::enterEvent(QEvent* event) {
	m_pEnterColor->show();
}
void ColorItem::leaveEvent(QEvent* event) {
	m_pEnterColor->hide();
}

ColorDialog::ColorDialog(QWidget* parent)
	: MoveDialog(parent)
{
	m_pTxt = new QLabel(tr("txt color"),this);
	m_pTxt->setGeometry(10,4,48,26);
	m_pTxt->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	m_pTxt->setStyleSheet("font-size: 12px;"
		"font-family:Microsoft YaHei;"
		"font-weight: 400;color: #383257;line-height: 26px;");
	setFixedSize(190,180);
	SetColor("white",0,true,2,"#888888");
	SetMoveEnabled(false);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint
		| Qt::Tool|Qt::WindowStaysOnTopHint|Qt::Popup);
	QStringList list;
	list.push_back("#FFFFFF");
	list.push_back("#000000");
	list.push_back("#96FF00");
	list.push_back("#2CB2FF");
	list.push_back("#8064A2");
	list.push_back("#FFC000");
	list.push_back("#FF55A1");
	list.push_back("rgb(223,66,86)");

	list.push_back("#F2F2F2");
	list.push_back("#808080");
	list.push_back("#E5FFC1");
	list.push_back("#CEE8FF");
	list.push_back("#E6E0EC");
	list.push_back("#FFF2CC");
	list.push_back("#FFD5E8");
	list.push_back("#FFDBDB");

	list.push_back("#D9D9D9");
	list.push_back("#595959");
	list.push_back("#CAF194");
	list.push_back("#96C2E9");
	list.push_back("#CCC1DA");
	list.push_back("#FFE699");
	list.push_back("#F4A4C8");
	list.push_back("#FFACAC");

	list.push_back("#BFBFBF");
	list.push_back("#404040");
	list.push_back("#ADE066");
	list.push_back("#619CD0");
	list.push_back("#B3A2C7");
	list.push_back("#FFD24B");
	list.push_back("#EC6BA5");
	list.push_back("#F25C5C");

	list.push_back("#A6A6A6");
	list.push_back("#1E1E1E");
	list.push_back("#8ECB39");
	list.push_back("#2970AF");
	list.push_back("#604A7B");
	list.push_back("#DBA500");
	list.push_back("#D4367D");
	list.push_back("#CC2424");

	list.push_back("#7F7F7F");
	list.push_back("#0D0D0D");
	list.push_back("#66A113");
	list.push_back("#0B508E");
	list.push_back("#403152");
	list.push_back("#7F6000");
	list.push_back("#A3004A");
	list.push_back("#9B0000");
	QString srBtn = QString(
		"QPushButton{background:transparent;border:none;}"
		"QPushButton:hover{background:transparent;border:none;}"
		"QPushButton:pressed{background:transparent;border:none;}"
		"QPushButton:checked{background:transparent;border:1px solid #5F5B73;border-radius:2px;}"
	);
	m_pGroup = new QButtonGroup(this);
	int xpos = 0;
	int ypos = 0;
	for (int i = 0; i < list.size(); i++) {
		xpos = i%8*22 + 7;
		ypos = i/8*22 + 30;
		ColorItem *item = new ColorItem(this);
		item->SetColor(list[i]);
		item->setStyleSheet(srBtn);
		item->setGeometry(xpos,ypos,22,22);
		m_pGroup->addButton(item,i);
		m_mapId2Color[i]=list[i];
	}
	connect(m_pGroup,SIGNAL(buttonClicked(int )),this,SLOT(OnButtonClicked(int )));
}

void ColorDialog::SetText(const QString& text) {
	m_pTxt->setText(text);
}

void ColorDialog::SetInitColor(const QString& color) {
	QMap<int,QString>::iterator iter =  m_mapId2Color.begin();
	while (iter!=m_mapId2Color.end()) {
		QAbstractButton * btn =m_pGroup->button(iter.key());
		if (btn != nullptr) {
			if (iter.value().compare(color) == 0) {
				btn->setChecked(true);
			} else {
				btn->setChecked(false);
			}
		}
		iter++;
	}
}

void ColorDialog::OnButtonClicked(int id){
	QString strTxt = m_mapId2Color[id];
	qDebug() << "strTxt:"<<strTxt;
	emit colorSelect(strTxt);
}

ColorDialog::~ColorDialog() {

}

const QString ComboStyle = QString(
	"QComboBox{padding-left: 7px;background-color:white;"
		"border-radius: 4px;border: 1px solid #E1DEE6;font-weight:400;"
		"font-size: 12px;font-family:Microsoft YaHei;" 
		"font-weight: 400;color: #383257;line-height: 16px}"
	"QComboBox:hover{padding-left: 7px;background-color:white;"
		"border-radius: 4px;border: 1px solid #A59CB2;font-weight:400;"
		"font-size: 12px;font-family:Microsoft YaHei;" 
		"font-weight: 400;color: #383257;line-height: 16px}"
	"QComboBox:focus{padding-left: 7px;background-color:white;"
		"border-radius: 4px;border: 1px solid #A59CB2;font-weight:400;"
		"font-size: 12px;font-family:Microsoft YaHei;" 
		"font-weight: 400;color: #383257;line-height: 16px}"
	"QComboBox::down-arrow {image: url(:/res/images/arr-right.png);}"
	"QComboBox::down-arrow:hover {image: url(:/res/images/arr-right-hover.png);}"
	"QComboBox::down-arrow:pressed {image: url(:/res/images/arr-right-hover.png);}"
	"QComboBox::drop-down{background-color:transparent;"
		"width: 12px;height: 7px;border: 0px;"
		"margin-top:10px;margin-right:6px;}"
	"QComboBox::drop-down:hover{background-color:transparent;"
		"width: 12px;height: 7px;}"
	"QComboBox::drop-down:pressed{background-color:transparent;"
		"width: 12px;height: 7px;}"
	"QComboBox QAbstractItemView{border-radius:4px;"
		"outline:0px;background-color:transparent;border:1px solid #e1dee6;}"
	"QComboBox QAbstractItemView::item{height:36px;border:0px;"
		"font-family:Microsoft YaHei;"
		"background-color:white;color:#383257;"
		"font-size:12px;font-weight:400;line-height:26px;}"
	"QComboBox QAbstractItemView::item:hover{"
		"font-family:Microsoft YaHei;"
		"background-color:#f0eff2;font-weight:400;"
		"color:#383257;line-height:26px;font-size:12px;}"
	"QComboBox QAbstractItemView::item:selected{"
		"font-family:Microsoft YaHei;"
		"background-color:#f0eff2;font-weight:400;"
		"color:#383257;line-height:26px;font-size:12px;}");


FontSet::FontSet(obs_source_t * source,QWidget* parent)
	: MoveDialog(parent)
	, m_pTitle(nullptr)
	, m_pClose(nullptr)
	, m_pFamilyCombo(nullptr)
	, m_pFamilyText(nullptr)
	, m_pSizeCombo(nullptr)
	, m_pTxtColor(nullptr)
	, m_pTxtBgColor(nullptr)
	, m_pBold(nullptr)
	, m_pItalic(nullptr)
	, m_pUnderLine(nullptr)
	, m_pTxtEdit(nullptr)
	, m_pTxtCount(nullptr)
	, m_pOk(nullptr)
	, m_pCancle(nullptr)
	, m_bEnterColor(false)
	, m_bEnterBgColor(false)
	, m_pSizeList(nullptr)
{
	m_bInit = false;
	m_bBold=false;
	m_bItalic=false;
	m_bUnderline=false;
	setFixedSize(503,329);
	SetPixmap(":/res/images/font-set-mask.png");
	setWindowFlags(Qt::FramelessWindowHint|Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	//SetNinePixmap(":res/images/shader.png", 20, 20, 442, 280);
	m_pTitle = new QLabel(tr("add-txt"),this);
	m_pTitle->setStyleSheet("background:transparent;"
		"font-family:Microsoft YaHei;"
		"font-weight: 600;color: #362F5E;line-height: 24px; font-size:16px;");
	m_pTitle->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
	m_pTitle->setGeometry(58,44,100,24);
	m_pClose = new QPushButton(this);
	m_pClose->setGeometry(429,44,16,16);
	m_pClose->setStyleSheet(CLOSE_BTN);
	

	m_pFamilyCombo=new QComboBox(this);
	m_pFamilyCombo->installEventFilter(this);
	m_pFamilyCombo->setStyleSheet(ComboStyle);
	m_pFamilyCombo->setGeometry(58,77,85,28);
	QFontDatabase fd;
#if 0
	QStringList list = fd.families();
#else
	QStringList list ;
	list.push_back(tr("SongTi"));
	list.push_back(tr("WeiRuanYaHei"));
	list.push_back(tr("KaiTi"));
	list.push_back(tr("HeiTi"));
#endif
	
	m_pFamilyList = new QListWidget(this);
	
	m_pFamilyText = new QLineEdit;
	m_pFamilyText->installEventFilter(this);
	m_pFamilyText->setReadOnly(true);
	m_pFamilyText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_pFamilyText->installEventFilter(this);
	m_pFamilyText->setStyleSheet("background-color:transparent;border: none;font-weight:400;color:#EFECF6;font-size:14px;font-family:Microsoft YaHei;");
	m_pFamilyText->setContextMenuPolicy(Qt::NoContextMenu);
	connect(m_pFamilyText, SIGNAL(textChanged(const QString &)), this, SLOT(OnTextChanged(const QString&)));
	m_pFamilyCombo->setLineEdit(m_pFamilyText);
	m_pFamilyCombo->setContextMenuPolicy(Qt::NoContextMenu);
	connect(m_pFamilyCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnCurrentFamilyIndexChanged(int)));
	m_pFamilyList->setStyleSheet("padding: 0px 0px;border: 1px solid #DAD6E2;");
	m_pFamilyList->setContentsMargins(0, 0, 0, 0);
	m_pFamilyList->setFixedWidth(85);
	QString txtStyle = QString(
		"margin-left:7px;"
		"font-size: 12px;"
		"font-family:Microsoft YaHei;"
		"font-weight: 400;"
		"color: #383257;"
		"line-height: 26px;");
	for (int i = 0; i < list.count(); ++i)
	{
		QListWidgetItem *item = new QListWidgetItem(m_pFamilyList);
		STText stText;
		stText.text = list.at(i);
		QVariant v;
		v.setValue(stText);
		item->setData(Qt::UserRole+1,v);
		QLabel *label = new QLabel(list.at(i),m_pFamilyList);
		label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		label->setStyleSheet("margin-left:7px;margin-right:7px;background:transparent;border:none;font-size:12px;font-family:Microsoft YaHei;color:#383257;");
		m_pFamilyList->setItemWidget(item,label);
		//item->setTextAlignment(Qt::AlignLeft);
		m_pFamilyList->addItem(item);
		m_mapIndex2Family[i]=list.at(i);
	}
	m_pFamilyCombo->setModel(m_pFamilyList->model());
	m_pFamilyCombo->setView(m_pFamilyList);
	m_pFamilyCombo->setMaxVisibleItems(5);
	m_pFamilyList->verticalScrollBar()->setStyleSheet(FontScroll);

	

	m_pSizeCombo=new QComboBox(this);
	m_pSizeCombo->installEventFilter(this);
	m_pSizeCombo->setStyleSheet(ComboStyle);
	m_pSizeCombo->setGeometry(149,77,45,28);
	QStringList listSize ;
	listSize << "6" << "7" << "8" << "9" << "10" << "11" << "12" << "14" 
		 <<"16" << "18" <<"20" << "22" << "24" << "26" << "28" << "36"
		 << "48" << "72";
	
	m_pSizeText = new QLineEdit;
	m_pSizeText->installEventFilter(this);
	m_pSizeText->setReadOnly(true);
	m_pSizeText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	m_pSizeText->installEventFilter(this);
	m_pSizeText->setStyleSheet("background-color:transparent;border: none;font-weight:400;color:#EFECF6;font-size:14px;font-family:Microsoft YaHei;");
	m_pSizeText->setContextMenuPolicy(Qt::NoContextMenu);
	connect(m_pSizeText, SIGNAL(textChanged(const QString &)), this, SLOT(OnSizeTetxChanged(const QString&)));
	m_pSizeCombo->setLineEdit(m_pSizeText);
	m_pSizeCombo->setContextMenuPolicy(Qt::NoContextMenu);
	connect(m_pSizeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(OnCurrentSizeIndexChanged(int)));
	m_pSizeList = new QListWidget(this);
	m_pSizeList->setStyleSheet("padding: 0px 0px;border: 1px solid #DAD6E2;");
	m_pSizeList->setContentsMargins(0, 0, 0, 0);
	m_pSizeList->setFixedWidth(45);

	for (int i = 0; i < listSize.count(); ++i)
	{
		QListWidgetItem *item = new QListWidgetItem(m_pSizeList);
		STText stText;
		stText.text = listSize.at(i);
		QVariant v;
		v.setValue(stText);
		item->setData(Qt::UserRole+1,v);
		QLabel *label = new QLabel(listSize.at(i),m_pSizeList);
		label->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
		label->setStyleSheet("margin-left:7px;margin-right:7px;background:transparent;border:none;font-size:12px;font-family:Microsoft YaHei;color:#383257;");
		m_pSizeList->setItemWidget(item,label);
		//item->setTextAlignment(Qt::AlignLeft);
		m_pSizeList->addItem(item);
		m_mapIndex2Size[i]=listSize.at(i);
	}
	
	m_pSizeCombo->setModel(m_pSizeList->model());
	m_pSizeCombo->setView(m_pSizeList);
	m_pSizeCombo->setMaxVisibleItems(5);
	if (listSize.count() > 16) {
		m_pSizeCombo->setCurrentIndex(16);
	}
	m_pSizeList->verticalScrollBar()->setStyleSheet(FontScroll);


	m_pTxtColor=new ColorControl(this);
	QString strIcon1 = QString("background:transparent;border-image:url(:/res/images/txt_color.png);");
	QString strArrow = QString("background:transparent;border-image:url(:/res/images/arr-txt-bottom.png);");
	m_pTxtColor->SetStyle(strIcon1,strArrow);
	m_pTxtColor->setGeometry(200,77,45,28);
	m_pTxtColor->installEventFilter(this);
	

	QString strIcon2 = QString("background:transparent;border-image:url(:/res/images/txt_color_2.png);");
	m_pTxtBgColor=new ColorControl(this);
	m_pTxtBgColor->installEventFilter(this);
	m_pTxtBgColor->SetStyle(strIcon2,strArrow);
	m_pTxtBgColor->setGeometry(251,77,45,28);
	
	QString strBtn = QString("QPushButton{background:transparent;}"
		"QPushButton::checked{background-color:#DFDDE3;border:none;}"
	);
	
	m_pBold = new QPushButton(this);
	m_pBold->setCheckable(true);
	QLabel *lb = new QLabel(m_pBold);
	lb->setGeometry(6,6,16,16);
	lb->setStyleSheet("background:transparent;border-image:url(:/res/images/b.png);");
	m_pBold->setGeometry(302,77,28,28);
	m_pBold->setStyleSheet(strBtn);
	auto BoldClick = [&]() {
		m_bBold = !m_bBold;
		m_pBold->setChecked(m_bBold);
		m_stTxtSet.bold = m_bBold;
		OnSetChanged();
	};
	connect(m_pBold,&QPushButton::clicked,BoldClick);
	m_pItalic = new QPushButton(this);
	m_pItalic->setCheckable(true);
	m_pItalic->setGeometry(340,77,28,28);
	m_pItalic->setStyleSheet(strBtn);
	auto ItalicClick = [&]() {
		m_bItalic = !m_bItalic;
		m_stTxtSet.italic = m_bItalic;
		OnSetChanged();
		m_pItalic->setChecked(m_bItalic);
	};
	connect(m_pItalic,&QPushButton::clicked,ItalicClick);
	QLabel *li = new QLabel(m_pItalic);
	li->setGeometry(6,6,16,16);
	li->setStyleSheet("background:transparent;border-image:url(:/res/images/i.png);");
	m_pUnderLine = new QPushButton(this);
	m_pUnderLine->setCheckable(true);
	m_pUnderLine->setGeometry(376,77,28,28);
	m_pUnderLine->setStyleSheet(strBtn);
	auto UnderLineClick = [&]() {
		m_bUnderline = !m_bUnderline;
		m_pUnderLine->setChecked(m_bUnderline);
		m_stTxtSet.underline = m_bUnderline;
		OnSetChanged();
	};
	connect(m_pUnderLine,&QPushButton::clicked,UnderLineClick);

	QLabel *lu = new QLabel(m_pUnderLine);
	lu->setGeometry(6,6,16,16);
	lu->setStyleSheet("background:transparent;border-image:url(:/res/images/u.png);");
	m_pTxtEdit=new QTextEdit(this);
	m_pTxtEdit->setGeometry(55,117,389,88);
	m_pTxtEdit->setStyleSheet(FONT_SET_TXT);
	m_pTxtEdit->setPlaceholderText(tr("input txt content"));
	m_pTxtEdit->verticalScrollBar()->setStyleSheet(FontScroll);
	connect(m_pTxtEdit, SIGNAL(textChanged()), this, SLOT(OnTextChanged()));
	m_pTxtCount = new QLabel("0/25",this);
	m_pTxtCount->setGeometry(390,180,40,16);
	m_pTxtCount->setStyleSheet(FONT_SET_COUNT);
	m_pTxtCount->setAlignment(Qt::AlignCenter);
	QString strText=tr("add");
	if (source!=nullptr)
		strText=tr("modify");

	m_nCountTips = new QLabel(tr("count tips"),this);
	m_nCountTips->setGeometry(58,205,111,20);
	m_nCountTips->setStyleSheet(FONT_COUNT_TIPS);
	m_nCountTips->hide();

	m_pOk = new QPushButton(strText,this);
	m_pOk->setGeometry(160,228,88,36);
	m_pOk->setStyleSheet(FONT_ADD_BTN);
	m_pCancle = new QPushButton(tr("cancle"),this);
	m_pCancle->setGeometry(256,228,100,36);
	m_pCancle->setStyleSheet(FONT_SET_CANCLE);
	connect(m_pOk,&QPushButton::clicked,this,&FontSet::OnAddClicked);
	connect(m_pCancle,&QPushButton::clicked,this,&FontSet::close);
	connect(m_pClose,&QPushButton::clicked,this,&FontSet::close);
	m_pColorDialog = new ColorDialog(this);
	m_pColorDialog->setGeometry(184,84,190,180);
	m_pColorDialog->installEventFilter(this);
	m_pBgDialog = new ColorDialog(this);
	m_pBgDialog->setGeometry(184,84,190,180);
	m_pBgDialog->installEventFilter(this);
	m_pBgDialog->SetText(tr("bg-color"));
	connect(m_pColorDialog,&ColorDialog::colorSelect,this,&FontSet::OnColorSelect);
	connect(m_pBgDialog,&ColorDialog::colorSelect,this,&FontSet::OnBkColorSelect);

	m_pPosTimer = new QTimer(this);
	m_pPosTimer->setInterval(500);
	auto TSlot = [&]() {
		ResetColorDialogPos();
		HideColorDialog();
	};
	connect(m_pPosTimer, &QTimer::timeout, TSlot);
	m_pPosTimer->start();
	InitSource(source);
	m_bInit = true;
	OnSetChanged();
}

void FontSet::OnColorSelect(const QString & color) {
	if (!color.isEmpty()) {
		m_stTxtSet.txt_color = color;
		OnSetChanged();
		m_pTxtColor->SetColor(color);
	}
}

void FontSet::OnBkColorSelect(const QString & color) {
	if (!color.isEmpty()) {
		m_stTxtSet.bg_color = color;
		OnSetChanged();
		m_pTxtBgColor->SetColor(color);
	}
}

void FontSet::OnTextChanged() {
	if (m_pTxtEdit != nullptr) {
		QString text = m_pTxtEdit->toPlainText();
		int len = text.length();
		if (len > 25) {
			QString strText = text.left(25);
			len = 25;
			m_pTxtEdit->setText(strText);
			QTextCursor cursor = m_pTxtEdit->textCursor();
			cursor.movePosition(QTextCursor::End);
			m_pTxtEdit->setTextCursor(cursor);
		}
		m_pTxtCount->setText(QString("%1/25").arg(len));
		m_nCountTips->setVisible(len==25);
	}
}

void FontSet::ResetColorDialogPos() {
	if (m_pColorDialog->isVisible()) {
		QRect rect = m_pTxtColor->geometry();
		QPoint point = this->mapToGlobal(QPoint(rect.x(), rect.y()));
		QPoint pos(point.x(),point.y() + m_pTxtColor->height());
		m_pColorDialog->move(pos);
	}
	if (m_pBgDialog->isVisible()) {
		QRect rect = m_pTxtBgColor->geometry();
		QPoint point = this->mapToGlobal(QPoint(rect.x(), rect.y()));
		QPoint pos(point.x(),point.y() + m_pTxtBgColor->height());
		m_pBgDialog->move(pos);
	}
}

void FontSet::HideColorDialog() {
	QPoint pos = m_pColorDialog->mapFromGlobal(QCursor::pos());
	if (!m_pColorDialog->rect().contains(pos)) {
		if (!m_bEnterColor && m_pColorDialog->isVisible()) {
			m_pColorDialog->hide();
		}
	}
	QPoint pos2 = m_pBgDialog->mapFromGlobal(QCursor::pos());
	if (!m_pBgDialog->rect().contains(pos2)) {
		if (!m_bEnterBgColor && m_pBgDialog->isVisible()) {
			m_pBgDialog->hide();
		}
	}
}

bool FontSet::eventFilter(QObject* obj, QEvent* event) {
	if (obj == m_pFamilyText && event->type()==event->MouseButtonPress) {
		m_pFamilyCombo->showPopup();
	}
	if (m_pFamilyCombo == obj && event->type() == QEvent::Wheel) {
		return true;
	}
	if (obj == m_pSizeText && event->type()==event->MouseButtonPress) {
		m_pSizeCombo->showPopup();
	}
	if (m_pSizeCombo == obj && event->type() == QEvent::Wheel) {
		return true;
	}
	if (obj == m_pTxtColor) {
		if (event->type() == QEvent::Enter) {
			m_bEnterColor = true;
			m_pBgDialog->hide();
			m_pColorDialog->show();
			ResetColorDialogPos();
			
		} else if (event->type() == QEvent::Leave) {
			m_bEnterColor = false;
		}
	}
	if (obj == m_pTxtBgColor) {
		if (event->type() == QEvent::Enter) {
			m_bEnterBgColor = true;
			m_pColorDialog->hide();
			m_pBgDialog->show();
			ResetColorDialogPos();
			
		} else if (event->type() == QEvent::Leave) {
			m_bEnterBgColor = false;
		}
	}
	return __super::eventFilter(obj,event);
}

void FontSet::OnAddClicked() {
	m_stTxtSet.text = m_pTxtEdit->toPlainText();
	if (!m_stTxtSet.text.isEmpty()) {
		emit addTxtSignal(m_stTxtSet);
		close();
	}
	else {
		MsgBox::information(nullptr, StringConvert::GetInstance()->GetString(STR_PROCESS), StringConvert::GetInstance()->GetString(STR_FONT_SET_NOEMPTY));
	}
}

void FontSet::OnTextChanged(const QString& text) {
	qDebug()<<"text:"<<text;
	STText stText = qvariant_cast<STText>(m_pFamilyCombo->itemData(m_pFamilyCombo->currentIndex(),Qt::UserRole+1));
	m_pFamilyText->setText(stText.text);
	m_stTxtSet.family=stText.text;
	OnSetChanged();
}


void FontSet::OnCurrentFamilyIndexChanged(int index) {
	STText stText = qvariant_cast<STText>(m_pFamilyCombo->itemData(index,Qt::UserRole+1));
	m_pFamilyText->setText(stText.text);
	m_stTxtSet.family=stText.text;
	OnSetChanged();
}

void FontSet::OnSizeTetxChanged(const QString& text) {
	STText stText = qvariant_cast<STText>(m_pSizeCombo->itemData(m_pSizeCombo->currentIndex(),Qt::UserRole+1));
	m_pSizeText->setText(stText.text);
	m_stTxtSet.font_size=stText.text;
	OnSetChanged();
	if (m_pSizeList != nullptr && m_bInit) {
		m_pSizeCombo->view()->update();
	}
}

void FontSet::OnCurrentSizeIndexChanged(int index) {
	STText stText = qvariant_cast<STText>(m_pSizeCombo->itemData(index,Qt::UserRole+1));
	m_pSizeText->setText(stText.text);
	m_stTxtSet.font_size=stText.text;
	OnSetChanged();
	if (m_pSizeList != nullptr && m_bInit) {
		m_pSizeCombo->view()->update();
	}
}


FontSet::~FontSet()
{

}

void FontSet::OnSetChanged() {
	QString strStyle = QString(
	"margin-left:5px;margin-top:5px;margin-right:5px;margin-bottom:5px;"
	"border-radius: 4px;border: 1px solid #D9D4E3;"
	);
	if (m_stTxtSet.family.isEmpty()) {
		strStyle+="font-family :Microsoft YaHei;";
	} else {
		strStyle += QString("font-family : %1;").arg(m_stTxtSet.family);
	}
	if (m_stTxtSet.font_size.isEmpty()) {
		strStyle+="font-size : 48px;";
	} else {
		strStyle += QString("font-size : %2px;").arg(m_stTxtSet.font_size);
	}
	if (m_stTxtSet.txt_color.isEmpty()) {
		strStyle += "color : #5C567B;";
	} else {
		strStyle += QString("color:%1;").arg(m_stTxtSet.txt_color);
	}
	if (m_stTxtSet.bg_color.isEmpty()) {
		strStyle += "background-color : #ffffff;";
	} else {
		strStyle += QString("background-color:%1;").arg(m_stTxtSet.bg_color);
	}
	if (m_stTxtSet.bold) {
		strStyle += "font-weight : bold;";
	} else {
		strStyle += "font-weight : normal;";
	}
	if (m_stTxtSet.italic) {
		strStyle += "font-style : italic;";
	} else {
		strStyle += "font-style : normal;";
	}
	if (m_stTxtSet.underline) {
		strStyle += "text-decoration: underline;";
	} else {
		strStyle += "text-decoration: none;";
	}
	
	if (m_pTxtEdit != nullptr) {
		m_pTxtEdit->setStyleSheet(strStyle);
	}
}

void FontSet::InitSource(obs_source_t* source) {
    m_pSource = source;
    if (m_pSource != nullptr) {
	const char* name =obs_source_get_name(source);
	std::string source_name = CharatersConvert::UTF8ToGBK(std::string(name));
	if (name && *name) {
		memcpy(m_stTxtSet.name,name,std::min<int>(strlen(name),_countof(m_stTxtSet.name)));
	}
        obs_data_t*  settings = obs_source_get_settings(source);
        const char *text = obs_data_get_string(settings,"text");
	long long color = obs_data_get_int(settings, "color");
	long long bk_color = obs_data_get_int(settings, "bk_color");
	obs_data_t *font_setting = obs_data_get_obj(settings,"font");
	const char* face = obs_data_get_string(font_setting,"face");
	int flags = obs_data_get_int(font_setting, "flags");
	int font_size = obs_data_get_int(font_setting, "size");
	if (text && *text) {
		m_pTxtEdit->setText(QString::fromStdString(std::string(text)));
		m_stTxtSet.text = QString::fromStdString(std::string(text));
	}
	if (color >= 0) {
		QColor color_obj = common::color_from_int(color);
		int red = color_obj.red();
		int green = color_obj.green();
		int blue = color_obj.blue();
		char data[128] = {0};
		sprintf(data,"#%02X%02X%02X",red,green,blue);
		m_pColorDialog->SetInitColor(data);
		m_pTxtColor->SetColor(data);
		m_stTxtSet.txt_color = QString(data);
	}
	if (bk_color >= 0) {
		QColor color_obj = common::color_from_int(bk_color);
		int red = color_obj.red();
		int green = color_obj.green();
		int blue = color_obj.blue();
		char data[128] = {0};
		sprintf(data,"#%02X%02X%02X",red,green,blue);
		m_pBgDialog->SetInitColor(data);
		m_pTxtBgColor->SetColor(data);
		m_stTxtSet.bg_color = QString(data);
	}
	if (face && *face) {
		QMap<int, QString>::iterator iter = m_mapIndex2Family.begin();
		while (iter!=m_mapIndex2Family.end()) {
			if (iter.value().compare(face) == 0) {
				m_stTxtSet.family=iter.value();
				m_pFamilyCombo->setCurrentIndex(iter.key());
			}
			iter++;
		}
	}
	if (font_size > 0) {
		QMap<int, QString>::iterator iter = m_mapIndex2Size.begin();
		while (iter!=m_mapIndex2Size.end()) {
			if (iter.value().compare(QString::number(font_size)) == 0) {
				m_pSizeCombo->setCurrentIndex(iter.key());
				m_stTxtSet.font_size=iter.value();
			}
			iter++;
		}
	}
	if (flags > 0) {
		if (flags & OBS_FONT_BOLD) {
			m_pBold->setChecked(true);
			m_bBold = true;
			m_stTxtSet.bold = true;
		}
		if (flags & OBS_FONT_ITALIC) {
			m_pItalic->setChecked(true);
			m_bItalic = true;
			m_stTxtSet.italic = m_bItalic;
		}
		if (flags & OBS_FONT_UNDERLINE) {
			m_pUnderLine->setChecked(true);
			m_bUnderline = true;
			m_stTxtSet.italic = m_bUnderline;
		}
	}
	OnSetChanged();
    }
}
