#include "sacrouge-dialog.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QIntValidator>
#include <QListWidget>
#include <QPainter>
#include <QScrollBar>
#include <QTextCodec>
#include <QDebug>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <json/json.h>
#include <ctime>
#include <iomanip>
#include<sstream>

#include "mask-label.h"
#include "callback-request.h"
#include "characters-convert.h"
#include "json-parser.h"
#include "custom-stylesheet.h"
#include "util/base.h"

SacrougeDialog::SacrougeDialog(const STBeginRsp &stBeginRsp, const std::string &strToken, QWidget *parent)
	: MoveDialog(parent),
	  m_pMontantEdit(nullptr),
	  m_pCountEdit(nullptr),
	  m_pTicketCount(nullptr),
	  m_bHandOut(true),
	  m_strToken(strToken),
	  m_stBeginRsp(stBeginRsp),
	  m_pGetAccountThread(nullptr)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowModality(Qt::WindowModal);
	setFixedSize(323, 321);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	QWidget *bgWidget = new QWidget(this);
	bgWidget->setObjectName("SacrougeBg");
	bgWidget->setStyleSheet(SACROUGE_BG);
	mainLayout->addWidget(bgWidget);

	auto closeClick = [&] {
		this->close();
	};

	QPushButton *closeBtn = new QPushButton(bgWidget);
	closeBtn->setGeometry(295, 12, 18, 18);
	closeBtn->setStyleSheet(CLOSE_BTN);
	connect(closeBtn, &QPushButton::clicked, this, closeClick);

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->setContentsMargins(27, 59, 26, 30);

	QLabel *montant = new QLabel(QString(tr("montant")), bgWidget);
	montant->setStyleSheet(SACROUGE_BIG_LABEL);
	gridLayout->addWidget(montant, 0, 0, 1, 1);

	QRegExp regExp("[1-9][0-9]*"); 
	QRegExpValidator *regVal = new QRegExpValidator(regExp, this);
	m_pMontantEdit = new QLineEdit(bgWidget);
	m_pMontantEdit->setFixedSize(180, 36);
	m_pMontantEdit->setStyleSheet(SACROUGE_EDIT);
	m_pMontantEdit->setPlaceholderText(QString(tr("montant_limit")));
	m_pMontantEdit->setValidator(regVal);
	connect(m_pMontantEdit, &QLineEdit::textChanged, this, &SacrougeDialog::OnMontantChanged);
	gridLayout->addWidget(m_pMontantEdit, 0, 1, 1, 1);

	QLabel *ticket = new QLabel(QString(tr("ticket")), bgWidget);
	ticket->setStyleSheet(SACROUGE_BIG_LABEL);
	gridLayout->addWidget(ticket, 0, 2, 1, 1);

	QHBoxLayout *horLayout = new QHBoxLayout();
	horLayout->setSpacing(5);
	horLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->addLayout(horLayout, 1, 1, 1, 1);

	QLabel *labelone = new QLabel(QString(tr("solde")), bgWidget);
	labelone->setStyleSheet(SACROUGE_SMALL_LABEL);
	horLayout->addWidget(labelone);

	m_pTicketCount = new QLabel(bgWidget);
	m_pTicketCount->setStyleSheet(SACROUGE_TICKET_COUNT);
	horLayout->addWidget(m_pTicketCount);

	QLabel *labeltwo = new QLabel(QString(tr("ticket")), bgWidget);
	labeltwo->setStyleSheet(SACROUGE_SMALL_LABEL);
	horLayout->addWidget(labeltwo);
	horLayout->addStretch();

	QLabel *labelthree = new QLabel(QString(tr("count")), bgWidget);
	labelthree->setStyleSheet(SACROUGE_BIG_LABEL);
	gridLayout->addWidget(labelthree, 2, 0, 1, 1);

	QRegExp regExp2("^((1[1-9])|([2-9]\d)|([1-9]\d{2,}))$");
	QRegExpValidator *regVal2 = new QRegExpValidator(regExp, this);
	m_pCountEdit = new QLineEdit(bgWidget);
	m_pCountEdit->setFixedSize(180, 36);
	m_pCountEdit->setStyleSheet(SACROUGE_EDIT);
	m_pCountEdit->setValidator(regVal);
	m_pCountEdit->setPlaceholderText(QString(tr("count_limit")));
	m_pCountEdit->setValidator(regVal2);
	connect(m_pCountEdit, &QLineEdit::editingFinished, this, &SacrougeDialog::OnCountFinish);
	gridLayout->addWidget(m_pCountEdit, 2, 1, 1, 1);

	QLabel *labelfour = new QLabel(QString(tr("company")), bgWidget);
	labelfour->setStyleSheet(SACROUGE_SMALL_LABEL);
	gridLayout->addWidget(labelfour, 2, 2, 1, 1);

	QWidget *wi = new QWidget(bgWidget);
	wi->setFixedHeight(10);
	gridLayout->addWidget(wi, 3, 0, 1, 3);

	QPushButton *confirmBtn = new QPushButton(QString(tr("sacrouge")), bgWidget);
	confirmBtn->setFixedSize(270, 44);
	confirmBtn->setStyleSheet(SACROUGE_CONFIRM_BTN);
	connect(confirmBtn, &QPushButton::clicked, this,
		&SacrougeDialog::OnHandOutRedenvel);
	gridLayout->addWidget(confirmBtn, 4, 0, 1, 3);

	QHBoxLayout *horLayout2 = new QHBoxLayout();
	horLayout2->setContentsMargins(0, 5, 0, 0);

	QLabel *labelfive = new QLabel(QString(tr("red_envelopes_info")), bgWidget);
	labelfive->setAlignment(Qt::AlignHCenter);
	labelfive->setStyleSheet(SACROUGE_SMALL_LABEL);
	horLayout2->addWidget(labelfive);

	gridLayout->addLayout(horLayout2, 5, 0, 1, 3);

	bgWidget->setLayout(gridLayout);

	setLayout(mainLayout);

	GetAccount();
}

SacrougeDialog::~SacrougeDialog() {
	if (m_pGetAccountThread != nullptr) {
		m_pGetAccountThread->wait();
		m_pGetAccountThread->deleteLater();
		m_pGetAccountThread = nullptr;
	}
}

void SacrougeDialog::GetAccount() {
	STReqData req;
	req.func = FUNC_GET_ACCOUNT;
	req.method = GET;
	req.list.append(qMakePair(QByteArray("Accept"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/app/user/account/v1/getAccount");
	blog(LOG_INFO, "request %s", req.url);

	auto callback_ = std::bind(&SacrougeDialog::GeAccountCallback, this,
				   std::placeholders::_1,
				   std::placeholders::_2);
	if (m_pGetAccountThread == nullptr) {
		m_pGetAccountThread = new CallbackThread(req, callback_);
	} else {
		m_pGetAccountThread->SetReqData(req);
	}
}

void SacrougeDialog::GeAccountCallback(const QByteArray &bytes,
				       const QNetworkReply::NetworkError &error)
{
	STCompteInfo info = ParseAccount(bytes);
	if (m_pTicketCount)
		m_pTicketCount->setText(QString::number(info.pnTotalAmount));
}

void SacrougeDialog::OnCountFinish() {
	QString str = m_pCountEdit->text();
	if (str.toInt() < 10) {
		m_pCountEdit->clear();
		m_bHandOut = false;
	} else {
		m_bHandOut = true;
	}
}

void SacrougeDialog::OnHandOutRedenvel() {
	if (m_bHandOut && !m_pMontantEdit->text().isEmpty() && !m_pCountEdit->text().isEmpty()) {
		STReqData req;
		req.func = FUNC_HAND_OUT_RED;
		req.method = POST;
		req.list.append(qMakePair(QByteArray("Content-Type"),
					  QByteArray("application/json")));
		req.list.append(qMakePair(QByteArray("appCode"),
					  QByteArray("keepfun_sportlive")));
		req.list.append(qMakePair(QByteArray("terminalType"),
					  QByteArray("1")));
		req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
		sprintf(req.url, "%s%s", BASE_URL.c_str(),
			"/app/red/v1/redHandOut");
		sprintf(req.body,
			"{\"liveId\":%d,\"roomId\":%d, \"totalAmount\":%d,\"totalNumber\":%d}",
			m_stBeginRsp.liveId, m_stBeginRsp.roomId,
			m_pMontantEdit->text().toInt(), m_pCountEdit->text().toInt());
		blog(LOG_INFO, "request  %s", req.url);

		AsynReqManager::GetInstance()->InsertReq(req);
		close();
	}
}

void SacrougeDialog::OnMontantChanged(const QString& text) {
	int value = text.toInt();
	if (value > 5000) {
		m_pMontantEdit->backspace();
	}
}


RedDetailsItem::RedDetailsItem(STPersonRedDetails stPerson, QWidget *parent) : QWidget(parent)
{
	setFixedSize(272, 50);
	QWidget *line = new QWidget(this);
	line->setStyleSheet(DIVIDING_LINE_LIST);
	line->setGeometry(0, 49, 272, 1);

	QGridLayout *mainLayout = new QGridLayout();
	mainLayout->setContentsMargins(6, 7, 10, 7);
	mainLayout->setHorizontalSpacing(10);

	MaskLabel *head = new MaskLabel(this);
	head->setFixedSize(32, 34);
	head->SetNetPath(QString::fromStdString(stPerson.avatar));
	head->setAttribute(Qt::WA_TranslucentBackground);
	mainLayout->addWidget(head, 0, 0, 2, 1);

	QLabel *name = new QLabel(this);
	name->setStyleSheet(RED_DETAILS_ITEM_NAME);
	name->setText(QString::fromStdString(stPerson.nickname));
	mainLayout->addWidget(name, 0, 1, 1, 1);

	QLabel *amount = new QLabel(this);
	amount->setStyleSheet(RED_DETAILS_ITEM_AMOUNT);
	amount->setText(QString::number(stPerson.drawAmount) + QString(tr("ticket")));
	mainLayout->addWidget(amount, 0, 2, 1, 1, Qt::AlignRight);

	QLabel *time = new QLabel(this);
	time->setStyleSheet(RED_DETAILS_ITEM_TIME);
	mainLayout->addWidget(time, 1, 1, 1, 1);
	
	auto ISO8601ToLocaltime = [](const std::string &time) {
		struct std::tm time_struct;
		std::istringstream ss(time);
		ss >> std::get_time(&time_struct, "%Y-%m-%dT%H:%M:%SZ");
		std::time_t time_unix = _mkgmtime(&time_struct);
		if (time_unix == -1)
			return std::string("");

		char current_time[32];
		strftime(current_time, sizeof(current_time), "%m.%d %H:%M", localtime(&time_unix));
		std::string local_time_str(current_time);

		return local_time_str;
	};
	time->setText(QString::fromStdString(ISO8601ToLocaltime(stPerson.drawTime)));

	setLayout(mainLayout);
}

RedDetailsItem::~RedDetailsItem() {}

RedDetailsDialog::RedDetailsDialog(int redID, const std::string &strToken, QWidget *parent)
	: MoveDialog(parent),
	  m_pAmount(nullptr),
	  m_pCount(nullptr),
	  m_pTotal(nullptr),
	  m_pListWidget(nullptr),
	  m_nID(redID),
	  m_strToken(strToken),
	  m_pRedDetailsThread(nullptr)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowModality(Qt::WindowModal);
	setFixedSize(323, 389);
	SetColor("#FCFBFD", 6);

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setContentsMargins(26, 42, 12, 26);
	mainLayout->setSpacing(20);

	auto closeClick = [&] { done(DialogCode::Rejected); };

	QPushButton *closeBtn = new QPushButton(this);
	closeBtn->setGeometry(295, 16, 18, 18);
	closeBtn->setStyleSheet(CLOSE_BTN);
	connect(closeBtn, &QPushButton::clicked, this, closeClick);

	QGridLayout *gridLayout = new QGridLayout();
	gridLayout->setContentsMargins(0, 0, 0, 0);
	gridLayout->setHorizontalSpacing(10);
	gridLayout->setVerticalSpacing(10);

	QLabel *image = new QLabel(this);
	image->setFixedSize(30, 41);
	image->setStyleSheet(RED_DETAILS_IMAGE);
	gridLayout->addWidget(image, 0, 0, 2, 1);

	QLabel *label1 = new QLabel(QString(tr("red_envelope_amount")), this);
	label1->setStyleSheet(RED_DETAILS_SHALLOW);
	gridLayout->addWidget(label1, 0, 1, 1, 1);

	m_pAmount = new QLabel(this);
	m_pAmount->setStyleSheet(RED_DETAILS_DEEP);
	gridLayout->addWidget(m_pAmount, 0, 2, 1, 1);

	QLabel *label2 = new QLabel(QString(tr("ticket")), this);
	label2->setStyleSheet(RED_DETAILS_SHALLOW);
	gridLayout->addWidget(label2, 0, 3, 1, 3);

	QLabel *label3 = new QLabel(QString(tr("receive_count")), this);
	label3->setStyleSheet(RED_DETAILS_SHALLOW);
	gridLayout->addWidget(label3, 1, 1, 1, 1);

	m_pCount = new QLabel(this);
	m_pCount->setStyleSheet(RED_DETAILS_DEEP);
	gridLayout->addWidget(m_pCount, 1, 2, 1, 1);

	m_pTotal = new QLabel(this);
	m_pTotal->setStyleSheet(RED_DETAILS_SHALLOW);
	gridLayout->addWidget(m_pTotal, 1, 3, 1, 3);

	mainLayout->addLayout(gridLayout);

	m_pListWidget = new QListWidget();
	//m_pListWidget->setStyleSheet(RED_DETAILS_LIST);
	//m_pListWidget->verticalScrollBar()->setStyleSheet(RED_DETAILS_SCROLL);
	mainLayout->addWidget(m_pListWidget);

	QLabel *label4 = new QLabel(QString(tr("refund_instructions")), this);
	label4->setFixedHeight(28);
	label4->setStyleSheet(RED_DETAILS_LABEL);
	label4->setAlignment(Qt::AlignCenter);
	mainLayout->addWidget(label4);

	setLayout(mainLayout);

	GetDetails(1);

	STPersonRedDetails st;
	st.avatar = "http://pic.keepfun.cn/upload//image/avatar/2020/11/05/1604564571307019.jpeg";
	st.drawAmount = 100;
	st.drawTime = "2020-11-13T06:49:22.645Z";
	st.nickname = "136****5914";

	AddItem(st);
	AddItem(st);
	AddItem(st);
	AddItem(st);
	AddItem(st);
	AddItem(st);
}

RedDetailsDialog ::~RedDetailsDialog() {
	if (m_pRedDetailsThread != nullptr) {
		m_pRedDetailsThread->wait();
		m_pRedDetailsThread->deleteLater();
		m_pRedDetailsThread = nullptr;
	}
}

void RedDetailsDialog::GetDetails(int page)
{
	STReqData req;
	req.func = FUNC_RED_DETAILS;
	req.method = POST;
	req.list.append(qMakePair(QByteArray("Content-Type"),
				  QByteArray("application/json")));
	req.list.append(qMakePair(QByteArray("appCode"),
				  QByteArray("keepfun_sportlive")));
	req.list.append(qMakePair(QByteArray("terminalType"),
				  QByteArray("1")));
	req.list.append(qMakePair(QByteArray("token"), QByteArray(m_strToken.c_str())));
	sprintf(req.url, "%s%s", BASE_URL.c_str(), "/app/red/v1/redDrawDetail");
	sprintf(req.body, "{\"page\":%d,\"param\":{\"redId\":%d}}", page, m_nID);
	blog(LOG_INFO, "request %s", req.url);

	auto callback_ = std::bind(&RedDetailsDialog::GeDetailsCallback, this,
				   std::placeholders::_1,
				   std::placeholders::_2);
	if (m_pRedDetailsThread == nullptr) {
		m_pRedDetailsThread = new CallbackThread(req, callback_);
	} else {
		m_pRedDetailsThread->SetReqData(req);
	}
}

void RedDetailsDialog::GeDetailsCallback(const QByteArray &bytes, const QNetworkReply::NetworkError &error)
{
	Json::Reader reader;
	Json::Value root;

	static int totalAmount = 1000;
	static int totalCount = 10;
	static int hascount = 4;	// 已被领取个数
	int page = 0, pageNum = 0; 

	if (!reader.parse(bytes.data(), root)) {
		qDebug() << "parse error";
	} else {
		qDebug() << "parse success";
		if (root.isMember("success")) {
			if (root["success"].isBool()) {
				bool success = root["success"].asBool();
				if (success) {
					if (root.isMember("result")) {
						Json::Value result = root["result"];
						if (result.isMember("totalAmount")) {
							if (result["totalAmount"].isInt()) {
								totalAmount = result["totalAmount"].asInt();
							}
						}
						if (result.isMember("totalNumber")) {
							if (result["totalNumber"].isInt()) {
								totalCount = result["totalNumber"].asInt();
							}
						}
						if (result.isMember("hasDrawTotalNumber")) {
							if (result["hasDrawTotalNumber"].isInt()) {
								hascount = result["hasDrawTotalNumber"].asInt();
							}
						}
						if (result.isMember("page")) {
							if (result["page"].isInt()) {
								page = result["page"].asInt();
							}
						}
						if (result.isMember("pageNum")) {
							if (result["pageNum"].isInt()) {
								pageNum = result["pageNum"].asInt();
							}
						}
						if (result.isMember("drawRecordPageVo")) {
							Json::Value details = result["drawRecordPageVo"];
							if (details.isMember("datas")) {
								if (details["datas"].isArray()) {
									Json::Value datas = details["datas"];
									for (int i = 0; i < datas.size(); i++) {
										STPersonRedDetails stPerson;
										if (datas[i].isMember("avatar")) {
											if (datas[i]["avatar"].isString()) {
												stPerson.avatar = datas[i]["avatar"].asString();
											}
										}
										if (datas[i].isMember("drawAmount")) {
											if (datas[i]["drawAmount"].isInt()) {
												stPerson.drawAmount = datas[i]["drawAmount"].asInt();
											}
										}
										if (datas[i].isMember("drawTime")) {
											if (datas[i]["drawTime"].isString()) {
												stPerson.drawTime = datas[i]["drawTime"].asString();
											}
										}
										if (datas[i].isMember("isBestLuck")) {
											if (datas[i]["isBestLuck"].isString()) {
												stPerson.isBestLuck = datas[i]["isBestLuck"].asString();
											}
										}
										if (datas[i].isMember("nickname")) {
											if (datas[i]["nickname"].isString()) {
												stPerson.nickname = datas[i]["nickname"].asString();
											}
										}
										if (datas[i].isMember("userId")) {
											if (datas[i]["userId"].isInt()) {
												stPerson.userId = datas[i]["userId"].asInt();
											}
										}
										AddItem(stPerson);
									}
								}
							}
						}
					}
				} else {
					if (root.isMember("appMsg")) {
						qDebug() << root["appMsg"].asString().data();
					}
				}
			}
		}

		if (page < pageNum) {
			GetDetails(++page);
		}
		if (page == pageNum) {
			m_pAmount->setText(QString::number(totalAmount));
			m_pCount->setText(QString::number(hascount) + QString(tr("company")));
			m_pTotal->setText(QString(tr("total")) + QString::number(totalCount) + QString(tr("company")));
		}
	}
}

void RedDetailsDialog::AddItem(STPersonRedDetails stPerson) {
	RedDetailsItem *redItem = new RedDetailsItem(stPerson, m_pListWidget);
	QListWidgetItem *item = new QListWidgetItem();
	//设置 Item 的大小
	item->setSizeHint(QSize(0, redItem->height()));
	m_pListWidget->addItem(item);
	m_pListWidget->setItemWidget(item, redItem);
}
