#include "live-partner.h"

#include <QLayout>
#include <QTabWidget>
#include <QTabBar>
#include <QStyle>
#include <QStyleOption>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QScrollBar>
#include <QCoreApplication>
#include <QDebug>

#include "custom-stylesheet.h"
#include "signal-proxy_model.h"
#include "common.h"
#include "util/base.h"

QSize TabStyle::sizeFromContents(ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
	QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
	if (type == QStyle::CT_TabBarTab) {
		s.transpose();
		s.rwidth() = 86; 
		s.rheight() = 34;
	}
	return s;
}

void TabStyle::drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
	if (element == CE_TabBarTabLabel) {
		if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
			QRect allRect = tab->rect;

			if (tab->state & QStyle::State_Selected) {
				painter->save();
				painter->fillRect(allRect, QColor(255, 219, 0));
				painter->restore();
			} else {
				painter->save();
				painter->fillRect(allRect, QColor(243, 243, 243));
				painter->restore();
			}
			QTextOption option;
			option.setAlignment(Qt::AlignCenter);
			if (tab->state & QStyle::State_Selected) {
				painter->setPen(QColor(34, 34, 34));
			}
			else {
				painter->setPen(QColor(85, 85, 85));
			}
			QFont font;
			font.setPixelSize(14);
			painter->setFont(font);
			painter->drawText(allRect, tab->text, option);
			return;
		}
	}

	if (element == CE_TabBarTab) {
		QProxyStyle::drawControl(element, option, painter, widget);
	}
}

SliderEdit::SliderEdit(const QString &name, QWidget *parent, int value)
	: QWidget(parent)
	, m_pSlider(nullptr)
	, m_pLineEdit(nullptr)
{
	QHBoxLayout *h1= new QHBoxLayout(this);
	h1->setContentsMargins(0,0,0,0);
	h1->setSpacing(15);

	QLabel *l1 = new QLabel(name, this);
	l1->setStyleSheet("QLabel {color:rgb(68, 68, 68); font-size:14px; font-weight: 400; font-family:Microsoft YaHei;}");
	h1->addWidget(l1);

	auto sliderChange = [&](int value) {
		if (m_pLineEdit) {
			m_pLineEdit->setText(QString::number(value));
			emit valueChange(value);
		}
	};

	m_pSlider = new QSlider(this);
	m_pSlider->setOrientation(Qt::Horizontal);
	m_pSlider->setMinimum(0);
	m_pSlider->setMaximum(100);
	m_pSlider->setSingleStep(1);
	m_pSlider->setValue(value);
	m_pSlider->setStyleSheet("QSlider::add-page:Horizontal{background-color:rgba(0,0,0,10);height:4px;border-radius:2px;}"
			"QSlider::sub-page:Horizontal{background-color:rgb(255, 192, 0);height:4px;border-radius:2px;}"
			"QSlider::groove:Horizontal {background:transparent;height:4px;}"
			"QSlider::handle:Horizontal {height:14px;width:14px;background:rgb(255,255,255);margin: -5 0px; border:1px solid rgb(226, 226, 226); border-radius:7px;}");
	connect(m_pSlider, &QSlider::valueChanged, sliderChange);
	h1->addWidget(m_pSlider);

	auto editChange = [&](const QString &text){
		if (m_pSlider) {
			if (text.isEmpty()) {
				m_pLineEdit->setText("0");
				m_pSlider->setValue(0);
			} else {
				m_pSlider->setValue(text.toInt());
			}
		}
	};

	m_pLineEdit = new QLineEdit(this);
	m_pLineEdit->setFixedSize(50, 26);
	m_pLineEdit->setAlignment(Qt::AlignHCenter);
	m_pLineEdit->setText(QString::number(value));
	m_pLineEdit->setStyleSheet("QLineEdit{border-radius: 4px; border:1px solid rgb(204, 204, 204); font-size:12px; color:rgb(68, 68, 68); background-color:transparent;font-family:Microsoft YaHei;}");
	connect(m_pLineEdit, &QLineEdit::textChanged, editChange);
	h1->addWidget(m_pLineEdit);

	QRegExp reg("^100$|^(\\d|[1-9]\\d)$");
	QRegExpValidator *validator = new QRegExpValidator(reg, this);
	m_pLineEdit->setValidator(validator);
}

SliderEdit::~SliderEdit() {}

void SliderEdit::SetData(QVariant variant) {
	m_variant = variant;
}

QVariant SliderEdit::GetData()
{
	return m_variant;
}

void SliderEdit::SetValue(int value) {
	if (value >=0 && value <= 100 && m_pLineEdit) {
		m_pLineEdit->setText(QString::number(value));
	}
}

BeautyFaceWidget::BeautyFaceWidget(QMap<BeautyFace_Type, int> &beautyData, QWidget *parent)
	: QWidget(parent)
	, m_pSe1(nullptr)
	, m_pSe2(nullptr)
	, m_pSe3(nullptr)
	, m_pSe4(nullptr)
	, m_pSe5(nullptr)
	, m_pSe6(nullptr)
	, m_pSe7(nullptr)
{
	setStyleSheet("QWidget {background-color:rgb(253, 253, 253);}");
	QVBoxLayout *v1 = new QVBoxLayout(this);
	v1->setContentsMargins(30,5,30,25);
	v1->setSpacing(16);

	m_pSe1 = new SliderEdit(QString(tr("beauty_white")), this);
	m_pSe1->SetData(QVariant((int)BeautyFace_SkinWhite));
	m_pSe1->SetValue(beautyData.find(BeautyFace_SkinWhite).value());
	connect(m_pSe1, &SliderEdit::valueChange, this, &BeautyFaceWidget::OnValueChange);
	v1->addWidget(m_pSe1);

	m_pSe2 = new SliderEdit(QString(tr("beauty_moulting")), this);
	m_pSe2->SetData(QVariant((int)BeautyFace_Moulting));
	m_pSe2->SetValue(beautyData.find(BeautyFace_Moulting).value());
	connect(m_pSe2, &SliderEdit::valueChange, this, &BeautyFaceWidget::OnValueChange);
	v1->addWidget(m_pSe2);

	m_pSe3 = new SliderEdit(QString(tr("beauty_ruddy")), this);
	m_pSe3->SetData(QVariant((int)BeautyFace_Ruddy));
	m_pSe3->SetValue(beautyData.find(BeautyFace_Ruddy).value());
	connect(m_pSe3, &SliderEdit::valueChange, this, &BeautyFaceWidget::OnValueChange);
	v1->addWidget(m_pSe3);

	m_pSe4 = new SliderEdit(QString(tr("beauty_eyes_reshape")), this);
	m_pSe4->SetData(QVariant((int)BeautyFace_EyesReshape));
	m_pSe4->SetValue(beautyData.find(BeautyFace_EyesReshape).value());
	connect(m_pSe4, &SliderEdit::valueChange, this, &BeautyFaceWidget::OnValueChange);
	v1->addWidget(m_pSe4);

	m_pSe5 = new SliderEdit(QString(tr("beauty_face_lift")), this);
	m_pSe5->SetData(QVariant((int)BeautyFace_FaceLift));
	m_pSe5->SetValue(beautyData.find(BeautyFace_FaceLift).value());
	connect(m_pSe5, &SliderEdit::valueChange, this, &BeautyFaceWidget::OnValueChange);
	v1->addWidget(m_pSe5);

	m_pSe6 = new SliderEdit(QString(tr("beauty_blusher")), this);
	m_pSe6->SetData(QVariant((int)BeautyFace_Blusher));
	m_pSe6->SetValue(beautyData.find(BeautyFace_Blusher).value());
	connect(m_pSe6, &SliderEdit::valueChange, this, &BeautyFaceWidget::OnValueChange);
	v1->addWidget(m_pSe6);

	m_pSe7 = new SliderEdit(QString(tr("beauty_thin_chin")), this);
	m_pSe7->SetData(QVariant((int)BeautyFace_ThinChin));
	m_pSe7->SetValue(beautyData.find(BeautyFace_ThinChin).value());
	connect(m_pSe7, &SliderEdit::valueChange, this, &BeautyFaceWidget::OnValueChange);
	v1->addWidget(m_pSe7);
	v1->addStretch();

	QHBoxLayout *h1 = new QHBoxLayout();
	h1->setContentsMargins(0,0,0,0);
	h1->setSpacing(0);
	v1->addLayout(h1);
	h1->addStretch();

	QPushButton *btn = new QPushButton(QString(tr("all_reset")), this);
	btn->setFixedSize(86,32);
	btn->setStyleSheet("QPushButton {background-color:rgb(255,255,255); border:1px solid #EBEBEF; font-size:14px; color:#222222; border-radius:6px; font-weight: 400; font-family:Microsoft YaHei;}"
			"QPushButton:hover {background-color:rgb(232, 229, 237);}");
	connect(btn, &QPushButton::clicked, this, &BeautyFaceWidget::Reset);
	h1->addWidget(btn);
}

BeautyFaceWidget::~BeautyFaceWidget() {}

void BeautyFaceWidget::OnValueChange(int value) {
	SliderEdit *sliderEdit = qobject_cast<SliderEdit *>(sender());
	BeautyFace_Type type = (BeautyFace_Type)sliderEdit->GetData().toInt();
	blog(LOG_INFO, "ChangeBeautyFace type:%d, value:%d", type, value);
	emit ChangeBeautyFace(type, value);
}

void BeautyFaceWidget::Reset() {
	m_pSe1->SetValue(50);
	emit ChangeBeautyFace(BeautyFace_SkinWhite, 50);

	m_pSe2->SetValue(8);
	emit ChangeBeautyFace(BeautyFace_Moulting, 8);

	m_pSe3->SetValue(50);
	emit ChangeBeautyFace(BeautyFace_Ruddy, 50);

	m_pSe4->SetValue(8);
	emit ChangeBeautyFace(BeautyFace_EyesReshape, 8);

	m_pSe5->SetValue(8);
	emit ChangeBeautyFace(BeautyFace_FaceLift, 8);

	m_pSe6->SetValue(16);
	emit ChangeBeautyFace(BeautyFace_Blusher, 16);

	m_pSe7->SetValue(8);
	emit ChangeBeautyFace(BeautyFace_ThinChin, 8);
}

void BeautyFaceWidget::paintEvent(QPaintEvent *event) {
	QStyleOption option;
	option.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);
}

FilterButton::FilterButton(const QString &path, QWidget *parent)
	: QPushButton(parent)
	, m_pCheckLabel(nullptr)
{
	setCheckable(true);
	setFixedSize(72,72);
	setStyleSheet("QPushButton{border: 2px solid rgb(217, 212, 227);border-radius:8px;}"
			"QPushButton:hover{border: 2px solid rgb(255, 185, 0);border-radius:8px;}"
			"QPushButton:pressed{border: 2px solid rgb(255, 185, 0);border-radius:8px;}"
			"QPushButton:checked{border: 2px solid rgb(255, 185, 0);border-radius:8px;}");

	QHBoxLayout *h1 = new QHBoxLayout(this);
	h1->setContentsMargins(2,2,2,2);
	h1->setSpacing(0);
	QLabel *l1 = new QLabel();
	l1->setStyleSheet(QString("QLabel {border-radius:8px;border-image:url(") + path + QString(");}"));
	h1->addWidget(l1);

	m_pCheckLabel = new QLabel(this);
	m_pCheckLabel->setFixedSize(20,20);
	m_pCheckLabel->setGeometry(52,52,20,20);
	m_pCheckLabel->setStyleSheet("QLabel {border-image:url(:res/images/check.png); border-bottom-right-radius:8px; background:transparent;}");
	m_pCheckLabel->setVisible(false);
}

FilterButton::~FilterButton() {}

void FilterButton::SetSelect(bool select) {
	m_pCheckLabel->setVisible(select);
	setChecked(select);
}

FilterItem::FilterItem(const QString &name, const QString &path, QWidget *parent)
	: QWidget(parent)
	, m_pFilterButton(nullptr)
{
	QVBoxLayout *mainlayout = new QVBoxLayout(this);
	mainlayout->setContentsMargins(0,0,0,0);
	mainlayout->setSpacing(10);

	auto click = [&]{
		emit clicked(this);
	};

	m_pFilterButton = new FilterButton(path, this);
	connect(m_pFilterButton, &FilterButton::clicked, click);
	mainlayout->addWidget(m_pFilterButton);

	QLabel *label = new QLabel(name, this);
	label->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
	label->setStyleSheet("QLabel {color:rgb(102, 102, 102); font-size:12px; background:transparent;font-family:Microsoft YaHei;}");
	mainlayout->addWidget(label);
}

FilterItem::~FilterItem() {}

void FilterItem::SetSelect(bool bSelect) {
	m_pFilterButton->SetSelect(bSelect);
}

void FilterItem::SetData(QVariant variant) {
	m_varData = variant;
}

QVariant FilterItem::GetData()
{
	return m_varData;
}

FilterWidget::FilterWidget(Filter_Type type, QWidget *parent)
	: QWidget(parent)
	, m_pListWidget(nullptr)
	, m_type(type)
{
	setStyleSheet("QWidget {background-color:rgb(253, 253, 253);}");
	QVBoxLayout *mainlayout = new QVBoxLayout(this);
	mainlayout->setContentsMargins(20,0,20,10);
	mainlayout->setSpacing(0);

	m_pListWidget = new QListWidget(this);
	m_pListWidget->setSpacing(12);
	m_pListWidget->setFixedSize(358,324);
	m_pListWidget->setViewMode(QListView::IconMode);
	m_pListWidget->setResizeMode(QListView::Adjust);
	m_pListWidget->setStyleSheet("QListWidget {border: 0px; padding:0px; margin:0px; background-color:transparent;}"
			"QListWidget:focus {outline: none;} ");
	m_pListWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:none;margin:0px,0px,0px,0px;width:4px; border-radius:2px;background-color:#E8E5ED;}"
			"QScrollBar:vertical:hover {margin: 0px,0px,0px,0px; background-color:#E8E5ED;}"
			"QScrollBar::handle:vertical {max-width:4px; background-color:rgb(187, 187, 187);  height: 40px; border-radius: 2px;}"
			"QScrollBar::add-line:vertical {height:0px;}"
			"QScrollBar::sub-line:vertical {height:0px;}"
			"QScrollBar::add-page:vertical {background:transparent;}"
			"QScrollBar::sub-page:vertical {background:transparent;}");
	m_pListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	m_pListWidget->verticalScrollBar()->setSingleStep(30);
	mainlayout->addWidget(m_pListWidget);

	QHBoxLayout *horlayout = new QHBoxLayout();
	horlayout->setContentsMargins(0,0,0,0);
	horlayout->setSpacing(0);
	mainlayout->addLayout(horlayout);
	horlayout->addStretch();

	QPushButton *resetBtn = new QPushButton(QString(tr("all_reset")), this);
	resetBtn->setFixedSize(86,32);
	resetBtn->setStyleSheet("QPushButton {background-color:rgb(255,255,255); border:1px solid #EBEBEF; font-size:14px; color:#222222; border-radius:6px; font-weight: 400; font-family:Microsoft YaHei;}"
			"QPushButton:hover {background-color:rgb(232, 229, 237);}");
	connect(resetBtn, &QPushButton::clicked, this, &FilterWidget::Reset);
	horlayout->addWidget(resetBtn);
	horlayout->addSpacing(12);

	auto click = [&] {
		emit Confirm();
	};
	QPushButton *confirmBtn = new QPushButton(QString(tr("confirm")), this);
	confirmBtn->setFixedSize(86, 32);
	confirmBtn->setStyleSheet("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px; font-size:14px; color:#222222;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");
	connect(confirmBtn, &QPushButton::clicked, click);
	horlayout->addWidget(confirmBtn);

	horlayout->addSpacing(22);

	for (int i = 0; i < 24; i++) {
		AddItem(i);
	}
}

FilterWidget::~FilterWidget() {
}

void FilterWidget::GetData(Filter_Type type, QString &name, QString &path)
{
	QString progpath = QCoreApplication::instance()->applicationDirPath();

	switch (type) {
	case Filter_Normal:
		name = QString(tr("filter_nomal"));
		path = QString(":filter/images/filter/a_filter_normal.jpg");
		break;
	case Filter_Baixi:
		name = QString(tr("filter_baixi"));
		path = QString(":filter/images/filter/a_filter_baixi.jpg");
		break;
	case Filter_Chaobai:
		name = QString(tr("filter_chaobai"));
		path = QString(":filter/images/filter/a_filter_chaobai.jpg");
		break;
	case Filter_Qingyang:
		name = QString(tr("filter_qingyang"));
		path = QString(":filter/images/filter/a_filter_qingyang.jpg");
		break;
	case Filter_Baixue:
		name = QString(tr("filter_baixue"));
		path = QString(":filter/images/filter/b_filter_baixue.jpg");
		break;
	case Filter_Chulian:
		name = QString(tr("filter_chulian"));
		path = QString(":filter/images/filter/b_filter_chulian.jpg");
		break;
	case Filter_Naiyou:
		name = QString(tr("filter_naiyou"));
		path = QString(":filter/images/filter/b_filter_naiyou.jpg");
		break;
	case Filter_Qingchun:
		name = QString(tr("filter_qingchun"));
		path = QString(":filter/images/filter/b_filter_qingchun.jpg");
		break;
	case Filter_Rouhe:
		name = QString(tr("filter_rouhe"));
		path = QString(":filter/images/filter/b_filter_rouhe.jpg");
		break;
	case Filter_Weixun:
		name = QString(tr("filter_weixun"));
		path = QString(":filter/images/filter/b_filter_weixun.jpg");
		break;
	case Filter_Chuxin:
		name = QString(tr("filter_chuxin"));
		path = QString(":filter/images/filter/c_filter_chuxin.jpg");
		break;
	case Filter_Feifan:
		name = QString(tr("filter_feifan"));
		path = QString(":filter/images/filter/c_filter_feifan.jpg");
		break;
	case Filter_Huopo:
		name = QString(tr("filter_huopo"));
		path = QString(":filter/images/filter/c_filter_huopo.jpg");
		break;
	case Filter_Musi:
		name = QString(tr("filter_musi"));
		path = QString(":filter/images/filter/c_filter_musi.jpg");
		break;
	case Filter_Qiangwei:
		name = QString(tr("filter_qiangwei"));
		path = QString(":filter/images/filter/c_filter_qiangwei.jpg");
		break;
	case Filter_Dongren:
		name = QString(tr("filter_dongren"));
		path = QString(":filter/images/filter/d_filter_dongren.jpg");
		break;
	case Filter_Landiao:
		name = QString(tr("filter_landiao"));
		path = QString(":filter/images/filter/d_filter_landiao.jpg");
		break;
	case Filter_Naihui:
		name = QString(tr("filter_naihui"));
		path = QString(":filter/images/filter/d_filter_naihui.jpg");
		break;
	case Filter_Quqi:
		name = QString(tr("filter_quqi"));
		path = QString(":filter/images/filter/d_filter_quqi.jpg");
		break;
	case Filter_Rixi:
		name = QString(tr("filter_rixi"));
		path = QString(":filter/images/filter/d_filter_rixi.jpg");
		break;
	case Filter_Shensui:
		name = QString(tr("filter_shensui"));
		path = QString(":filter/images/filter/d_filter_shensui.jpg");
		break;
	case Filter_Fu:
		name = QString(tr("filter_fugu"));
		path = QString(":filter/images/filter/e_filter_fugu.png");
		break;
	case Filter_Bai:
		name = QString(tr("filter_lihuabai"));
		path = QString(":filter/images/filter/e_filter_lihuabai.png");
		break;
	case Filter_Fen:
		name = QString(tr("filter_rouwufen"));
		path = QString(":filter/images/filter/e_filter_rouwufen.png");
		break;
	default:
		break;
	}
}

void FilterWidget::AddItem(int i)
{
	QString name, path;
	GetData((Filter_Type)i, name, path);
	QString filterPath = common::GetFilterPath((Filter_Type)i);

	FilterItem *filterItem = new FilterItem(name, path, m_pListWidget);
	filterItem->SetData(QVariant(i));
	if ((Filter_Type)i == m_type)
		filterItem->SetSelect(true);

	connect(filterItem, &FilterItem::clicked, this, &FilterWidget::OnItemClicked);
	QListWidgetItem *item = new QListWidgetItem();
	//设置 Item 的大小
	item->setSizeHint(QSize(72,92));
	m_pListWidget->addItem(item);
	m_pListWidget->setItemWidget(item, filterItem);
}

void FilterWidget::OnItemClicked(FilterItem *item) {
	int count = m_pListWidget->count();
	for (int i = 0; i < count; i++) {
		FilterItem *pfilterItem = qobject_cast<FilterItem *>(m_pListWidget->itemWidget(m_pListWidget->item(i)));
		pfilterItem->SetSelect(false);
	}

	if (item) {
		item->SetSelect(true);
		blog(LOG_INFO, "FilterChange type:%d", item->GetData().toInt());
		emit FilterChange(item->GetData().toInt());
	}
}

void FilterWidget::paintEvent(QPaintEvent *event) {
	QStyleOption option;
	option.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &option, &p, this);
}

void FilterWidget::Reset() {
	int count = m_pListWidget->count();
	FilterItem *pfilterItem = nullptr;
	for (int i = 0; i < count; i++) {
		pfilterItem = qobject_cast<FilterItem *>(m_pListWidget->itemWidget(m_pListWidget->item(i)));
		pfilterItem->SetSelect(false);
	}
	pfilterItem = qobject_cast<FilterItem *>(m_pListWidget->itemWidget(m_pListWidget->item(1)));
	pfilterItem->SetSelect(true);

	emit FilterChange(pfilterItem->GetData().toInt());
}

LivePartner::LivePartner(Filter_Type type, QMap<BeautyFace_Type, int> &beautyData, QWidget *parent)
	: MoveDialog(parent)
	, m_pFilterWidget(nullptr)
	, m_type(type)
{
	setFixedSize(556, 504);
	SetPixmap(":res/images/beauty_mask.png");
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	//SetNinePixmap(":res/images/shader.png", 20, 20, 540, 480);

	QVBoxLayout *mainlayout = new QVBoxLayout(this);
	mainlayout->setContentsMargins(38,29,38,47);
	mainlayout->setSpacing(0);

	QHBoxLayout *horlayout = new QHBoxLayout();
	horlayout->setContentsMargins(20,15,20,15);
	mainlayout->addLayout(horlayout);

	QLabel *label = new QLabel(QString(tr("title")), this);
	label->setStyleSheet("QLabel {color:rgb(34, 34, 34); font-size:16px; font-weight: 600; font-family:Microsoft YaHei; background:transparent;}");
	horlayout->addWidget(label);
	horlayout->addStretch();

	QPushButton *closeBtn = new QPushButton(this);
	closeBtn->setFixedSize(18, 18);
	closeBtn->setStyleSheet(CLOSE_BTN);
	connect(closeBtn, &QPushButton::clicked, this, &LivePartner::OnCloseClick);
	horlayout->addWidget(closeBtn);

	QTabWidget *tabwidget = new QTabWidget(this);
	tabwidget->setObjectName("tabwidget");
	tabwidget->setTabPosition(QTabWidget::West);
	tabwidget->setAttribute(Qt::WA_StyledBackground);
	tabwidget->setAutoFillBackground(true);
	tabwidget->setStyleSheet("QTabWidget#tabwidget {border:0px; background-color:rgb(243,243,243);}"
			"QTabWidget#tabwidget::pane{border: 0px;}");
	tabwidget->tabBar()->setStyle(new TabStyle());
	mainlayout->addWidget(tabwidget);

	BeautyFaceWidget *faceWidget = new BeautyFaceWidget(beautyData, this);
	connect(faceWidget, &BeautyFaceWidget::ChangeBeautyFace, SignalProxyModel::GetInstance(), &SignalProxyModel::ChangeBeautyFace, Qt::UniqueConnection);
	tabwidget->addTab(faceWidget, QString(tr("beauty_face")));

	auto confirm_click = [&] {
		done(QDialog::Accepted);
		close();
	};
	m_pFilterWidget = new FilterWidget(type, this);
	connect(m_pFilterWidget, &FilterWidget::FilterChange, SignalProxyModel::GetInstance(), &SignalProxyModel::FilterChange, Qt::UniqueConnection);
	connect(m_pFilterWidget, &FilterWidget::Confirm, confirm_click);
	tabwidget->addTab(m_pFilterWidget, QString(tr("filter")));
}

LivePartner::~LivePartner() {}

void LivePartner::OnCloseClick() {
	m_pFilterWidget->FilterChange(m_type);
	done(QDialog::Accepted);
	close();
}
