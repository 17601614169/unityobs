#include "customer-service-base-dialog.h"

#include <QPushButton>
#include <QBoxLayout>
#include <QListWidget>
#include <QComboBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QScrollBar>
#include <sstream>
#include <QPropertyAnimation>
#include <QPainter>
#include <QScrollArea>
#include <QCheckBox>
#include <QLineEdit>
#include <QRegExp>
#include <QMenu>
#include <QTimerEvent>
#include <QMessageBox>
#include <QTextEdit>
#include <QEventLoop>
#include <QTimer>
#include <QFileDialog>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QDebug>
#include <QDateTime>
#include <QCoreApplication>

#include "custom-stylesheet.h"
#include "signal-proxy_model.h"
#include "string-convert.h"
#include "url-label.h"
#include "json-parser.h"
#include "obs.hpp"
#include <json/json.h>
#include <ctime>
#include <iomanip>
#include "base-define.h"
#include "custom-config-ini.h"
#include "md5.h"
#include "msgbox.h"
#include "live-partner.h"
#include "common.h"

static std::string ResString(uint32_t cx, uint32_t cy)
{
	std::stringstream res;
	res << cx << "x" << cy;
	return res.str();
}

//ConfirmDialog::ConfirmDialog(QWidget *parent)
//	: MoveDialog(parent)
//{
//	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
//	setWindowModality(Qt::WindowModal);
//	SetColor(QString("#FFFFFF"), 10);
//	setFixedSize(380,264);
//
//	QVBoxLayout *mainLayout = new QVBoxLayout(this);
//	mainLayout->setContentsMargins(0,0,0,35);
//	mainLayout->setSpacing(0);
//
//	QWidget *topWidget = new QWidget(this);
//	topWidget->setFixedHeight(40);
//	topWidget->setObjectName("cs_top");
//	topWidget->setStyleSheet(CS_BASE_DIALOG_TOP_BG);
//	mainLayout->addWidget(topWidget);
//
//	QHBoxLayout *horLayout = new QHBoxLayout(topWidget);
//	horLayout->setContentsMargins(17,8,20,10);
//	QLabel *title = new QLabel(QString(tr("confirm_tips")), topWidget);
//	title->setStyleSheet(CS_BASE_DIALOG_TITLE);
//	horLayout->addWidget(title);
//	horLayout->addStretch();
//
//	auto on_close = [&] {
//		done(DialogCode::Rejected);
//		close();
//	};
//
//	QPushButton *closeBtn = new QPushButton(topWidget);
//	closeBtn->setFixedSize(16,16);
//	closeBtn->setStyleSheet(CS_BASE_DIALOG_CLOSE);
//	connect(closeBtn, &QPushButton::clicked, this, on_close);
//	horLayout->addWidget(closeBtn);
//
//	mainLayout->addStretch();
//
//	QVBoxLayout *contentLayout = new QVBoxLayout();
//	contentLayout->setContentsMargins(30,0,30,0);
//	contentLayout->setSpacing(10);
//	mainLayout->addLayout(contentLayout);
//
//	QLabel *label1 = new QLabel(QString(tr("update_set")), this);
//	label1->setStyleSheet(CS_BASE_DIALOG_TITLE);
//	contentLayout->addWidget(label1);
//	QLabel *label2 = new QLabel(QString(tr("is_confirm")), this);
//	label2->setStyleSheet(FINISH_DIALOG_LABEL);
//	contentLayout->addWidget(label2);
//
//	mainLayout->addStretch();
//
//	QHBoxLayout *bottomLayout = new QHBoxLayout();
//	bottomLayout->setContentsMargins(0,0,0,0);
//	bottomLayout->setSpacing(16);
//	bottomLayout->addStretch();
//
//	QPushButton *cancelBtn = new QPushButton(QString(tr("cancel")), this);
//	cancelBtn->setStyleSheet(CS_BASE_DIALOG_CANCEL);
//	cancelBtn->setFixedSize(100, 36);
//	connect(cancelBtn, &QPushButton::clicked, this, on_close);
//	bottomLayout->addWidget(cancelBtn);
//
//	auto on_confirm = [&] {
//		done(DialogCode::Accepted);
//		close();
//	};
//
//	QPushButton *confirmBtn = new QPushButton(QString(tr("determine")), this);
//	confirmBtn->setStyleSheet(CS_BASE_DIALOG_CONFIRM);
//	confirmBtn->setFixedSize(100, 36);
//	connect(confirmBtn, &QPushButton::clicked, this, on_confirm);
//	bottomLayout->addWidget(confirmBtn);
//	bottomLayout->addStretch();
//	mainLayout->addLayout(bottomLayout);
//}
//
//ConfirmDialog::~ConfirmDialog() {}

TipsDialog::TipsDialog(const QString &info, QWidget *parent)
	: MoveDialog(parent)
{
	setFixedSize(376,276);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	//setWindowModality(Qt::WindowModal);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 376, 276);	

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(38,29,38,82);
	mainLayout->setSpacing(0);

	//QWidget *topWidget = new QWidget(this);
	//topWidget->setFixedHeight(40);
	//topWidget->setObjectName("cs_top");
	//topWidget->setStyleSheet(CS_BASE_DIALOG_TOP_BG);
	//mainLayout->addWidget(topWidget);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setContentsMargins(17,8,20,10);
	mainLayout->addLayout(horLayout);
	QLabel *title = new QLabel(QString(tr("confirm_tips")), this);
	title->setStyleSheet(CS_BASE_DIALOG_TITLE);
	horLayout->addWidget(title);
	horLayout->addStretch();

	auto on_close = [&] {
		done(DialogCode::Rejected);
		close();
	};

	QPushButton *closeBtn = new QPushButton(this);
	closeBtn->setFixedSize(16,16);
	closeBtn->setStyleSheet(CS_BASE_DIALOG_CLOSE);
	connect(closeBtn, &QPushButton::clicked, this, on_close);
	horLayout->addWidget(closeBtn);

	mainLayout->addStretch();

	QVBoxLayout *contentLayout = new QVBoxLayout();
	contentLayout->setContentsMargins(30,0,30,0);
	contentLayout->setSpacing(10);
	mainLayout->addLayout(contentLayout);

	QLabel *label2 = new QLabel(info, this);
	label2->setMaximumWidth(240);
	label2->setWordWrap(true);
	label2->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(label2);

	mainLayout->addStretch();
}

TipsDialog::~TipsDialog() {}

CustomerServiceBaseDialog::CustomerServiceBaseDialog(bool bConfirm, const QString &strTitle, QWidget *parent)
	: MoveDialog(parent)
	, m_pCancelBtn(nullptr)
	, m_pConfirmBtn(nullptr)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setWindowModality(Qt::WindowModal);
	setAttribute(Qt::WA_TranslucentBackground);

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(38,29,38,71);
	mainLayout->setSpacing(0);

	//QWidget *topWidget = new QWidget(this);
	//topWidget->setFixedHeight(40);
	//topWidget->setObjectName("cs_top");
	//topWidget->setStyleSheet(CS_BASE_DIALOG_TOP_BG);
	//mainLayout->addWidget(topWidget);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setContentsMargins(17,8,20,10);
	mainLayout->addLayout(horLayout);
	QLabel *title = new QLabel(strTitle, this);
	title->setStyleSheet(CS_BASE_DIALOG_TITLE);
	horLayout->addWidget(title);
	horLayout->addStretch();

	auto on_close = [&] {
		done(DialogCode::Rejected);
		close();
	};

	QPushButton *closeBtn = new QPushButton(this);
	closeBtn->setFixedSize(16,16);
	closeBtn->setStyleSheet(CS_BASE_DIALOG_CLOSE);
	connect(closeBtn, &QPushButton::clicked, this, on_close);
	horLayout->addWidget(closeBtn);

	mainLayout->addStretch();

	m_pContentLayout = new QVBoxLayout();
	m_pContentLayout->setContentsMargins(30,0,30,0);
	m_pContentLayout->setSpacing(0);
	mainLayout->addLayout(m_pContentLayout);

	mainLayout->addStretch();

	QHBoxLayout *bottomLayout = new QHBoxLayout();
	bottomLayout->setContentsMargins(0,0,0,0);
	bottomLayout->setSpacing(16);
	bottomLayout->addStretch();

	m_pCancelBtn = new QPushButton(QString(tr("cancel")), this);
	m_pCancelBtn->setStyleSheet("QPushButton {background-color:rgb(255,255,255); border:1px solid #EBEBEF; font-size:14px; color:#222222; border-radius:6px; font-weight: 400; font-family:Microsoft YaHei;}"
			"QPushButton:hover {background-color:rgb(232, 229, 237);}");
	m_pCancelBtn->setFixedSize(86, 32);
	connect(m_pCancelBtn, &QPushButton::clicked, this, on_close);
	bottomLayout->addWidget(m_pCancelBtn);

	auto on_confirm = [&, bConfirm] {
		if (bConfirm) {
			OnConfirmClick();
			done(DialogCode::Accepted);
			close();
			/*if (OnConfirmClick()) {
				TipsDialog dialog(QString(tr("operation_success")), this);
				dialog.exec();
				done(DialogCode::Accepted);
				close();
			} else {
				TipsDialog dialog(QString(tr("operation_fail")), this);
				dialog.exec();
			}*/
		} else {
			done(DialogCode::Accepted);
			close();
		}
	};

	m_pConfirmBtn = new QPushButton(QString(tr("determine")), this);
	m_pConfirmBtn->setStyleSheet("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px; font-size:14px; color:#222222;}"
	"QPushButton::hover{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");
	m_pConfirmBtn->setFixedSize(86, 32);
	connect(m_pConfirmBtn, &QPushButton::clicked, this, on_confirm);
	bottomLayout->addWidget(m_pConfirmBtn);
	bottomLayout->addStretch();
	mainLayout->addLayout(bottomLayout);
}

CustomerServiceBaseDialog::~CustomerServiceBaseDialog() {

}

FinishLiveDialog::FinishLiveDialog(QWidget *parent) : CustomerServiceBaseDialog(false, QString(tr("finish_live")), parent) {
	setFixedSize(456, 320);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 456, 320);
	QLabel *label1 = new QLabel(QString(tr("finish_title")), this);
	label1->setStyleSheet(CS_BASE_DIALOG_TITLE);
	m_pContentLayout->addWidget(label1);

	m_pContentLayout->addSpacing(10);

	QLabel *label2 = new QLabel(QString(tr("finish_content")), this);
	label2->setStyleSheet(FINISH_DIALOG_LABEL);
	label2->setWordWrap(true);
	m_pContentLayout->addWidget(label2);
}

FinishLiveDialog::~FinishLiveDialog() {}

bool FinishLiveDialog::OnConfirmClick() {
	return true;
}

ExitDialog::ExitDialog(QWidget *parent) : CustomerServiceBaseDialog(false, QString(tr("confirm_tips")), parent) {
	setFixedSize(406,291);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 406,291);
	QLabel *label1 = new QLabel(QString(tr("confirm_exit")), this);
	label1->setStyleSheet("QLabel{color:rgb(68, 68, 68);font-size:14px;background-color:transparent;font-family:Microsoft YaHei;}");
	m_pContentLayout->addWidget(label1);
}

ExitDialog::~ExitDialog() {}

bool ExitDialog::OnConfirmClick()
{
	return true;
}

EffectDialog::EffectDialog(const std::string &token, BtnTypes type, QWidget *parent)
	: CustomerServiceBaseDialog(true, type == TYPE_MASK ? QString(tr("choose_mask")) : QString(tr("choose_bkground")), parent)
	, m_type(type)
	, m_nPosType(0)
	, m_pGrid(nullptr)
	, m_pArea(nullptr)
{
	setFixedSize(596, 469);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 596, 469);
	connect(AsynReqManager::GetInstance(),&AsynReqManager::readyData, this, &EffectDialog::OnReadyData);

	if (m_type == TYPE_MASK)
		m_strPath = CustomConfig::GetMaskPath();
	else if (m_type == TYPE_BKGROUND)
		m_strPath = CustomConfig::GetBackgroundPath();

	m_pArea = new QScrollArea(this);
	m_pArea->setFixedSize(442, 230);
	m_pArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pArea->setStyleSheet("QScrollArea {background-color:transparent; border:0px;}");
	m_pArea->verticalScrollBar()->setStyleSheet(MASK_DIALOG_SCROLL);
	QWidget *w = new QWidget(m_pArea);
	w->setStyleSheet("QWidget {background-color:rgb(255, 255, 255);}");
	m_pArea->setWidget(w);
	m_pGrid = new QGridLayout(w);
	m_pGrid->setColumnStretch(4, 1);
	m_pGrid->setContentsMargins(15,15,15,15);
	m_pGrid->setHorizontalSpacing(20);
	m_pGrid->setVerticalSpacing(20);
	m_pContentLayout->addWidget(m_pArea);

	AddNoneItem(0);

	if (m_type == TYPE_MASK)
		GetMaskList(token);
	else if (m_type == TYPE_BKGROUND)
		GetBkgroundList(token);
}

EffectDialog::~EffectDialog() {}

void EffectDialog::AddNoneItem(int index) {
	bool bSelect = false;
	if (m_strPath == "")
		bSelect = true;

	int r = index / 4;
	int c = index % 4;

	NoneStateLabel *noneItme = new NoneStateLabel();
	noneItme->setFixedSize(QSize(88, 88));
	noneItme->setPixmap(QPixmap(":res/images/none.png"));
	noneItme->SetSelect(bSelect);
	connect(noneItme, &NoneStateLabel::click, this, &EffectDialog::OnNoneClicked);
	m_pGrid->addWidget(noneItme, r, c, 1, 1);
}

void EffectDialog::AddItem(int index, const QString &path, int type)
{
	bool bSelect = false;
	MD5 md5(path.toStdString().c_str());
	std::string name = md5.toString();
	std::string strPath = CustomConfig::GetImagePath() + "/" + name;
	if (strcmp(m_strPath.c_str(), strPath.c_str()) == 0)
		bSelect = true;

	int r = index / 4;
	int c = index % 4;

	UrlRadiusLabel *urlItme = new UrlRadiusLabel(type);
	urlItme->setFixedSize(QSize(88, 88));
	urlItme->SetNetPath(path);
	urlItme->SetSelect(bSelect);
	connect(urlItme, &UrlRadiusLabel::click, this, &EffectDialog::OnClicked);
	m_pGrid->addWidget(urlItme, r, c, 1, 1);
}

bool EffectDialog::OnConfirmClick() {
	if (m_type == TYPE_MASK)
		CustomConfig::SetMaskPath(m_strPath);
	else if (m_type == TYPE_BKGROUND)
		CustomConfig::SetBackgroundPath(m_strPath);

	emit ChangeFilterPath(m_nPosType, m_strPath);
	return true;
}

void EffectDialog::GetMaskList(const std::string &token) {
	STReqData req;
	req.func = FUNC_GET_MASK_LIST;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(token.c_str())));
	sprintf(req.url, "%s/shop/live/warehouse/list?page=%d&size=%d&fileType=%d", BASE_URL.c_str(), 1, 20, 10);

	AsynReqManager::GetInstance()->InsertReq(req);
}

void EffectDialog::GetBkgroundList(const std::string &token) {
	STReqData req;
	req.func = FUNC_GET_BACKGROUND_LIST;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(token.c_str())));
	sprintf(req.url, "%s/shop/live/warehouse/list?page=%d&size=%d&fileType=%d", BASE_URL.c_str(), 1, 20, 20);

	AsynReqManager::GetInstance()->InsertReq(req);
}

void EffectDialog::OnClicked(UrlRadiusLabel *label) {
	int count = m_pGrid->count();
	for (int i = 0; i < count; i++)
	{
	    QLayoutItem *pLayoutItem = (QLayoutItem *)m_pGrid->itemAt(i);
	    if (pLayoutItem) {
		NoneStateLabel *nonelabel = qobject_cast<NoneStateLabel *>(pLayoutItem->widget());
		if (nonelabel)
			nonelabel->SetSelect(false);
		UrlRadiusLabel *urlLabel = qobject_cast<UrlRadiusLabel *>(pLayoutItem->widget());
		if (urlLabel)
			urlLabel->SetSelect(false);
	    }
	}
	if (label) {
		label->SetSelect(true);
		m_strPath = label->GetMd5NetPath().toStdString();
		int type = label->GetType();
		if (type == 11) {
			m_nPosType = 0;
		} else if(type == 12) {
			m_nPosType = 1;
		} else if(type == 13) {
			m_nPosType = 2;
		} else if(type == 14) {
			m_nPosType = 0;
		} else if(type == 15) {
			m_nPosType = 0;
		} else if(type == 16) {
			m_nPosType = 3;
		}
	}
}

void EffectDialog::OnNoneClicked(NoneStateLabel *label) {
	int count = m_pGrid->count();
	for (int i = 0; i < count; i++)
	{
	    QLayoutItem *pLayoutItem = (QLayoutItem *)m_pGrid->itemAt(i);
	    if (pLayoutItem) {
		NoneStateLabel *nonelabel = qobject_cast<NoneStateLabel *>(pLayoutItem->widget());
		if (nonelabel)
			nonelabel->SetSelect(false);
		UrlRadiusLabel *urlLabel = qobject_cast<UrlRadiusLabel *>(pLayoutItem->widget());
		if (urlLabel)
			urlLabel->SetSelect(false);
	    }
	}
	if (label) {
		label->SetSelect(true);
		m_strPath = "";
		m_nPosType = 0;
	}
}

//void EffectDialog::OnItemClicked(QListWidgetItem *item) {
//	int index = m_pList->row(item);
//	int count = m_pList->count();
//	NoneStateLabel *nonelabel = qobject_cast<NoneStateLabel*>(m_pList->itemWidget(m_pList->item(0)));
//	nonelabel->SetSelect(false);
//	for (int i = 1; i < count; i++) {
//		UrlRadiusLabel *urlLabel = qobject_cast<UrlRadiusLabel*>(m_pList->itemWidget(m_pList->item(i)));
//		urlLabel->SetSelect(false);
//	}
//
//	if (index == 0) {
//		m_strPath = "";
//		nonelabel->SetSelect(true);
//		m_nPosType = 0;
//	} else {
//		UrlRadiusLabel *curLabel =qobject_cast<UrlRadiusLabel*>(m_pList->itemWidget(item));
//		curLabel->SetSelect(true);
//		m_strPath = curLabel->GetMd5NetPath().toStdString();
//		int type = curLabel->GetType();
//		if (type == 11) {
//			m_nPosType = 0;
//		} else if(type == 12) {
//			m_nPosType = 1;
//		} else if(type == 13) {
//			m_nPosType = 2;
//		} else if(type == 14) {
//			m_nPosType = 0;
//		} else if(type == 15) {
//			m_nPosType = 0;
//		} else if(type == 16) {
//			m_nPosType = 3;
//		}
//	}
//}

void EffectDialog::OnReadyData(const STRspData &stRsp) {
	switch (stRsp.func) {
		case FUNC_GET_MASK_LIST: {
			std::vector<STWareHouseData> vcmask = ParseWarehouseList(stRsp.recvBytes);
			int count = vcmask.size();
			for (int i = 0; i < count; i++) {
				AddItem(i + 1, QString::fromStdString(vcmask[i].fileAddress), vcmask[i].site);
			}
			int r = ((count + 1) / 4) + 1;
			int h = 30 + r * 88 + (r - 1) * 20;
			int w = h > 230 ? 434 : 442;
			m_pArea->widget()->resize(QSize(w, h));
			break;
		}
		case FUNC_GET_BACKGROUND_LIST: {
			std::vector<STWareHouseData> vcbackground = ParseWarehouseList(stRsp.recvBytes);
			int count = vcbackground.size();
			for (int i = 0; i < count; i++) {
				AddItem(i + 1, QString::fromStdString(vcbackground[i].fileAddress), vcbackground[i].site);
			}
			int r = ((count + 1) / 4) + 1;
			int h = 30 + r * 88 + (r - 1) * 20;
			int w = h > 230 ? 434 : 442;
			m_pArea->widget()->resize(QSize(w, h));
			break;
		}
		break;
	}
}

MaskDialog::MaskDialog(const std::string &token, QWidget *parent)
	: CustomerServiceBaseDialog(true, QString(tr("choose_mask")), parent)
	, m_pGrid(nullptr)
	, m_pArea(nullptr)
{
	m_strPath = CustomConfig::GetMaskPath();

	setFixedSize(596, 469);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 596, 469);

	m_pArea = new QScrollArea(this);
	m_pArea->setFixedSize(442, 230);
	m_pArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pArea->setStyleSheet("QScrollArea {background-color:transparent; border:0px;}");
	m_pArea->verticalScrollBar()->setStyleSheet(MASK_DIALOG_SCROLL);
	QWidget *w = new QWidget(m_pArea);
	w->setStyleSheet("QWidget {background-color:rgb(255, 255, 255);}");
	m_pArea->setWidget(w);
	m_pGrid = new QGridLayout(w);
	m_pGrid->setColumnStretch(4, 1);
	m_pGrid->setContentsMargins(15,15,15,15);
	m_pGrid->setHorizontalSpacing(20);
	m_pGrid->setVerticalSpacing(20);
	m_pContentLayout->addWidget(m_pArea);

	AddNoneItem(0);
	AddMosaicItem(1);

	int count = 31;
	for (int i = 0; i < count; i++) {
		AddItem(i + 1);
	}

	int r1 = ((2 + count + 1) / 4) + 1;
	int hi = 30 + r1 * 88 + (r1 - 1) * 20;
	int wi = hi > 230 ? 434 : 442;
	m_pArea->widget()->resize(QSize(wi, hi));
}

void MaskDialog::AddMosaicItem(int index) {
	bool bSelect = false;
	if (strcmp(m_strPath.c_str(), ":mask/images/mask/mosaic.png") == 0)
		bSelect = true;

	int r = index / 4;
	int c = index % 4;

	MaskFilterLabel *maskItme = new MaskFilterLabel();
	maskItme->setFixedSize(QSize(88, 88));
	maskItme->setPixmap(QPixmap(":mask/images/mask/mosaic.png"));
	maskItme->SetSelect(bSelect);
	maskItme->SetData((Mask_Type)index);
	connect(maskItme, &MaskFilterLabel::click, this, &MaskDialog::OnClicked);
	m_pGrid->addWidget(maskItme, r, c, 1, 1);
}


MaskDialog::~MaskDialog() {}

void MaskDialog::AddNoneItem(int index) {
	bool bSelect = false;
	if (strcmp(m_strPath.c_str(), ":res/images/none.png") == 0)
		bSelect = true;

	int r = index / 4;
	int c = index % 4;

	NoneStateLabel *noneItme = new NoneStateLabel();
	noneItme->setFixedSize(QSize(88, 88));
	noneItme->setPixmap(QPixmap(":res/images/none.png"));
	noneItme->SetSelect(bSelect);
	connect(noneItme, &NoneStateLabel::click, this, &MaskDialog::OnNoneClicked);
	m_pGrid->addWidget(noneItme, r, c, 1, 1);
}

void MaskDialog::AddItem(int index)
{
	bool bSelect = false;

	int r = index / 4;
	int c = index % 4;

	QString rpath="",dir1="",dir2="";
	index--;
	GetData((Mask_Type)index, rpath, dir1, dir2);
	if (strcmp(rpath.toStdString().c_str(), m_strPath.c_str()) == 0)
		bSelect = true;

	MaskFilterLabel *maskItme = new MaskFilterLabel();
	maskItme->setFixedSize(QSize(88, 88));
	maskItme->setPixmap(QPixmap(rpath));
	maskItme->SetSelect(bSelect);
	maskItme->SetData((Mask_Type)index);
	connect(maskItme, &MaskFilterLabel::click, this, &MaskDialog::OnClicked);
	m_pGrid->addWidget(maskItme, r, c, 1, 1);
}

void MaskDialog::GetData(Mask_Type type, QString &rpath, QString &img_dir1,
			 QString &img_dir2)
{
	QString progpath = QCoreApplication::instance()->applicationDirPath();
	switch (type) {
	case Mask_Mosaic:
		rpath = ":mask/images/mask/mosaic.png";
		img_dir1 = ":mask/images/mask/mosaic.png";
		break;
	case Mask_Toushi_0:
		rpath = ":mask/images/mask/toushi_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/toushi/hong";
		break;
	case Mask_Toushi_1:
		rpath = ":mask/images/mask/toushi_1.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/toushi/zi";
		break;
	case Mask_Toushi_2:
		rpath = ":mask/images/mask/toushi_2.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/toushi/hei";
		break;
	case Mask_Tousha_0:
		rpath = ":mask/images/mask/tousha_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/tousha";
		break;
	case Mask_Patch_0:
		rpath = ":mask/images/mask/patch_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/bianfu";
		break;
	case Mask_Patch_1:
		rpath = ":mask/images/mask/patch_1.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/huli";
		break;
	case Mask_Patch_2:
		rpath = ":mask/images/mask/patch_2.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/nangua";
		break;
	case Mask_Patch_3:
		rpath = ":mask/images/mask/patch_3.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/qingwa";
		break;
	case Mask_Patch_4:
		rpath = ":mask/images/mask/patch_4.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/zhu";
		break;
	case Mask_Patch_5:
		rpath = ":mask/images/mask/patch_5.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/xiongmao";
		break;
	case Mask_Patch_6:
		rpath = ":mask/images/mask/patch_6.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/mao";
		break;
	case Mask_Patch_7:
		rpath = ":mask/images/mask/patch_7.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/dujiaoshou";
		break;
	case Mask_Patch_8:
		rpath = ":mask/images/mask/patch_8.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/tuzi";
		break;
	case Mask_Patch_9:
		rpath = ":mask/images/mask/patch_9.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanzhao/xiongmao1";
		break;
	case Mask_Glasses_0:
		rpath = ":mask/images/mask/glasses_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanjinger/hei";
		break;
	case Mask_Glasses_1:
		rpath = ":mask/images/mask/glasses_1.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanjinger/honghei";
		break;
	case Mask_Nose_0:
		rpath = ":mask/images/mask/nose_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/bizi";
		break;
	case Mask_Combination_0:
		rpath = ":mask/images/mask/combination_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/yanjingbizi";
		break;
	case Mask_Mask_0:
		rpath = ":mask/images/mask/mask_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/quanlian/xiong";
		break;
	case Mask_Mask_1:
		rpath = ":mask/images/mask/mask_1.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/banlian";
		break;
	case Mask_Mask_2:
		rpath = ":mask/images/mask/mask_2.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/quanlian/guaishou";
		break;
	case Mask_Mask_3:
		rpath = ":mask/images/mask/mask_3.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/quanlian/xiongmao";
		break;
	case Mask_Mask_4:
		rpath = ":mask/images/mask/mask_4.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/quanlian/zhizhuxia";
		break;
	case Mask_Animation_0:
		rpath = ":mask/images/mask/animation_0.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/0/erduo";
		img_dir2 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/0/huzi";
		break;
	case Mask_Animation_1:
		rpath = ":mask/images/mask/animation_1.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/1";
		break;
	case Mask_Animation_2:
		rpath = ":mask/images/mask/animation_2.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/2/cao";
		img_dir2 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/2/gua";
		break;
	case Mask_Animation_3:
		rpath = ":mask/images/mask/animation_3.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/3/erduo";
		img_dir2 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/3/hongyun";
		break;
	case Mask_Animation_4:
		rpath = ":mask/images/mask/animation_4.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/4/hudie";
		img_dir2 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/4/lian";
		break;
	case Mask_Animation_5:
		rpath = ":mask/images/mask/animation_5.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/5";
		break;
	case Mask_Animation_6:
		rpath = ":mask/images/mask/animation_6.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/6/meiqiu";
		img_dir2 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/6/lian";
		break;
	case Mask_Animation_7:
		rpath = ":mask/images/mask/animation_7.png";
		img_dir1 = progpath + "/../../data/obs-plugins/obs-filters/LUTs/mask/7";
		break;
	default:
		break;
	}
}

bool MaskDialog::OnConfirmClick() {
	CustomConfig::SetMaskPath(m_strPath);
	emit ChangeMaskPath(m_dir1.toStdString(), m_dir2.toStdString());
	return true;
}

void MaskDialog::OnClicked(MaskFilterLabel *label) {
	int count = m_pGrid->count();
	for (int i = 0; i < count; i++)
	{
	    QLayoutItem *pLayoutItem = (QLayoutItem *)m_pGrid->itemAt(i);
	    if (pLayoutItem) {
		NoneStateLabel *nonelabel = qobject_cast<NoneStateLabel *>(pLayoutItem->widget());
		if (nonelabel)
			nonelabel->SetSelect(false);
		MaskFilterLabel *urlLabel = qobject_cast<MaskFilterLabel *>(pLayoutItem->widget());
		if (urlLabel)
			urlLabel->SetSelect(false);
	    }
	}
	if (label) {
		label->SetSelect(true);
		Mask_Type type = label->GetData();
		QString rpath = "";
		GetData(type, rpath, m_dir1, m_dir2);
		m_strPath = rpath.toStdString();
	}
}

void MaskDialog::OnNoneClicked(NoneStateLabel *label) {
	bool bSelect = false;
	if (m_index == -2)
		bSelect = true;
	int count = m_pGrid->count();
	for (int i = 0; i < count; i++)
	{
	    QLayoutItem *pLayoutItem = (QLayoutItem *)m_pGrid->itemAt(i);
	    if (pLayoutItem) {
		NoneStateLabel *nonelabel = qobject_cast<NoneStateLabel *>(pLayoutItem->widget());
		if (nonelabel)
			nonelabel->SetSelect(false);
		MaskFilterLabel *urlLabel = qobject_cast<MaskFilterLabel *>(pLayoutItem->widget());
		if (urlLabel)
			urlLabel->SetSelect(false);
	    }
	}
	if (label) {
		label->SetSelect(true);
		m_strPath = ":res/images/none.png";
		m_dir1 = "";
		m_dir2 = "";
	}
}

MaskDialog3D::MaskDialog3D(int idx, QWidget *parent)
	: CustomerServiceBaseDialog(true, QString(tr("choose_mask")), parent)
{
	m_index = idx;
	setFixedSize(596, 469);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 596, 469);

	m_pArea = new QScrollArea(this);
	m_pArea->setFixedSize(442, 230);
	m_pArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pArea->setStyleSheet("QScrollArea {background-color:transparent; border:0px;}");
	m_pArea->verticalScrollBar()->setStyleSheet(MASK_DIALOG_SCROLL);
	QWidget *w = new QWidget(m_pArea);
	w->setStyleSheet("QWidget {background-color:rgb(255, 255, 255);}");
	m_pArea->setWidget(w);
	m_pGrid = new QGridLayout(w);
	m_pGrid->setColumnStretch(4, 1);
	m_pGrid->setContentsMargins(15,15,15,15);
	m_pGrid->setHorizontalSpacing(20);
	m_pGrid->setVerticalSpacing(20);
	m_pContentLayout->addWidget(m_pArea);

	AddNoneItem(0);
	for (int i = 1; i < 47; i++) {
		AddItem(i);
	}
}

MaskDialog3D::~MaskDialog3D() {}

bool MaskDialog3D::OnConfirmClick() {
	//CustomConfig::SetMask3DType(m_index);
	obs_source_send_model_index(m_index);
	emit ChangeIndex(m_index);
	return true;
}

void MaskDialog3D::AddNoneItem(int index) {
	bool bSelect = false;
	if (m_index == -2)
		bSelect = true;

	int r = index / 4;
	int c = index % 4;

	NoneStateLabel *noneItme = new NoneStateLabel();
	noneItme->setFixedSize(QSize(88, 88));
	noneItme->setPixmap(QPixmap(":res/images/none.png"));
	noneItme->SetSelect(bSelect);
	connect(noneItme, &NoneStateLabel::click, this, &MaskDialog3D::OnNoneClicked);
	m_pGrid->addWidget(noneItme, r, c, 1, 1);

	int r1 = ((index + 1) / 4) + 1;
	int h = 30 + r1 * 88 + (r1 - 1) * 20;
	int w = h > 230 ? 434 : 442;
	m_pArea->widget()->resize(QSize(w, h));
}

void MaskDialog3D::AddItem(int index)
{
	QString path;
	int type = index - 1;
	GetData((Mask3d_Type)type, path);

	bool bSelect = false;
	if (m_index == type)
		bSelect = true;

	int r = index / 4;
	int c = index % 4;

	UrlRadiusLabel *urlItme = new UrlRadiusLabel((int)type);
	urlItme->setFixedSize(QSize(88, 88));
	urlItme->setPixmap(QPixmap(path));
	urlItme->SetSelect(bSelect);
	connect(urlItme, &UrlRadiusLabel::click, this, &MaskDialog3D::OnClicked);
	m_pGrid->addWidget(urlItme, r, c, 1, 1);

	int r1 = ((index + 1) / 4) + 1;
	int h = 30 + r1 * 88 + (r1 - 1) * 20;
	int w = h > 230 ? 434 : 442;
	m_pArea->widget()->resize(QSize(w, h));
}

void MaskDialog3D::GetData(Mask3d_Type type, QString &path)
{
	QString progpath = QCoreApplication::instance()->applicationDirPath();

	switch (type) {
	case Mask3d_Kzyj:
		path = QString(":mask3d/images/mask3d/kouzhaoyanjing.png");
		break;
	case Mask3d_Ssds:
		path = QString(":mask3d/images/mask3d/shishangdashu.png");
		break;
	case Mask3d_Hdj:
		path = QString(":mask3d/images/mask3d/hudiejie.png");
		break;
	case Mask3d_Hlmj:
		path = QString(":mask3d/images/mask3d/hulimianju.png");
		break;
	case Mask3d_Ms:
		path = QString(":mask3d/images/mask3d/miansha.png");
		break;
	case Mask3d_Shtmz:
		path = QString(":mask3d/images/mask3d/shanghaitanmaozi.png");
		break;
	case Mask3d_Tzej:
		path = QString(":mask3d/images/mask3d/tuzierji.png");
		break;
	case Mask3d_Mymz:
		path = QString(":mask3d/images/mask3d/mingyuanmaozi.png");
		break;
	case Mask3d_Gdej:
		path = QString(":mask3d/images/mask3d/guodongerji.png");
		break;
	case Mask3d_Hrx:
		path = QString(":mask3d/images/mask3d/huanraoxin.png");
		break;
	case Mask3d_Hrt:
		path = QString(":mask3d/images/mask3d/huanraotu.png");
		break;
	case Mask3d_Qqx:
		path = QString(":mask3d/images/mask3d/qiqiuxin.png");
		break;
	case Mask3d_Hx:
		path = QString(":mask3d/images/mask3d/hongxin.png");
		break;
	case Mask3d_Lx:
		path = QString(":mask3d/images/mask3d/lanxin.png");
		break;
	case Mask3d_Mosaic:
		path = ":mask/images/mask/mosaic.png";
		break;
	case Mask3d_Toushi_0:
		path = ":mask/images/mask/toushi_0.png";
		break;
	case Mask3d_Toushi_1:
		path = ":mask/images/mask/toushi_1.png";
		break;
	case Mask3d_Toushi_2:
		path = ":mask/images/mask/toushi_2.png";
		break;
	case Mask3d_Tousha_0:
		path = ":mask/images/mask/tousha_0.png";
		break;
	case Mask3d_Patch_0:
		path = ":mask/images/mask/patch_0.png";
		break;
	case Mask3d_Patch_1:
		path = ":mask/images/mask/patch_1.png";
		break;
	case Mask3d_Patch_2:
		path = ":mask/images/mask/patch_2.png";
		break;
	case Mask3d_Patch_3:
		path = ":mask/images/mask/patch_3.png";
		break;
	case Mask3d_Patch_4:
		path = ":mask/images/mask/patch_4.png";
		break;
	case Mask3d_Patch_5:
		path = ":mask/images/mask/patch_5.png";
		break;
	case Mask3d_Patch_6:
		path = ":mask/images/mask/patch_6.png";
		break;
	case Mask3d_Patch_7:
		path = ":mask/images/mask/patch_7.png";
		break;
	case Mask3d_Patch_8:
		path = ":mask/images/mask/patch_8.png";
		break;
	case Mask3d_Patch_9:
		path = ":mask/images/mask/patch_9.png";
		break;
	case Mask3d_Glasses_0:
		path = ":mask/images/mask/glasses_0.png";
		break;
	case Mask3d_Glasses_1:
		path = ":mask/images/mask/glasses_1.png";
		break;
	case Mask3d_Nose_0:
		path = ":mask/images/mask/nose_0.png";
		break;
	case Mask3d_Combination_0:
		path = ":mask/images/mask/combination_0.png";
		break;
	case Mask3d_Mask3d_0:
		path = ":mask/images/mask/mask_0.png";
		break;
	case Mask3d_Mask3d_1:
		path = ":mask/images/mask/mask_1.png";
		break;
	case Mask3d_Mask3d_2:
		path = ":mask/images/mask/mask_2.png";
		break;
	case Mask3d_Mask3d_3:
		path = ":mask/images/mask/mask_3.png";
		break;
	case Mask3d_Mask3d_4:
		path = ":mask/images/mask/mask_4.png";
		break;
	case Mask3d_Animation_0:
		path = ":mask/images/mask/animation_0.png";
		break;
	case Mask3d_Animation_1:
		path = ":mask/images/mask/animation_1.png";
		break;
	case Mask3d_Animation_2:
		path = ":mask/images/mask/animation_2.png";
		break;
	case Mask3d_Animation_3:
		path = ":mask/images/mask/animation_3.png";
		break;
	case Mask3d_Animation_4:
		path = ":mask/images/mask/animation_4.png";
		break;
	case Mask3d_Animation_5:
		path = ":mask/images/mask/animation_5.png";
		break;
	case Mask3d_Animation_6:
		path = ":mask/images/mask/animation_6.png";
		break;
	case Mask3d_Animation_7:
		path = ":mask/images/mask/animation_7.png";
		break;
	default:
		break;
	}
}

void MaskDialog3D::OnNoneClicked(NoneStateLabel *label) {
	int count = m_pGrid->count();
	for (int i = 0; i < count; i++)
	{
	    QLayoutItem *pLayoutItem = (QLayoutItem *)m_pGrid->itemAt(i);
	    if (pLayoutItem) {
		NoneStateLabel *nonelabel = qobject_cast<NoneStateLabel *>(pLayoutItem->widget());
		if (nonelabel)
			nonelabel->SetSelect(false);
		UrlRadiusLabel *urlLabel = qobject_cast<UrlRadiusLabel *>(pLayoutItem->widget());
		if (urlLabel)
			urlLabel->SetSelect(false);
	    }
	}
	if (label) {
		label->SetSelect(true);
		m_index = -2;
	}
}

void MaskDialog3D::OnClicked(UrlRadiusLabel *label) {
	int count = m_pGrid->count();
	for (int i = 0; i < count; i++)
	{
	    QLayoutItem *pLayoutItem = (QLayoutItem *)m_pGrid->itemAt(i);
	    if (pLayoutItem) {
		NoneStateLabel *nonelabel = qobject_cast<NoneStateLabel *>(pLayoutItem->widget());
		if (nonelabel)
			nonelabel->SetSelect(false);
		UrlRadiusLabel *urlLabel = qobject_cast<UrlRadiusLabel *>(pLayoutItem->widget());
		if (urlLabel)
			urlLabel->SetSelect(false);
	    }
	}
	if (label) {
		label->SetSelect(true);
		m_index = label->GetType();
	}
}

DeviceDialog::DeviceDialog(obs_source_t *camera_source, obs_source_t* mic_source, QWidget *parent)
	: CustomerServiceBaseDialog(true, QString(tr("device_set")), parent)
	, m_pCameraCombo(nullptr)
	, m_pMicroCombo(nullptr)
	, m_cameraProperties(nullptr, obs_properties_destroy)
	, m_microProperties(nullptr, obs_properties_destroy)
	, m_pCameraSource(camera_source)
	, m_pMicSource(mic_source)
{
	setFixedSize(456,340);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 456, 340);	
	m_pContentLayout->setContentsMargins(74,8,72,8);
	m_pContentLayout->setSpacing(8);

	QHBoxLayout *horlayout1 = new QHBoxLayout();
	horlayout1->setContentsMargins(0,0,0,0);
	horlayout1->setSpacing(5);
	m_pContentLayout->addLayout(horlayout1);

	QLabel *label1 = new QLabel(QString(tr("camera")), this);
	label1->setStyleSheet(DEVICE_LABEL);
	horlayout1->addWidget(label1);

	m_pCameraCombo = new QComboBox(this);
	m_pCameraCombo->setFixedSize(174, 36);
	m_pCameraCombo->setStyleSheet(CAMERA_COMBOBOX);
	m_pCameraCombo->setView(new QListView());
	connect(m_pCameraCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(OnCameraIndexChanged(int)));
	horlayout1->addWidget(m_pCameraCombo);

	QHBoxLayout *horlayout2 = new QHBoxLayout();
	horlayout2->setContentsMargins(0,0,0,0);
	horlayout2->setSpacing(5);
	m_pContentLayout->addLayout(horlayout2);

	QLabel *label2 = new QLabel(QString(tr("microphone")), this);
	label2->setStyleSheet(DEVICE_LABEL);
	horlayout2->addWidget(label2);

	m_pMicroCombo = new QComboBox(this);
	m_pMicroCombo->setFixedSize(174, 36);
	m_pMicroCombo->setStyleSheet(CAMERA_COMBOBOX);
	m_pMicroCombo->setView(new QListView());
	horlayout2->addWidget(m_pMicroCombo);

	InitCamera(m_pCameraSource);
	InitMicro(m_pMicSource);
}

DeviceDialog::~DeviceDialog() {
	killTimer(m_nTimerId);
}

void DeviceDialog::InitCamera(obs_source_t *source) {
	if (source) {
		OBSData settings = obs_source_get_settings(source);
		m_cameraProperties.reset(obs_source_properties(source));
		obs_property_t *property = obs_properties_first(m_cameraProperties.get());
		while (property) {
			obs_property_type type = obs_property_get_type(property);
			if (!obs_property_visible(property))
				break;

			if (type == OBS_PROPERTY_LIST) {
				const char *name = obs_property_name(property);
				obs_combo_format format = obs_property_list_format(property);
				if (strcmp(name, "video_device_id") == 0) {
					size_t count = obs_property_list_item_count(property);
					for (size_t i = 0; i < count; i++) {
						const char *name = obs_property_list_item_name(property, i);
						QVariant var;
						if (format == OBS_COMBO_FORMAT_STRING) {
							var = QByteArray(obs_property_list_item_string(property, i));
						}

						m_pCameraCombo->addItem(QString::fromUtf8(name), var);
					}

					std::string value = obs_data_get_string(settings, name);
					int idx = m_pCameraCombo->findData(QByteArray(value.c_str()));
					if (idx != -1)
						m_pCameraCombo->setCurrentIndex(idx);
				}
			}
			obs_property_next(&property);
		}
		obs_data_release(settings);
	}
}

void DeviceDialog::InitMicro(obs_source_t *source) {
	if (source) {
		OBSData settings = obs_source_get_settings(source);
		m_microProperties.reset(obs_source_properties(source));
		obs_property_t *property = obs_properties_first(m_microProperties.get());
		while (property) {
			obs_property_type type = obs_property_get_type(property);
			if (!obs_property_visible(property))
				break;

			if (type == OBS_PROPERTY_LIST) {
				const char *name = obs_property_name(property);
				obs_combo_format format = obs_property_list_format(property);
				if (strcmp(name, "device_id") == 0) {
					size_t count = obs_property_list_item_count(property);
					for (size_t i = 0; i < count; i++) {
						const char *name = obs_property_list_item_name(property, i);
						QVariant var;
						if (format == OBS_COMBO_FORMAT_STRING) {
							var = QByteArray(obs_property_list_item_string(property, i));
						}

						m_pMicroCombo->addItem(QString::fromUtf8(name), var);
					}

					std::string value = obs_data_get_string(settings, name);
					int idx = m_pMicroCombo->findData(QByteArray(value.c_str()));
					if (idx != -1)
						m_pMicroCombo->setCurrentIndex(idx);
				}
			}
			obs_property_next(&property);
		}
		obs_data_release(settings);
	}
}

bool DeviceDialog::OnConfirmClick() {
	{
		OBSData settings = obs_source_get_settings(m_pCameraSource);
		QVariant var = m_pCameraCombo->itemData(m_pCameraCombo->currentIndex());
		obs_data_set_string(settings, "video_device_id", var.toByteArray().constData());
		obs_source_update(m_pCameraSource, settings);
	}

	{
		OBSData settings = obs_source_get_settings(m_pMicSource);
		QVariant var = m_pMicroCombo->itemData(m_pMicroCombo->currentIndex());
		obs_data_set_string(settings, "device_id", var.toByteArray().constData());
		obs_source_update(m_pMicSource, settings);
	}
	return true;
}

void DeviceDialog::OnCameraIndexChanged(int index) {
	killTimer(m_nTimerId);
	m_nTimerId = startTimer(100);
}

void DeviceDialog::timerEvent(QTimerEvent *event) {
	if(event->timerId() == m_nTimerId){  
		 QVariant var = m_pCameraCombo->currentData();
		 std::string strName = var.toByteArray().constData();
		 bool bEnable = false;
		 bool result = SignalProxyModel::GetInstance()->GetVedioSource("dshow_input", strName, bEnable);
		 if (result) {
			 killTimer(m_nTimerId);
			 if (!bEnable)
				MsgBox::information(this, StringConvert::GetInstance()->GetString(STR_PROCESS), StringConvert::GetInstance()->GetString(STR_CAMERA_USED));
		 }
	} 
}

QualityDialog::QualityDialog(config_t *config, QWidget *parent)
	: CustomerServiceBaseDialog(true, QString(tr("quality_set")), parent)
	, m_pQualityGroup(nullptr)
	, m_pConfig(config)
{
	setFixedSize(456, 290);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 456, 290);	
	m_pContentLayout->setContentsMargins(74,8,0,8);
	m_pContentLayout->setSpacing(10);

	m_pQualityGroup = new QButtonGroup(this);
	m_pQualityGroup->setExclusive(true);
	QStringList list;
	//list << QString(tr("high_definition")) << QString(tr("standard_definition"));
	list << QString(tr("high_definition"));
	for (int i = 0; i < list.count(); i++) {
		QRadioButton *radio = new QRadioButton(list[i], this);
		radio->setFixedHeight(22);
		radio->setStyleSheet(SETTING_RADIO_BUTTON);

		m_pQualityGroup->addButton(radio, QUALITY_HIGH + i);
		m_pContentLayout->addWidget(radio);
	}

	uint32_t out_cx = config_get_uint(m_pConfig, "Video", "OutputCX");
	uint32_t out_cy = config_get_uint(m_pConfig, "Video", "OutputCY");
	std::string outRes = ResString(out_cx, out_cy);
	m_pQualityGroup->button(QUALITY_HIGH)->setChecked(true);
	//if (_strcmpi(outRes.c_str(), "1920x1080") == 0) {
	//	qobject_cast<QRadioButton *>(m_pQualityGroup->button(QUALITY_HIGH))->setChecked(true);
	//} else if (_strcmpi(outRes.c_str(), "1280x720") == 0) {
	//	qobject_cast<QRadioButton *>(m_pQualityGroup->button(QUALITY_STANDARD))->setChecked(true);
	//}
}

QualityDialog::~QualityDialog() {}

std::string QualityDialog::GetOutputResolut()
{
	if (m_pQualityGroup->checkedId() == (int)QUALITY_HIGH)
		return "1920x1080";
	else if (m_pQualityGroup->checkedId() == (int)QUALITY_STANDARD)
		return "1280x720";

	return std::string();
}

bool QualityDialog::OnConfirmClick() {
	return true;
}

NotificationButton::NotificationButton(const QString &strtext, QWidget *parent) : QPushButton(parent) {
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(43,0,43,0);
	mainLayout->setSpacing(0);
	
	QLabel *text = new QLabel(strtext, this);
	text->setAlignment(Qt::AlignCenter);
	text->setStyleSheet("QLabel {color:rgb(34, 34, 34);font-size:14px; background-color:transparent; font-family:Microsoft YaHei;}");
	mainLayout->addWidget(text);

	m_pCount = new QLabel(this);
	m_pCount->setAlignment(Qt::AlignCenter);
	m_pCount->setStyleSheet("QLabel {background:red; border-radius:7px; font-size:10px; color:rgb(255,255,255);font-family:Microsoft YaHei;}");
	m_pCount->resize(0,0);
}
NotificationButton::~NotificationButton() {}

void NotificationButton::SetNoteCount(int count) {
	if (count <= 0) {
		m_pCount->setText(QString::number(count));
		m_pCount->resize(0, 0);
		m_pCount->setGeometry(0, 0, 0, 0);
	} else if (count > 0 && count < 10) {
		m_pCount->setText(QString::number(count));
		m_pCount->resize(14, 14);
		m_pCount->setGeometry(79, 7, 14, 14);
	} else if (count >= 10 && count < 100) {
		m_pCount->setText(QString::number(count));	
		m_pCount->resize(20, 14);
		m_pCount->setGeometry(79, 7, 20, 14);
	} else {
		m_pCount->setText(QString::number(99) + QString("+"));
		m_pCount->resize(26, 14);
		m_pCount->setGeometry(79, 7, 26, 14);
	}
}

NoteButtonGroup::NoteButtonGroup(QWidget *parent)
	: QWidget(parent)
	, m_curIndex(0)
	, m_lineHeight(3)
	, m_lineColor(QColor(255, 230, 16))
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	m_btnGroup = new QButtonGroup(this);
	connect(m_btnGroup, SIGNAL(buttonClicked(int)), this, SLOT(OnButtonClicked(int)));

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, m_lineHeight);
	layout->setSpacing(0);
}

NoteButtonGroup::~NoteButtonGroup()
{

}

void NoteButtonGroup::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setPen(Qt::NoPen);
	painter.setBrush(m_lineColor);
	painter.drawRect(m_curIndex * m_btnSize.width() + 43, this->height() - m_lineHeight, 32, m_lineHeight);
}

void NoteButtonGroup::AddButton(QPushButton* btn, int id)
{
	m_btnGroup->addButton(btn, id);
	layout()->addWidget(btn);
	m_btnSize = btn->size();
}

QPushButton *NoteButtonGroup::GetButton(int id)
{
	return qobject_cast<QPushButton *>(m_btnGroup->button(id));
}

void NoteButtonGroup::OnButtonClicked(int index)
{
	m_btnGroup->button(index)->setChecked(true);
	m_curIndex = index;
	update();

	emit ButtonClicked(index);
}

NoteTypeItem::NoteTypeItem(const STInfoClass &info, QWidget *parent)
	: QWidget(parent)
	, m_infoClass(info)
{
	QHBoxLayout *mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(10,0,0,0);
	mainLayout->setSpacing(10);

	QLabel *text = new QLabel(QString::fromStdString(m_infoClass.className), this);
	text->setStyleSheet("QLabel {color:rgb(68, 68, 68);font-size:14px; background-color:transparent;}");
	text->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	mainLayout->addWidget(text);

	m_pCountLabel = new QLabel(this);
	m_pCountLabel->setAlignment(Qt::AlignCenter);
	m_pCountLabel->setStyleSheet("QLabel {background:red; border:0px; border-radius:7px; font-size:10px; color:rgb(255,255,255);}");
	mainLayout->addWidget(m_pCountLabel);
	mainLayout->addStretch();

	SetNoteCount(m_infoClass.unreadNum);
}

NoteTypeItem::~NoteTypeItem() {}

void NoteTypeItem::SetNoteCount(int count) {
	if (count <= 0) {
		m_pCountLabel->setFixedSize(0, 0);
	} else if (count < 10) {
		m_pCountLabel->setText(QString::number(count));
		m_pCountLabel->setFixedSize(14, 14);
	} else if (count < 100) {
		m_pCountLabel->setText(QString::number(count));	
		m_pCountLabel->setFixedSize(20, 14);
	} else {
		m_pCountLabel->setText(QString::number(99) + QString("+"));
		m_pCountLabel->setFixedSize(26, 14);
	}
}

int NoteTypeItem::GetClassId()
{
	return m_infoClass.classId;
}

NoteTitleWidget::NoteTitleWidget(const STPageData &note, QWidget *parent)
	: QWidget(parent)
	, m_pRadioButton(nullptr)
{
	QHBoxLayout *mainlayout = new QHBoxLayout(this);
	mainlayout->setContentsMargins(15, 7, 20, 9);
	mainlayout->setSpacing(10);

	m_pRadioButton = new QRadioButton(this);
	m_pRadioButton->setFixedSize(17, 17);
	m_pRadioButton->setStyleSheet("QRadioButton {background-color:transparent;}"
			"QRadioButton::indicator {width:17px; height:17px; background-color:transparent;}"
			"QRadioButton::indicator::unchecked {image:url(:/res/images/radio_normal2.png);}"
			"QRadioButton::indicator::unchecked:hover {image:url(:/res/images/radio_hover.png);}"
			"QRadioButton::indicator::unchecked:pressed {image:url(:/res/images/radio_hover.png);}"
			"QRadioButton::indicator::checked {image:url(:/res/images/radio_check_normal.png);}"
			"QRadioButton::indicator::checked:hover {image:url(:/res/images/radio_check_normal.png);}"
			"QRadioButton::indicator::checked:pressed {image:url(:/res/images/radio_check_normal.png);}");
	mainlayout->addWidget(m_pRadioButton);

	QLabel *title = new QLabel(QString::fromStdString(note.title), this);
	title->setFixedHeight(18);
	title->setStyleSheet("QLabel {color:rgb(34, 34, 34);font-size:14px;font-weight:bold; background-color:transparent; font-family:Microsoft YaHei;}");
	mainlayout->addWidget(title);

	mainlayout->addStretch();

	QString qss("QLabel {color:rgb(136, 136, 136);font-size:12px; background-color:transparent; font-family:Microsoft YaHei;}");
	QLabel *name = new QLabel(QString::fromStdString(note.senderName), this);
	name->setStyleSheet(qss);
	mainlayout->addWidget(name);

	auto a = [] (long long timestamp){
		// 后端 大kengbi 大kenghuo caibi
		QDateTime time = QDateTime::fromMSecsSinceEpoch(timestamp);
		return time.toString("yyyy-MM-dd hh:mm:ss");
	};

	long long llsend = (long long)note.sendTime;

	QLabel *time = new QLabel(a(llsend), this);
	time->setStyleSheet(qss);
	mainlayout->addWidget(time);

	/*m_pCheckBox = new QCheckBox(this);
	m_pCheckBox->setFixedSize(14, 14);
	m_pCheckBox->setStyleSheet("QCheckBox {background-color:transparent;}"
			"QCheckBox::indicator{width:14px;height:14px;}"
			"QCheckBox::indicator:checked{border-image:url(:/res/images/arr_top.png);}"
			"QCheckBox::indicator:checked:hover{border-image:url(:/res/images/arr_top_hover.png);}"
			"QCheckBox::indicator:checked:hover{border-image:url(:/res/images/arr_top_hover.png);}"
			"QCheckBox::indicator:unchecked{border-image:url(:/res/images/arr_bottom.png);}"
			"QCheckBox::indicator:unchecked:hover{border-image:url(:/res/images/arr_bottom_hover.png);}"
			"QCheckBox::indicator:unchecked:hover{border-image:url(:/res/images/arr_bottom_hover.png);}");
	mainlayout->addWidget(m_pCheckBox);
	connect(m_pCheckBox, &QCheckBox::clicked, this, &NoteTitleWidget::CheckNote);*/
}

NoteTitleWidget::~NoteTitleWidget() {

}

void NoteTitleWidget::SetCheck(bool check) {
	m_pRadioButton->setChecked(check);
}

bool NoteTitleWidget::GetCheck()
{
	return m_pRadioButton->isChecked();
}

void NoteTitleWidget::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		emit CheckNote(true);
	}
}

NotificationItem::NotificationItem(const STPageData &note, const resize_callback &func, const click_callback &func1, QWidget *parent)
	: QWidget(parent)
	, m_pWidget(nullptr)
	, m_nNoteId(note.id)
{
	setFixedHeight(38);
	QVBoxLayout *mainlayout = new QVBoxLayout(this);
	mainlayout->setContentsMargins(0,0,0,0);
	mainlayout->setSpacing(0);

	m_pWidget = new NoteTitleWidget(note, this);
	mainlayout->addWidget(m_pWidget);

	QWidget *notewidget = new QWidget(this);
	notewidget->setVisible(false);
	mainlayout->addWidget(notewidget);
	QHBoxLayout *noteLayout = new QHBoxLayout(notewidget);
	noteLayout->setContentsMargins(15,10,11,16);
	noteLayout->setSpacing(0);

	QLabel *content = new QLabel(QString::fromStdString(note.content), this);
	content->setStyleSheet("QLabel {color:rgb(68, 68, 68);font-size:12px; background-color:transparent; font-family:Microsoft YaHei;}");
	content->setWordWrap(true);
	content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	noteLayout->addWidget(content);

	QLabel *line = new QLabel(this);
	line->setFixedHeight(1);
	line->setStyleSheet("QLabel {background-color:rgb(238, 238, 238);}");
	mainlayout->addWidget(line);

	auto check = [&, note, notewidget, func, func1](bool checked) {
		if (notewidget) {
			/*notewidget->setVisible(checked);
			setFixedHeight(checked ? 106 : 38);
			if (func != nullptr) {
				func(checked ? 68 : -68);
			}*/
			if (func1 != nullptr) {
				func1(note);
			}
			if (note.readStatus == 0)
				emit ReadNote(m_nNoteId);
		}
	};
	connect(m_pWidget, &NoteTitleWidget::CheckNote, check);
}

NotificationItem::~NotificationItem() {}

void NotificationItem::SetCheck(bool check)
{
	m_pWidget->SetCheck(check);
}

bool NotificationItem::GetCheck()
{
	return m_pWidget->GetCheck();
}

int NotificationItem::GetNoteId()
{
	return m_nNoteId;
}

PageWidget::PageWidget(QWidget *parent)
	: QWidget(parent)
	, m_pPageGroup(nullptr)
	, m_nShowCount(5)
	, m_nAllCount(0)
	, m_nFirstIndex(1)
{
	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(3);
	
	m_pPageGroup = new QButtonGroup(this);
	connect(m_pPageGroup, SIGNAL(buttonClicked(int)), this, SLOT(OnButtonClicked(int)));
}

PageWidget::~PageWidget() {

}

void PageWidget::SetPageCount(int count) {
	DeleteLayout();
	m_nAllCount = count;
	for (int i = 0; i < count; i++) {
		QPushButton *button = new QPushButton(QString::number(i+1), this);
		button->setFixedSize(26,26);
		button->setCheckable(true);
		button->setStyleSheet("QPushButton {font-size:12px; color:rgba(0,0,0,166); border-radius:4px; background-color:transparent;}"
			"QPushButton:hover {border:1px solid rgb(238,238,238);color:rgba(0,0,0);}"
			"QPushButton:pressed {border:1px solid rgb(238,238,238);color:rgba(0,0,0);}"
			"QPushButton::checked {background-color:rgb(255, 219, 0); color:rgba(0,0,0,166);}"
			"QPushButton::checked:hover {background-color:rgb(255, 219, 0); color:rgba(0,0,0);}"
			"QPushButton::checked:pressed {background-color:rgb(255, 219, 0); color:rgba(0,0,0);}");
		this->layout()->addWidget(button);
		m_pPageGroup->addButton(button, i + 1);
		if (i == 0)
			button->setChecked(true);
	}

	if (count > m_nShowCount) {
		for (int i = m_nShowCount; i < count; i++) {
			m_pPageGroup->button(i + 1)->setVisible(false);
		}
	}
	m_nFirstIndex = 1;
}

void PageWidget::SetCurrentCount(int current) {
	if ((current == m_nFirstIndex) && (current != 1)) {
		m_nFirstIndex--;
	} else if ((current == m_nFirstIndex + m_nShowCount - 1) && (current != m_nAllCount)) {
		m_nFirstIndex++;
	} else if ((current > m_nFirstIndex + m_nShowCount - 1) && (current < m_nAllCount)) {
		m_nFirstIndex = current - m_nShowCount + 2;
	} else if (current < m_nFirstIndex && current > 1) {
		m_nFirstIndex = current - 1;
	} else if (current == m_nAllCount) {
		m_nFirstIndex = current - m_nShowCount + 1;
	} else if (current == 1) {
		m_nFirstIndex = 1;
	}

	if (m_pPageGroup) {
		for (int i = 1; i <= m_nAllCount; i++) {
			if (i >= m_nFirstIndex && i <= m_nFirstIndex + m_nShowCount - 1)
				m_pPageGroup->button(i)->setVisible(true);
			else
				m_pPageGroup->button(i)->setVisible(false);
		}
	}
}

void PageWidget::SetCheckIndex(int index) {
	if (m_pPageGroup) {
		m_pPageGroup->button(index)->setChecked(true);
	}
}

void PageWidget::DeleteLayout() {
	if (this->layout()) {
		QLayoutItem *child;
		while ((child = this->layout()->takeAt(0)) != 0) {
		    delete child->widget();
		    delete child;
		    child = nullptr;
		}
	}

	if (m_pPageGroup) {
		QList<QAbstractButton *> buttonlist = m_pPageGroup->buttons();
		for (auto btn : buttonlist) {
			m_pPageGroup->removeButton(btn);
		}
	}
}

void PageWidget::OnButtonClicked(int id) {
	SetCurrentCount(id);
	emit ButtonClicked(id);
}

NotificationDialog::NotificationDialog(const std::string &token, QWidget *parent)
	: CustomerServiceBaseDialog(true, QString(tr("notification_center")), parent)
	, m_pStateGroup(nullptr)
	, m_pPageLabel(nullptr)
	, m_pPageWidget(nullptr)
	, m_pEdit(nullptr)
	, m_pWidget(nullptr)
	, m_strToken(token)
	, m_classid(-1)
	, m_stateType(STATE_UNREAD)
	, m_nSize(10)
	, m_nPage(1)
	, m_nAllPage(0)
{
	setFixedSize(841, 716);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 841, 716);	
	m_pCancelBtn->setVisible(false);
	m_pConfirmBtn->setVisible(false);
	m_pContentLayout->setSpacing(15);
	connect(AsynReqManager::GetInstance(),&AsynReqManager::readyData, this, &NotificationDialog::OnReadyData);

	QHBoxLayout *horlayout = new QHBoxLayout();
	horlayout->setContentsMargins(0,10,0,0);
	horlayout->setSpacing(0);
	m_pContentLayout->addLayout(horlayout);
	horlayout->addStretch();

	m_pStateGroup = new NoteButtonGroup(this);
	QStringList strList;
	strList << QString(tr("unread")) << QString(tr("read")) << QString(tr("all"));
	for (int i = 0; i < 3; i++) {
		NotificationButton *btn = new NotificationButton(strList[i], this);
		btn->setFixedSize(118, 40);
		btn->setStyleSheet("QPushButton {background-color:transparent;}");
		m_pStateGroup->AddButton(btn, STATE_UNREAD + i);
	}
	auto state_click = [&](int id) {
		m_nPage = 1;
		m_stateType = (STATE_TYPE)id;
		GetPageList();
	};
	connect(m_pStateGroup, &NoteButtonGroup::ButtonClicked, state_click);
	horlayout->addWidget(m_pStateGroup);
	horlayout->addStretch();

	QHBoxLayout *horlayou2 = new QHBoxLayout();
	horlayou2->setContentsMargins(0,0,0,0);
	horlayou2->setSpacing(10);
	m_pContentLayout->addLayout(horlayou2);

	m_pListWidget = new QListWidget(this);
	m_pListWidget->setFixedSize(109, 418);
	m_pListWidget->setStyleSheet("QListWidget {border:0px; background-color:transparent;}"
			"QListWidget:focus {outline: none;}"
			"QListWidget:item {background-color:transparent;}"
			"QListWidget:item:hover {border:0px; background-color:rgb(243, 243, 243); border-radius:4px;}"
			"QListWidget:item::selected:active {border:0px; background-color:rgb(243, 243, 243); border-radius:4px;}"
			"QListWidget:item:selected {border:0px; background-color:rgb(243, 243, 243); border-radius:4px;}");
	horlayou2->addWidget(m_pListWidget, 0, Qt::AlignTop);
	connect(m_pListWidget, &QListWidget::itemClicked, this, &NotificationDialog::OnListItemClicked);

	STInfoClass info;
	info.className = QString(tr("all")).toStdString();
	AddListItem(info);
	m_pListWidget->setCurrentRow(0);

	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->setContentsMargins(0,0,0,0);
	rightLayout->setSpacing(15);
	horlayou2->addLayout(rightLayout);

	m_pScrollArea = new QScrollArea(this);
	m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pScrollArea->setFixedSize(580, 418);
	m_pScrollArea->setStyleSheet("QScrollArea {border: 1px solid rgb(238,238,238); background-color:transparent;}");
	m_pScrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {border:none;margin:0px,0px,0px,0px;width:4px; border-radius:2px;background-color:#FFFFFF;}"
			"QScrollBar:vertical:hover {margin: 0px,0px,0px,0px; background-color:#FFFFFF;}"
			"QScrollBar::handle:vertical {max-width:4px; background-color:rgb(187, 187, 187);  height: 40px; border-radius: 2px;}"
			"QScrollBar::add-line:vertical {height:0px;}"
			"QScrollBar::sub-line:vertical {height:0px;}"
			"QScrollBar::add-page:vertical {background:transparent;}"
			"QScrollBar::sub-page:vertical {background:transparent;}");
	rightLayout->addWidget(m_pScrollArea);

	QWidget *scrollWidget = new QWidget(this);
	scrollWidget->setStyleSheet("QWidget {background-color:transparent;}");
	//scrollWidget->setAttribute(Qt::WA_TranslucentBackground);
	m_pScrollArea->setWidget(scrollWidget);
	QVBoxLayout *verlayout = new QVBoxLayout();
	verlayout->setContentsMargins(0,0,0,0);
	verlayout->setSpacing(0);
	scrollWidget->setLayout(verlayout);

	QHBoxLayout *bottomlayout = new QHBoxLayout();
	bottomlayout->setContentsMargins(0,0,0,0);
	bottomlayout->setSpacing(0);
	rightLayout->addLayout(bottomlayout);

	QString qss("QLabel {color:rgb(68, 68, 68);font-size:12px; background-color:transparent;}");
	m_pPageLabel = new QLabel(this);
	m_pPageLabel->setStyleSheet(qss);
	bottomlayout->addWidget(m_pPageLabel);
	bottomlayout->addSpacing(10);

	QComboBox *pageCombo = new QComboBox(this);
	pageCombo->setFixedSize(65, 28);
	pageCombo->setStyleSheet("QComboBox {padding-left:8px; border:1px solid #D3CCDF; border-radius:4px; background-color:transparent;}"
			"QComboBox::drop-down {border:0px; width:32px;}"
			"QComboBox::down-arrow {border-image:url(:/res/images/arr_bottom.png); width:14px; height:14px;}"
			"QComboBox::down-arrow:hover {border-image:url(:/res/images/arr_bottom_hover.png);}"
			"QComboBox QAbstractItemView {border:1px solid #D3CCDF; outline:none;}"
			"QComboBox QAbstractItemView:item {background-color:rgb(255,255,255); color:rgb(85, 85, 85); height:36px;}"
			"QComboBox QAbstractItemView:item:hover {background-color:rgb(249,248,251); color:rgb(85, 85, 85);}");
	pageCombo->setView(new QListView());
	for (int i = 0; i < 5; i++) {
		pageCombo->addItem(QString::number((i + 1) * 10));
	}
	pageCombo->setCurrentIndex(0);
	connect(pageCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(OnPageComboChange(const QString &)));
	bottomlayout->addWidget(pageCombo);
	bottomlayout->addSpacing(10);

	QLabel *label = new QLabel(QString(tr("count")), this);
	label->setStyleSheet(qss);
	bottomlayout->addWidget(label);

	bottomlayout->addSpacing(20);

	auto left_click = [&]{
		int page = m_nPage - 1;
		if (page > 0) {
			m_pPageWidget->SetCurrentCount(page);
			m_nPage = page;
			m_pPageWidget->SetCheckIndex(m_nPage);
			GetPageListLimit();
		}
	};
	QPushButton *leftbtn = new QPushButton(this);
	leftbtn->setFixedSize(14,14);
	leftbtn->setStyleSheet("QPushButton {border-image:url(:/res/images/arr_left.png); background-color:transparent;}"
			"QPushButton:hover {border-image:url(:/res/images/arr_left_hover.png);}");
	connect(leftbtn, &QPushButton::clicked, left_click);
	bottomlayout->addWidget(leftbtn);
	bottomlayout->addSpacing(5);

	auto page_click = [&](int page) {
		m_nPage = page;
		GetPageListLimit();
	};
	m_pPageWidget = new PageWidget(this);
	connect(m_pPageWidget, &PageWidget::ButtonClicked, page_click);
	bottomlayout->addWidget(m_pPageWidget);
	bottomlayout->addSpacing(5);

	auto right_click = [&] {
		int page = m_nPage + 1;
		if (page <= m_nAllPage) {
			m_pPageWidget->SetCurrentCount(page);
			m_nPage = page;
			m_pPageWidget->SetCheckIndex(m_nPage);
			GetPageListLimit();
		}
	};
	QPushButton *rightbtn = new QPushButton(this);
	rightbtn->setFixedSize(14,14);
	rightbtn->setStyleSheet("QPushButton {border-image:url(:/res/images/arr_right.png); background-color:transparent;}"
			"QPushButton:hover {border-image:url(:/res/images/arr_right_hover.png);}");
	connect(rightbtn, &QPushButton::clicked, right_click);
	bottomlayout->addWidget(rightbtn);

	bottomlayout->addSpacing(20);

	m_pEdit = new QLineEdit(this);
	m_pEdit->setFixedSize(44, 28);
	m_pEdit->setAlignment(Qt::AlignHCenter);
	m_pEdit->setStyleSheet("QLineEdit{border-radius: 4px; border:1px solid rgb(233, 233, 233); font-size:12px; color:rgb(68,68,68); background-color:transparent;}");
	QRegExp regExp("^[0-9]*[1-9][0-9]*$");
	QRegExpValidator *pattern= new QRegExpValidator(regExp, this);
	m_pEdit->setValidator(pattern);
	bottomlayout->addWidget(m_pEdit);
	bottomlayout->addSpacing(5);

	auto skip_click = [&]{
		int page = m_pEdit->text().toInt();
		if (page >= 1 && page <= m_nAllPage) {
			m_nPage = page;
			m_pPageWidget->SetCurrentCount(m_nPage);
			m_pPageWidget->SetCheckIndex(m_nPage);
			GetPageListLimit();
		}
	};
	QPushButton *skipbtn = new QPushButton(QString(tr("jump_to")), this);
	skipbtn->setFixedWidth(50);
	skipbtn->setStyleSheet("QPushButton {font-size:12px; color:rgb(68,68,68); background-color:transparent;}"
			"QPushButton:hover {color:rgb(255, 219, 0);}");
	connect(skipbtn, &QPushButton::clicked, skip_click);
	bottomlayout->addWidget(skipbtn);
	bottomlayout->addStretch();

	QHBoxLayout *horlayout3 = new QHBoxLayout();
	horlayout3->setContentsMargins(0,0,0,0);
	horlayout3->setSpacing(8);
	rightLayout->addLayout(horlayout3);

	auto selectall = [&]{
		if (m_pScrollArea) {
			QLayoutItem *child;
			int count = m_pScrollArea->widget()->layout()->count();
			for (int i = 0; i < count; i++) {
				child = m_pScrollArea->widget()->layout()->itemAt(i);
				NotificationItem *item = qobject_cast<NotificationItem *>(child->widget());
				item->SetCheck(true);
			}
		}
	};
	QPushButton *selectallBtn = new QPushButton(QString(tr("select_all")), this);
	selectallBtn->setFixedSize(100,36);
	selectallBtn->setStyleSheet(CS_BASE_DIALOG_CANCEL);
	connect(selectallBtn, &QPushButton::clicked, selectall);
	horlayout3->addWidget(selectallBtn);

	auto selectinvert = [&] {
		if (m_pScrollArea) {
			QLayoutItem *child;
			int count = m_pScrollArea->widget()->layout()->count();
			for (int i = 0; i < count; i++) {
				child = m_pScrollArea->widget()->layout()->itemAt(i);
				NotificationItem *item = qobject_cast<NotificationItem *>(child->widget());
				item->SetCheck(!item->GetCheck());
			}
		}
	};
	QPushButton *selectinvertBtn = new QPushButton(QString(tr("select_invert")), this);
	selectinvertBtn->setFixedSize(100,36);
	selectinvertBtn->setStyleSheet(CS_BASE_DIALOG_CANCEL);
	connect(selectinvertBtn, &QPushButton::clicked, selectinvert);
	horlayout3->addWidget(selectinvertBtn);

	QPushButton *operationBtn = new QPushButton(QString(tr("operation")), this);
	operationBtn->setFixedSize(100,36);
	operationBtn->setStyleSheet("QPushButton {font-size:14px; color:rgb(34, 34, 34); background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop : 0 #FFDB00, stop : 1 #FFEB00);border-radius:6px; background-image: url(:res/images/more.png); background-repeat: repeat-no-repeat; background-position: right; background-origin:content; padding-right: 15px;}"
			"QPushButton:hover {background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop : 0 #FFC900, stop : 1 #FFDC00);}");
	connect(operationBtn, &QPushButton::clicked, this, &NotificationDialog::OnOperationClicked);
	horlayout3->addWidget(operationBtn);
	horlayout3->addStretch();

	GetUnreadCount();
	GetInfoClass();
	GetPageList();
 }

NotificationDialog::~NotificationDialog() {}

void NotificationDialog::AddListItem(const STInfoClass &info) {
	QListWidgetItem *item = new QListWidgetItem();
	item->setSizeHint(QSize(109, 40));
	m_pListWidget->addItem(item);
	NoteTypeItem *noteItem = new NoteTypeItem(info);
	m_pListWidget->setItemWidget(item, noteItem);
}

void NotificationDialog::UpdateListItem(const STInfoClass &info) {
	int count = m_pListWidget->count();
	for (int i = 1; i < count; i++) {
		NoteTypeItem *noteItem = qobject_cast<NoteTypeItem *>(m_pListWidget->itemWidget(m_pListWidget->item(i)));
		if (noteItem && noteItem->GetClassId() == info.classId) {
			noteItem->SetNoteCount(info.unreadNum);
		}
	}
}

void NotificationDialog::AddNoteItem(const STPageData &note) {
	QLayout *layout = m_pScrollArea->widget()->layout();
	auto callback = std::bind(&NotificationDialog::UpdateSize, this, std::placeholders::_1);
	auto callback1 = std::bind(&NotificationDialog::ClickNote, this, std::placeholders::_1);
	NotificationItem *notItem = new NotificationItem(note, callback, callback1, this);
	QSize size = m_pScrollArea->widget()->size();
	QSize scrollSzie = m_pScrollArea->size();
	if (size.height() + notItem->height() > scrollSzie.height())
		m_pScrollArea->widget()->resize(scrollSzie.width() - 8, size.height() + notItem->height());
	else
		m_pScrollArea->widget()->resize(scrollSzie.width(), size.height() + notItem->height());

	layout->addWidget(notItem);

	connect(notItem, &NotificationItem::ReadNote, this, &NotificationDialog::OnReadNote);
}

void NotificationDialog::UpdateSize(int diffheight) {
	m_pScrollArea->widget()->resize(m_pScrollArea->widget()->width(), m_pScrollArea->widget()->height() + diffheight);
}

void NotificationDialog::ClickNote(const STPageData &note) {
	if (m_pWidget) {
		delete m_pWidget;
		m_pWidget = nullptr;
	}

	m_pWidget = new QWidget(this);
	m_pWidget->setFixedSize(765, 640);
	m_pWidget->setStyleSheet("QWidget {background-color:rgb(255,255,255);border-radius:10px;}");
	m_pWidget->setGeometry(38,29,765,640);
	QVBoxLayout *verlayout2 = new QVBoxLayout(m_pWidget);
	verlayout2->setContentsMargins(30,10,20,20);
	verlayout2->setSpacing(10);
	QHBoxLayout *horlayout4 = new QHBoxLayout(m_pWidget);
	horlayout4->setContentsMargins(0,0,0,0);
	horlayout4->setSpacing(0);
	horlayout4->addStretch();
	verlayout2->addLayout(horlayout4);
	QPushButton *button = new QPushButton(m_pWidget);
	button->setFixedSize(16,16);
	button->setStyleSheet(CS_BASE_DIALOG_CLOSE);
	horlayout4->addWidget(button);
	auto visible = [&] {
		GetCurrentPageList();
		m_pWidget->close();
		m_pWidget->deleteLater();
		m_pWidget = nullptr;
	};
	connect(button, &QPushButton::clicked, visible);

	QLabel *pLabel = new QLabel(m_pWidget);
	pLabel->setStyleSheet("QLabel {color:#222222;font-size:16px; background-color:transparent;font-family:Microsoft YaHei;font-weight: 600;}");
	pLabel->setFixedWidth(710);
	pLabel->setWordWrap(true);
	verlayout2->addWidget(pLabel);

	QLabel *pLabel1 = new QLabel(m_pWidget);
	pLabel1->setStyleSheet("QLabel {color:rgb(136, 136, 136);font-size:12px; background-color:transparent; font-family:Microsoft YaHei;}");
	verlayout2->addWidget(pLabel1);

	WebBrowserWidget *pWebBrowserWidget = new WebBrowserWidget(m_pWidget);
	pWebBrowserWidget->SetMinimumSize(QSize(500,500));
	verlayout2->addWidget(pWebBrowserWidget);

	verlayout2->addStretch();


	pLabel->setText(QString::fromStdString(note.title));
	auto a = [] (long long timestamp){
		// 后端 大kengbi 大kenghuo caibi
		QDateTime time = QDateTime::fromMSecsSinceEpoch(timestamp);
		return time.toString("yyyy-MM-dd hh:mm:ss");
	};

	long long llsend = (long long)note.sendTime;
	pLabel1->setText(QString::fromStdString(note.senderName) + QString("\040\040\040\040\040\040") + a(llsend));

	QString hm = QString::fromStdString(note.content) + QString("<style type=\"text/css\">" 
		"::-webkit-scrollbar{width:4px;height:4px;}"
		"::-webkit-scrollbar-track{background: white;border-radius:2px;}"
		"::-webkit-scrollbar-thumb{background: #BBBBBB;border-radius:2px;}"
		"::-webkit-scrollbar-thumb:hover{background: #7D7D7D;}"
		"::-webkit-scrollbar-corner{background: white;}"
		"</style>");
	pWebBrowserWidget->SetHtml(hm.toUtf8());

	m_pWidget->show();
}

void NotificationDialog::DeleteLayout(QLayout *layout) {
	if (layout) {
		QLayoutItem *child;
		while ((child = layout->takeAt(0)) != 0) {
		    delete child->widget();
		    delete child;
		    child = nullptr;
		}
	}
}

void NotificationDialog::GetInfoClass() {
	STReqData req;
	req.func = FUNC_GET_INFO_CLASS;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("3")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/infoClass");

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::GetPageList() {
	STReqData req;
	req.func = FUNC_GET_PAGE_LIST;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/pageList");

	Json::FastWriter writer;
	Json::Value root;
	root["page"] = Json::Value(m_nPage);
	root["size"] = Json::Value(m_nSize);
	Json::Value param;
	if (m_classid != -1)
		param["classId"] = Json::Value(m_classid);
	if (m_stateType != STATE_ALL)
		param["readStatus"] = Json::Value(QString::number(m_stateType).toStdString().c_str());
	root["param"] = Json::Value(param);
	sprintf(req.body, "%s", writer.write(root).c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::GetPageListLimit() {
	STReqData req;
	req.func = FUNC_GET_PAGE_LIST_LIMIT;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/pageList");

	Json::FastWriter writer;
	Json::Value root;
	root["page"] = Json::Value(m_nPage);
	root["size"] = Json::Value(m_nSize);
	Json::Value param;
	if (m_classid != -1)
		param["classId"] = Json::Value(m_classid);
	if (m_stateType != STATE_ALL)
		param["readStatus"] = Json::Value(QString::number(m_stateType).toStdString().c_str());
	root["param"] = Json::Value(param);
	sprintf(req.body, "%s", writer.write(root).c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::GetUnreadCount() {
	STReqData req;
	req.func = FUNC_GET_UNREAD_COUNT;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("3")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/countUnRead");

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::GetCurrentPageList() {
	STReqData req;
	req.func = FUNC_GET_CURRENT_PAGE_LIST;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/pageList");

	Json::FastWriter writer;
	Json::Value root;
	root["page"] = Json::Value(m_nPage);
	root["size"] = Json::Value(m_nSize);
	Json::Value param;
	if (m_classid != -1)
		param["classId"] = Json::Value(m_classid);
	if (m_stateType != STATE_ALL)
		param["readStatus"] = Json::Value(QString::number(m_stateType).toStdString().c_str());
	root["param"] = Json::Value(param);
	sprintf(req.body, "%s", writer.write(root).c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

bool NotificationDialog::OnConfirmClick() {
	return true;
}

void NotificationDialog::OnListItemClicked(QListWidgetItem *item) {
	int row = m_pListWidget->row(item);
	m_nPage = 1;
	if (row == 0) {
		m_classid = -1;
	} else {
		NoteTypeItem *noteTypeItem = qobject_cast<NoteTypeItem *>(m_pListWidget->itemWidget(item));
		m_classid = noteTypeItem->GetClassId();
	}
	GetPageList();
}

void NotificationDialog::OnPageComboChange(const QString &text) {
	m_nPage = 1;
	m_nSize = text.toInt();
	GetPageList();
}

void NotificationDialog::OnOperationClicked() {
	QMenu menu;
	menu.setStyleSheet("QMenu {border:1px solid #D3CCDF; background-color:rgb(255,255,255);}"
			"QMenu::item {background-color:transparent; color:rgb(85, 85, 85); padding:8px 20px; font-size:14px; border-bottom:1px solid #DBDBDB;}"
			"QMenu::item:selected {background-color:rgb(249,248,251); color:rgb(85, 85, 85);}");
	if (m_stateType != STATE_READ)
		menu.addAction(QString(tr("select_read")), this, SLOT(OnSelectRead()));
	menu.addAction(QString(tr("select_delete")), this, SLOT(OnSelectDelete()));
	if (m_stateType != STATE_READ)
		menu.addAction(QString(tr("all_read")), this, SLOT(OnAllRead()));
	menu.addAction(QString(tr("all_delete")), this, SLOT(OnAllDelete()));

	QPushButton *btn =qobject_cast<QPushButton *>(sender());
	QRect rect = btn->geometry();
	QPoint pt = this->mapToGlobal(rect.bottomLeft());

	menu.exec(pt);
}

void NotificationDialog::OnSelectRead() {
	STReqData req;
	req.func = FUNC_NOTIC_READ_BATCH;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/readBatch");

	Json::FastWriter writer;
	Json::Value array;
	if (m_pScrollArea) {
		QLayoutItem *child;
		int count = m_pScrollArea->widget()->layout()->count();
		for (int i = 0; i < count; i++) {
			child = m_pScrollArea->widget()->layout()->itemAt(i);
			NotificationItem *item = qobject_cast<NotificationItem *>(child->widget());
			if (item->GetCheck())
				array.append(item->GetNoteId());
		}
	}

	sprintf(req.body, "%s", writer.write(array).c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::OnSelectDelete() {
	STReqData req;
	req.func = FUNC_NOTIC_DELETE_BATCH;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/deleteBatch");

	Json::FastWriter writer;
	Json::Value array;
	if (m_pScrollArea) {
		QLayoutItem *child;
		int count = m_pScrollArea->widget()->layout()->count();
		for (int i = 0; i < count; i++) {
			child = m_pScrollArea->widget()->layout()->itemAt(i);
			NotificationItem *item = qobject_cast<NotificationItem *>(child->widget());
			if (item->GetCheck())
				array.append(item->GetNoteId());
		}
	}

	sprintf(req.body, "%s", writer.write(array).c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::OnAllRead() {
	STReqData req;
	req.func = FUNC_NOTIC_READ_BATCH;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/readBatch");

	Json::FastWriter writer;
	Json::Value array;
	if (m_pScrollArea) {
		QLayoutItem *child;
		int count = m_pScrollArea->widget()->layout()->count();
		for (int i = 0; i < count; i++) {
			child = m_pScrollArea->widget()->layout()->itemAt(i);
			NotificationItem *item = qobject_cast<NotificationItem *>(child->widget());
			array.append(item->GetNoteId());
		}
	}

	sprintf(req.body, "%s", writer.write(array).c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::OnAllDelete() {
	STReqData req;
	req.func = FUNC_NOTIC_DELETE_BATCH;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/shop/cs/notice/deleteBatch");

	Json::FastWriter writer;
	Json::Value array;
	if (m_pScrollArea) {
		QLayoutItem *child;
		int count = m_pScrollArea->widget()->layout()->count();
		for (int i = 0; i < count; i++) {
			child = m_pScrollArea->widget()->layout()->itemAt(i);
			NotificationItem *item = qobject_cast<NotificationItem *>(child->widget());
			array.append(item->GetNoteId());
		}
	}

	sprintf(req.body, "%s", writer.write(array).c_str());

	AsynReqManager::GetInstance()->InsertReq(req);
}

void NotificationDialog::OnReadyData(const STRspData &stRsp)
{
	switch (stRsp.func) {
		case FUNC_GET_INFO_CLASS: {
			STInfoClassList infoClasslist = ParseInfoClassList(stRsp.recvBytes);
			if (m_pListWidget->count() > 1) {
				for (auto item : infoClasslist.classlist) {
					UpdateListItem(item);
				}
			} else {
				for (auto item : infoClasslist.classlist) {
					AddListItem(item);
				}
			}
			break;
		}
		case FUNC_GET_PAGE_LIST: {
			DeleteLayout(m_pScrollArea->widget()->layout());
			m_pScrollArea->widget()->resize(0,0);

			STPageList stPageList = ParsePageList(stRsp.recvBytes);
			// cjf
			//for (int i = 0; i < 5; i++) {
			//	STPageData data;
			//	data.title = "data.titledata.titledata.title";
			//	data.content = "data.contentdata.contentdata.contentdata.contentdata.contentdata.contentdata.contentdata.content";
			//	data.senderName = "data.senderNamedata.senderNamedata.senderName";
			//	data.sendTime = "2020-12-17T12:19:22.892Z";
			//	AddNoteItem(data);
			//}
			//m_pPageWidget->SetPageCount(8);
			//m_nAllPage = 8;

			m_nAllPage = stPageList.pageNum;
			m_pPageLabel->setText(QString(tr("total")) + QString::number(stPageList.pageNum)
				+ QString(tr("page")) + QString::number(stPageList.total) + QString(tr("page_info")));
			m_pPageWidget->SetPageCount(stPageList.pageNum);
			m_pEdit->setText("1");
			for (auto item : stPageList.pagelist) {
				AddNoteItem(item);
			}
			break;
		}
		case FUNC_GET_PAGE_LIST_LIMIT: {
			DeleteLayout(m_pScrollArea->widget()->layout());
			m_pScrollArea->widget()->resize(0,0);

			STPageList stPageList = ParsePageList(stRsp.recvBytes);
			// cjf
			//for (int i = 0; i < 5; i++) {
			//	STPageData data;
			//	data.title = "data.titledata.titledata.title";
			//	data.content = "data.contentdata.contentdata.contentdata.contentdata.contentdata.contentdata.contentdata.content";
			//	data.senderName = "data.senderNamedata.senderNamedata.senderName";
			//	data.sendTime = "2020-12-17T12:19:22.892Z";
			//	AddNoteItem(data);
			//}
			for (auto item : stPageList.pagelist) {
				AddNoteItem(item);
			}
			break;
		}
		case FUNC_NOTIC_DELETE_BATCH:
		case FUNC_NOTIC_READ_BATCH: 
			m_nPage = 1;
			GetUnreadCount();
			GetInfoClass();
			GetPageList();
			emit OperationMessage();
			break;
		case FUNC_GET_UNREAD_COUNT: {
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(stRsp.recvBytes.data(), root)) {
				qDebug() << "parse error";
			} else {
				qDebug() << "parse success";
				if (root.isMember("result")) {
					int count = root["result"].asInt();
					QPushButton *btn = m_pStateGroup->GetButton(0);
					if (btn) {
						NotificationButton *noteBtn = qobject_cast<NotificationButton *>(btn);
						noteBtn->SetNoteCount(count);
					}
				}
			}
			
			break;
		}
		case FUNC_NOTIC_READ: {
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(stRsp.recvBytes.data(), root)) {
				qDebug() << "parse error";
			} else {
				qDebug() << "parse success";
				if (root.isMember("success")) {
					if (root["success"].isBool() && root["success"].asBool()) {
						GetUnreadCount();
						GetInfoClass();
						emit OperationMessage();
					}
				}
			}

			break;
		}
		case FUNC_GET_CURRENT_PAGE_LIST:
			DeleteLayout(m_pScrollArea->widget()->layout());
			m_pScrollArea->widget()->resize(0,0);

			STPageList stPageList = ParsePageList(stRsp.recvBytes);

			if (stPageList.pagelist.size() > 0) {
				m_nAllPage = stPageList.pageNum;
				m_pPageLabel->setText(QString(tr("total")) + QString::number(stPageList.pageNum)
					+ QString(tr("page")) + QString::number(stPageList.total) + QString(tr("page_info")));
				m_pPageWidget->SetPageCount(stPageList.pageNum);
				if (m_nPage <= m_nAllPage)
					m_pPageWidget->SetCheckIndex(m_nPage);
				else
					m_pPageWidget->SetCheckIndex(m_nAllPage);
				for (auto item : stPageList.pagelist) {
					AddNoteItem(item);
				}
			} else {
				if (m_nPage > 1)
					m_nPage--;
				GetCurrentPageList();
			}
			
			break;
	}
}

void NotificationDialog::OnReadNote(int id) {
	STReqData req;
	req.func = FUNC_NOTIC_READ;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"), QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("platform"), QByteArray("5")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s%d", BASE_URL.c_str(), "/shop/cs/notice/read?id=", id);

	AsynReqManager::GetInstance()->InsertReq(req);
}

UserInfoDialog::UserInfoDialog(std::string token, STUser stUser, const QString &head, QWidget *parent)
	: MoveDialog(parent)
	, m_strToken(token)
	, m_pChineseName(nullptr)
	, m_pEnglishName(nullptr)
	, m_pWelcomeRemark(nullptr)
	, m_pCloseRemark(nullptr)
	, m_pImageBtn(nullptr)
	, m_pImageBtn2(nullptr)
	, m_strImagePath("")
	, m_strName(stUser.stCsUser.name)
	, m_strNameEn(stUser.stCsUser.nameEn)
	, m_strWelcome(stUser.stCsUser.welcomeInfo)
	, m_strGoodbye(stUser.stCsUser.goodbyeInfo)
	, m_strHead(head.isEmpty() ? QString(":/res/images/head_icon.png") : head)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	setFixedSize(560,616);
	SetNinePixmap(":res/images/cs_mask.png", 200, 100, 560, 616);	

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(38,29,38,82);
	mainLayout->setSpacing(0);

	//QWidget *topWidget = new QWidget(this);
	//topWidget->setFixedHeight(40);
	//topWidget->setObjectName("cs_top");
	//topWidget->setStyleSheet(CS_BASE_DIALOG_TOP_BG);
	//mainLayout->addWidget(topWidget);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setContentsMargins(17,8,20,10);
	mainLayout->addLayout(horLayout);
	QLabel *title = new QLabel(QString(tr("confirm_tips")), this);
	title->setStyleSheet(CS_BASE_DIALOG_TITLE);
	horLayout->addWidget(title);
	horLayout->addStretch();

	auto on_close = [&] {
		done(DialogCode::Rejected);
		close();
	};

	QPushButton *closeBtn = new QPushButton(this);
	closeBtn->setFixedSize(16,16);
	closeBtn->setStyleSheet(CS_BASE_DIALOG_CLOSE);
	connect(closeBtn, &QPushButton::clicked, this, on_close);
	horLayout->addWidget(closeBtn);

	mainLayout->addStretch();

	QGridLayout *contentLayout = new QGridLayout();
	contentLayout->setContentsMargins(30,0,30,0);
	contentLayout->setHorizontalSpacing(10);
	contentLayout->setVerticalSpacing(10);
	mainLayout->addLayout(contentLayout);

	QString starQss("QLabel {color:rgb(255,0,0);}");
	QLabel *star1 = new QLabel("*", this);
	star1->setStyleSheet(starQss);
	contentLayout->addWidget(star1, 0, 0, 1, 1);

	QLabel *loginAccLabel = new QLabel(QString(tr("login_account")), this);
	loginAccLabel->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(loginAccLabel, 0, 1, 1, 1);

	QLineEdit *pLoginAccount = new QLineEdit(this);
	pLoginAccount->setFixedSize(335, 36);
	pLoginAccount->setText(QString::fromStdString(stUser.userName));
	pLoginAccount->setReadOnly(true);
	pLoginAccount->setStyleSheet("QLineEdit{border: 1px solid #E9E9E9;border-radius:4px;font-size:12px;color:black;background:transparent;padding-left:10;}");
	contentLayout->addWidget(pLoginAccount, 0, 2, 1, 2);

	QLabel *headImgLabel = new QLabel(QString(tr("head_image")), this);
	headImgLabel->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(headImgLabel, 1, 1, 1, 1);

	QWidget *widget = new QWidget(this);
	widget->setFixedSize(70, 70);
	widget->setAttribute(Qt::WA_TranslucentBackground);
	contentLayout->addWidget(widget, 1, 2, 1, 1);

	QHBoxLayout *layout1 = new QHBoxLayout();
	layout1->setContentsMargins(0,0,0,0);
	layout1->setSpacing(0);
	widget->setLayout(layout1);

	m_pImageBtn = new QPushButton(this);
	m_pImageBtn->setCursor(QCursor(Qt::PointingHandCursor));
	m_pImageBtn->setFixedSize(70,70);
	m_pImageBtn->setStyleSheet("QPushButton {background-color: rgb(238, 238, 238); border-radius:10px; background-image: url(:res/images/add_path.png); background-origin: content; background-position: center; background-repeat: no-repeat;}");
	connect(m_pImageBtn, &QPushButton::clicked, this, &UserInfoDialog::OnUploadImage);
	layout1->addWidget(m_pImageBtn);

	m_pImageBtn2 = new QPushButton(this);
	m_pImageBtn2->setCursor(QCursor(Qt::PointingHandCursor));
	m_pImageBtn2->setFixedSize(70,70);
	if (!head.isEmpty()) {
		QString temp = head;
		m_pImageBtn2->setStyleSheet(QString("QPushButton {border-radius:10px; border-image:url(") + temp.replace('\\', '/') + QString(");}"));
	}
	connect(m_pImageBtn2, &QPushButton::clicked, this, &UserInfoDialog::OnUploadImage);
	layout1->addWidget(m_pImageBtn2);

	QLabel *formatLabel = new QLabel(QString(tr("image_format")), this);
	formatLabel->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(formatLabel, 1, 3, 1, 1);

	QLabel *star2 = new QLabel("*", this);
	star2->setStyleSheet(starQss);
	contentLayout->addWidget(star2, 2, 0, 1, 1);

	QLabel *chNameLabel = new QLabel(QString(tr("chinese_name")), this);
	chNameLabel->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(chNameLabel, 2, 1, 1, 1);

	m_pChineseName = new QLineEdit(this);
	m_pChineseName->setStyleSheet("QLineEdit{border: 1px solid #E9E9E9;border-radius:4px;font-size:12px;color:black;background:transparent;padding-left:10;}");
	m_pChineseName->setFixedSize(335, 36);
	m_pChineseName->setText(QString::fromStdString(m_strName));
	m_pChineseName->setPlaceholderText(QString(tr("name_limit")));
	m_pChineseName->setMaxLength(20);
	QRegExp rx = QRegExp("[^\\\\/:*?\"<>|]*");
	QRegExpValidator* validator = new QRegExpValidator(rx);
	m_pChineseName->setValidator(validator);
	contentLayout->addWidget(m_pChineseName, 2, 2, 1, 2);

	QLabel *star3 = new QLabel("*", this);
	star3->setStyleSheet(starQss);
	contentLayout->addWidget(star3, 3, 0, 1, 1);

	QLabel *enNameLabel = new QLabel(QString(tr("english_name")), this);
	enNameLabel->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(enNameLabel, 3, 1, 1, 1);

	m_pEnglishName = new QLineEdit(this);
	m_pEnglishName->setStyleSheet("QLineEdit{border: 1px solid #E9E9E9;border-radius:4px;font-size:12px;color:black;background:transparent;padding-left:10;}");
	m_pEnglishName->setFixedSize(335, 36);
	m_pEnglishName->setText(QString::fromStdString(m_strNameEn));
	m_pEnglishName->setPlaceholderText(QString(tr("name_limit")));
	m_pEnglishName->setMaxLength(20);
	m_pEnglishName->setValidator(validator);

	contentLayout->addWidget(m_pEnglishName, 3, 2, 1, 2);

	QLabel *star4 = new QLabel("*", this);
	star4->setStyleSheet(starQss);
	contentLayout->addWidget(star4, 4, 0, 1, 1);

	QLabel *welcomeLabel = new QLabel(QString(tr("welcome_remark")), this);
	welcomeLabel->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(welcomeLabel, 4, 1, 1, 1);

	m_pWelcomeRemark = new QTextEdit(this);
	m_pWelcomeRemark->setStyleSheet("QTextEdit{border: 1px solid #E9E9E9;border-radius:4px;font-size:12px;color:black;background:transparent;padding-left:10;}");
	m_pWelcomeRemark->setFixedHeight(67);
	m_pWelcomeRemark->setText(QString::fromStdString(stUser.stCsUser.welcomeInfo));
	connect(m_pWelcomeRemark, &QTextEdit::textChanged, this, &UserInfoDialog::OnTextChange);
	contentLayout->addWidget(m_pWelcomeRemark, 4, 2, 1, 2);
	
	QLabel *star5 = new QLabel("*", this);
	star5->setStyleSheet(starQss);
	contentLayout->addWidget(star5, 5, 0, 1, 1);

	QLabel *closeRemLabel = new QLabel(QString(tr("close_remark")), this);
	closeRemLabel->setStyleSheet(FINISH_DIALOG_LABEL);
	contentLayout->addWidget(closeRemLabel, 5, 1, 1, 1);

	m_pCloseRemark = new QTextEdit(this);
	m_pCloseRemark->setStyleSheet("QTextEdit{border: 1px solid #E9E9E9;border-radius:4px;font-size:12px;color:black;background:transparent;padding-left:10;}");
	m_pCloseRemark->setFixedHeight(67);
	m_pCloseRemark->setText(QString::fromStdString(stUser.stCsUser.goodbyeInfo));
	connect(m_pCloseRemark, &QTextEdit::textChanged, this, &UserInfoDialog::OnTextChange);
	contentLayout->addWidget(m_pCloseRemark, 5, 2, 1, 2);

	mainLayout->addStretch();

	QHBoxLayout *bottomLayout = new QHBoxLayout();
	bottomLayout->setContentsMargins(0,0,0,0);
	bottomLayout->setSpacing(16);
	bottomLayout->addStretch();

	QPushButton *resetBtn = new QPushButton(QString(tr("reset")), this);
	resetBtn->setStyleSheet(CS_BASE_DIALOG_CANCEL);
	resetBtn->setFixedSize(100, 36);
	connect(resetBtn, &QPushButton::clicked, this, &UserInfoDialog::OnResetClick);
	bottomLayout->addWidget(resetBtn);

	QPushButton *saveBtn = new QPushButton(QString(tr("save")), this);
	saveBtn->setStyleSheet(CS_BASE_DIALOG_CONFIRM);
	saveBtn->setFixedSize(100, 36);
	connect(saveBtn, &QPushButton::clicked, this, &UserInfoDialog::OnSaveClick);
	bottomLayout->addWidget(saveBtn);
	bottomLayout->addStretch();
	mainLayout->addLayout(bottomLayout);
	setLayout(mainLayout);

	if (!head.isEmpty()) {
		m_pImageBtn->setVisible(false);
		m_pImageBtn2->setVisible(true);
	} else {
		m_pImageBtn->setVisible(true);
		m_pImageBtn2->setVisible(false);
	}
}

UserInfoDialog::~UserInfoDialog() {}

STCsUser UserInfoDialog::GetCsUser()
{
	return m_stCsUser;
}

void UserInfoDialog::SetLoginInfo(const STCsUser &csuser) {
	
}

void UserInfoDialog::OnUploadImage() {
	QString fileName = QFileDialog::getOpenFileName(this, QString(tr("open_image")), "", QString(tr("file_format")));

	QNetworkRequest networkRequest;

	QFile *file = new QFile(fileName);
	if (!file->open(QIODevice::ReadOnly) || file->size() == 0) {
		file->close();
		return;
	}
	
	QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
	QHttpPart imagePart;
	imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("image/png"));
	imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"file\";filename=\"huli.png\""));

	imagePart.setBodyDevice(file);
	file->setParent(multiPart); 
	multiPart->append(imagePart);

	std::string url = BASE_URL + "/shop/file/image/upload";
	networkRequest.setUrl(QString::fromStdString(url));

	QNetworkAccessManager *networkManager = new QNetworkAccessManager();
	QNetworkReply *networkReply = nullptr;	

	networkRequest.setRawHeader(QByteArray("Content-Type"), QByteArray("multipart/form-data;boundary=") + multiPart->boundary());
	networkRequest.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
	networkRequest.setRawHeader(QByteArray("token"), QByteArray(m_strToken.c_str()));

	if (QString::fromStdString(url).contains("https")) {
#ifndef QT_NO_SSL
			bool sup = QSslSocket::supportsSsl();
			QSslConfiguration conf = networkRequest.sslConfiguration();
			conf.setPeerVerifyMode(QSslSocket::VerifyNone);
			conf.setProtocol(QSsl::TlsV1SslV3);
			networkRequest.setSslConfiguration(conf);
			sup = QSslSocket::supportsSsl();
			if (sup) {
				blog(LOG_INFO, "support ssl");
			} else {
				blog(LOG_INFO, "not support ssl");
				QString supVersion = QSslSocket::sslLibraryBuildVersionString();
				blog(LOG_INFO, "openssl version:%s", supVersion.toStdString().data());
			}
#endif
		}

	networkReply = networkManager->post(networkRequest, multiPart);
	multiPart->setParent(networkReply);
	std::string errorMsg;
	QEventLoop eventLoop;
	connect(networkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
	QTimer::singleShot(15000, &eventLoop, SLOT(quit()));
	eventLoop.exec();

	if (networkReply->error() != QNetworkReply::NoError) {
			blog(LOG_INFO, "error:%d,url:%s", networkReply->error(), networkReply->url().toString().toLocal8Bit().data());
	} else {
		blog(LOG_INFO,"upload image response");
		QByteArray strValue = networkReply->readAll();
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(strValue.data(), root)) {
			TipsDialog dialog(QString(tr("parse_fail")), this);
			dialog.exec();
		} else {
			if (root.isMember("success")) {
				bool success = root["success"].asBool();
				if (success) {
					m_strImagePath = root["result"].asString();
					m_pImageBtn2->setStyleSheet(QString("QPushButton {border-radius:10px; border-image:url(") + fileName.replace('\\', '/') + QString(");}"));
					m_pImageBtn2->setVisible(true);
					m_pImageBtn->setVisible(false);
				} else {
					TipsDialog dialog(QString::fromStdString(root["appMsg"].asString()), this);
					dialog.exec();
				}
			}	
		}
	}

	file->close();
	networkReply->deleteLater();
	networkManager->deleteLater();
}

void UserInfoDialog::OnSaveClick() {
	QString message = "";
	if (m_pChineseName->text().isEmpty()) {
		message = QString(tr("chinese_name_tips_info"));
	} else if (m_pEnglishName->text().isEmpty()) {
		message = QString(tr("english_name_tips_info"));
	} else if (m_pWelcomeRemark->toPlainText().isEmpty()) {
		message = QString(tr("welcome_tips_info"));
	} else if (m_pCloseRemark->toPlainText().isEmpty()) {
		message = QString(tr("close_tips_info"));
	} else if (m_pChineseName->text().count() > 20) {
		message = QString(tr("name_limit_count_tips_info"));
	} else if (m_pEnglishName->text().count() > 20) {
		message = QString(tr("name_limit_count_tips_info"));
	}

	if (message.isEmpty()) {
		std::string url = BASE_URL + "/shop/auth/cs/updateInfo";
		QNetworkRequest networkRequest;
		networkRequest.setUrl(QString::fromStdString(url));
		networkRequest.setRawHeader(QByteArray("Content-Type"), QByteArray("application/json"));
		networkRequest.setRawHeader(QByteArray("platform"), QByteArray("5"));
		networkRequest.setRawHeader(QByteArray("token"), QByteArray(m_strToken.c_str()));
		QNetworkAccessManager *networkManager = new QNetworkAccessManager();
		QNetworkReply *networkReply = nullptr;

		Json::FastWriter writer;
		Json::Value root;
		root["goodbyeInfo"] = m_pCloseRemark->toPlainText().toStdString();
		root["name"] = m_pChineseName->text().toStdString();
		root["nameEn"] = m_pEnglishName->text().toStdString();
		root["welcomeInfo"] = m_pWelcomeRemark->toPlainText().toStdString();
		if (!m_strImagePath.empty())
			root["avatarUrl"] = m_strImagePath;
		char body[2048];
		sprintf(body, "%s", writer.write(root).c_str());

		if (QString::fromStdString(url).contains("https")) {
#ifndef QT_NO_SSL
			bool sup = QSslSocket::supportsSsl();
			QSslConfiguration conf = networkRequest.sslConfiguration();
			conf.setPeerVerifyMode(QSslSocket::VerifyNone);
			conf.setProtocol(QSsl::TlsV1SslV3);
			networkRequest.setSslConfiguration(conf);
			sup = QSslSocket::supportsSsl();
			if (sup) {
				blog(LOG_INFO, "support ssl");
			} else {
				blog(LOG_INFO, "not support ssl");
				QString supVersion = QSslSocket::sslLibraryBuildVersionString();
				blog(LOG_INFO, "openssl version:%s", supVersion.toStdString().data());
			}
#endif
		}

		networkReply = networkManager->post(networkRequest, body);

		std::string errorMsg;
		QEventLoop eventLoop;
		connect(networkReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
		QTimer::singleShot(15000, &eventLoop, SLOT(quit()));
		eventLoop.exec();

		if (networkReply->error() != QNetworkReply::NoError) {
			blog(LOG_INFO, "error:%d,url:%s", networkReply->error(), networkReply->url().toString().toLocal8Bit().data());
		} else {
			blog(LOG_INFO,"update info response");
			QByteArray strValue = networkReply->readAll();
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(strValue.data(), root)) {
				TipsDialog dialog(QString(tr("parse_fail")), this);
				dialog.exec();
			} else {
				if (root.isMember("success")) {
					bool success = root["success"].asBool();
					if (success) {
						m_stCsUser = ParseCsUserInfo(strValue);
						done(DialogCode::Accepted);
					} else {
						TipsDialog dialog(QString::fromStdString(root["appMsg"].asString()), this);
						dialog.exec();
					}
				}
				
			}
		}

		networkReply->deleteLater();
		networkManager->deleteLater();
	} else {
		TipsDialog dialog(message, this);
		dialog.exec();
	}
}

void UserInfoDialog::OnResetClick() {
	m_strImagePath = "";
	if (m_strHead.isEmpty()) {
		m_pImageBtn->setVisible(true);
		m_pImageBtn2->setVisible(false);
	} else {
		m_pImageBtn->setVisible(false);
		QString temp = m_strHead;
		m_pImageBtn2->setStyleSheet(QString("QPushButton {border-radius:10px; border-image:url(") + temp.replace('\\', '/') + QString(");}"));
		m_pImageBtn2->setVisible(true);
	}
	m_pChineseName->setText(QString::fromStdString(m_strName));
	m_pEnglishName->setText(QString::fromStdString(m_strNameEn));
	m_pWelcomeRemark->setText(QString::fromStdString(m_strWelcome));
	m_pCloseRemark->setText(QString::fromStdString(m_strGoodbye));
}

void UserInfoDialog::OnTextChange() {
	QTextEdit *txt = qobject_cast<QTextEdit *>(sender());
	int length = txt->toPlainText().count();
	int maxLength = 50; 
 
	if(length > maxLength) {
		QTextCursor cursor = txt->textCursor();
		cursor.movePosition(QTextCursor::End);
		if(cursor.hasSelection()) {
			cursor.clearSelection();
		}
		cursor.deletePreviousChar();
		txt->setTextCursor(cursor);
	}
}
