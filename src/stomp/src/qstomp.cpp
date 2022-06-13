#include "qstomp.h"

#include <QtCore/QStringList>
#include <QtCore/QSet>
#include <QtCore/QTextCodec>
#include <QtNetwork/QTcpSocket>
#include <qwebsocket.h>
#include <QDebug>
#include <QHostAddress>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>

//发送消息destination
const std::string SEND_DESTINATION = std::string("/exchange/topic.chat");
//接受消息destination
const std::string RECV_DESTINATION = std::string("/user/exchange/topic.chat/message");
static const QList<QByteArray> VALID_COMMANDS =
		QList<QByteArray>()
		<< "ABORT"
		<< "ACK"
		<< "BEGIN"
		<< "COMMIT"
		<< "CONNECT"
		<< "DISCONNECT"
		<< "CONNECTED"
		<< "MESSAGE"
		<< "SEND"
		<< "SUBSCRIBE"
		<< "UNSUBSCRIBE"
		<< "RECEIPT"
		<< "ERROR";

QStompFrame::QStompFrame(QStompFramePrivate * d) : pd_ptr(d)
{
	d->m_valid = true;
	d->m_textCodec = QTextCodec::codecForName("utf-8");
}

QStompFrame::QStompFrame(const QStompFrame &other, QStompFramePrivate * d) : pd_ptr(d)
{
	d->m_valid = other.pd_ptr->m_valid;
	d->m_header = other.pd_ptr->m_header;
	d->m_body = other.pd_ptr->m_body;
	d->m_textCodec = other.pd_ptr->m_textCodec;
}

QStompFrame::~QStompFrame()
{
	delete this->pd_ptr;
}

QStompFrame & QStompFrame::operator=(const QStompFrame &other)
{
	P_D(QStompFrame);
	d->m_valid = other.pd_ptr->m_valid;
	d->m_header = other.pd_ptr->m_header;
	d->m_body = other.pd_ptr->m_body;
	d->m_textCodec = other.pd_ptr->m_textCodec;
	return *this;
}

void QStompFrame::setHeaderValue(const QByteArray &key, const QByteArray &value)
{
	P_D(QStompFrame);
	QByteArray lowercaseKey = key.toLower();
	QStompHeaderList::Iterator it = d->m_header.begin();
	while (it != d->m_header.end()) {
		if ((*it).first.toLower() == lowercaseKey) {
			(*it).second = value;
			return;
		}
		++it;
	}
	this->addHeaderValue(key, value);
}

void QStompFrame::setHeaderValues(const QStompHeaderList &values)
{
	P_D(QStompFrame);
	d->m_header = values;
}

void QStompFrame::addHeaderValue(const QByteArray &key, const QByteArray &value)
{
	P_D(QStompFrame);
	d->m_header.append(qMakePair(key, value));
}

QStompHeaderList QStompFrame::header() const
{
	const P_D(QStompFrame);
	return d->m_header;
}

bool QStompFrame::headerHasKey(const QByteArray &key) const
{
	const P_D(QStompFrame);
	QByteArray lowercaseKey = key.toLower();
	QStompHeaderList::ConstIterator it = d->m_header.constBegin();
	while (it != d->m_header.constEnd()) {
		if ((*it).first.toLower() == lowercaseKey)
			return true;
		++it;
	}
	return false;
}

QList<QByteArray> QStompFrame::headerKeys() const
{
	const P_D(QStompFrame);
	QList<QByteArray> keyList;
	QSet<QByteArray> seenKeys;
	QStompHeaderList::ConstIterator it = d->m_header.constBegin();
	while (it != d->m_header.constEnd()) {
		QByteArray key = (*it).first;
		QByteArray lowercaseKey = key.toLower();
		if (!seenKeys.contains(lowercaseKey)) {
			keyList.append(key);
			seenKeys.insert(lowercaseKey);
		}
		++it;
	}
	return keyList;
}

QByteArray QStompFrame::headerValue(const QByteArray &key) const
{
	const P_D(QStompFrame);
	QByteArray lowercaseKey = key.toLower();
	QStompHeaderList::ConstIterator it = d->m_header.constBegin();
	while (it != d->m_header.constEnd()) {
		if ((*it).first.toLower() == lowercaseKey)
			return (*it).second;
		++it;
	}
	return QByteArray();
}

QList<QByteArray> QStompFrame::allHeaderValues(const QByteArray &key) const
{
	const P_D(QStompFrame);
	QList<QByteArray> valueList;
	QByteArray lowercaseKey = key.toLower();
	QStompHeaderList::ConstIterator it = d->m_header.constBegin();
	while (it != d->m_header.constEnd()) {
		if ((*it).first.toLower() == lowercaseKey)
			valueList.append((*it).second);
		++it;
	}
	return valueList;
}

void QStompFrame::removeHeaderValue(const QByteArray &key)
{
	P_D(QStompFrame);
	QByteArray lowercaseKey = key.toLower();
	QStompHeaderList::Iterator it = d->m_header.begin();
	while (it != d->m_header.end()) {
		if ((*it).first.toLower() == lowercaseKey) {
			d->m_header.erase(it);
			return;
		}
		++it;
	}
}

void QStompFrame::removeAllHeaderValues(const QByteArray &key)
{
	P_D(QStompFrame);
	QByteArray lowercaseKey = key.toLower();
	QStompHeaderList::Iterator it = d->m_header.begin();
	while (it != d->m_header.end()) {
		if ((*it).first.toLower() == lowercaseKey) {
			d->m_header.erase(it);
			continue;
		}
		++it;
	}
}

bool QStompFrame::hasContentLength() const
{
	return this->headerHasKey("content-length");
}

uint QStompFrame::contentLength() const
{
	return this->headerValue("content-length").toUInt();
}

void QStompFrame::setContentLength(uint len)
{
	this->setHeaderValue("content-length", QByteArray::number(len));
}

bool QStompFrame::hasContentType() const
{
	return this->headerHasKey("content-type");
}

QByteArray QStompFrame::contentType() const
{
	QByteArray type = this->headerValue("content-type");
	if (type.isEmpty())
		return QByteArray();

	int pos = type.indexOf(';');
	if (pos == -1)
		return type;

	return type.left(pos).trimmed();
}

void QStompFrame::setContentType(const QByteArray &type)
{
	this->setHeaderValue("content-type", type);
}

bool QStompFrame::hasContentEncoding() const
{
	return this->headerHasKey("content-length");
}

QByteArray QStompFrame::contentEncoding() const
{
	return this->headerValue("content-encoding");
}

void QStompFrame::setContentEncoding(const QByteArray &name)
{
	P_D(QStompFrame);
	//this->setHeaderValue("content-encoding", name);
	this->setHeaderValue("Content-Type", name);
	d->m_textCodec = QTextCodec::codecForName(name);
}

void QStompFrame::setContentEncoding(const QTextCodec * codec)
{
	P_D(QStompFrame);
	//this->setHeaderValue("content-encoding", codec->name());
	this->setHeaderValue("Content-Type", codec->name());
	d->m_textCodec = codec;
}

QByteArray QStompFrame::toByteArray() const
{
	const P_D(QStompFrame);
	if (!this->isValid())
		return QByteArray("");

	QByteArray ret = QByteArray("");

	QStompHeaderList::ConstIterator it = d->m_header.constBegin();
	while (it != d->m_header.constEnd()) {
		QByteArray key = (*it).first;
		ret += key + ":" + (*it).second + "\n";
		++it;
	}
	ret.append('\n');
	return ret + d->m_body;
}

bool QStompFrame::isValid() const
{
	const P_D(QStompFrame);
	return d->m_valid;
}

bool QStompFrame::parseHeaderLine(const QByteArray &line, int)
{
	int i = line.indexOf(':');
	if (i == -1)
		return false;

	QByteArray key = line.left(i).trimmed();
	if (key.toLower() != "passcode" && key.toLower() != "login")
		this->addHeaderValue(key, line.mid(i + 1).trimmed());
	else
		this->addHeaderValue(key, line.mid(i + 1));

	return true;
}

bool QStompFrame::parse(const QByteArray &frame)
{
	P_D(QStompFrame);
	int headerEnd = frame.indexOf("\n\n");
	if (headerEnd == -1)
		return false;

	d->m_body = frame.mid(headerEnd+2);

	QList<QByteArray> lines = frame.left(headerEnd).split('\n');

	if (lines.isEmpty())
		return false;

	for (int i = 0; i < lines.size(); i++) {
		if (!this->parseHeaderLine(lines.at(i), i))
			return false;
	}
	if (this->hasContentLength())
		d->m_body.resize(this->contentLength());
	else if (d->m_body.endsWith(QByteArray("\0\n", 2)))
		d->m_body.chop(2);

	return true;
}

void QStompFrame::setValid(bool v)
{
	P_D(QStompFrame);
	d->m_valid = v;
}

QString QStompFrame::body() const
{
	const P_D(QStompFrame);
	return d->m_textCodec->toUnicode(d->m_body);
}

QByteArray QStompFrame::rawBody() const
{
	const P_D(QStompFrame);
	return d->m_body;
}

void QStompFrame::setBody(const QString &body)
{
	P_D(QStompFrame);
	d->m_body = d->m_textCodec->fromUnicode(body);
}

void QStompFrame::setRawBody(const QByteArray &body)
{
	P_D(QStompFrame);
	d->m_body = body;
}


QStompResponseFrame::QStompResponseFrame() : QStompFrame(new QStompResponseFramePrivate)
{
	this->setType(QStompResponseFrame::ResponseInvalid);
}

QStompResponseFrame::QStompResponseFrame(const QStompResponseFrame &other) : QStompFrame(other, new QStompResponseFramePrivate)
{
	P_D(QStompResponseFrame);
	d->m_type = other.pd_func()->m_type;
}

QStompResponseFrame::QStompResponseFrame(const QByteArray &frame) : QStompFrame(new QStompResponseFramePrivate)
{
	this->setValid(this->parse(frame));
}

QStompResponseFrame::QStompResponseFrame(QStompResponseFrame::ResponseType type) : QStompFrame(new QStompResponseFramePrivate)
{
	this->setType(type);
}

QStompResponseFrame & QStompResponseFrame::operator=(const QStompResponseFrame &other)
{
	QStompFrame::operator=(other);
	P_D(QStompResponseFrame);
	d->m_type = other.pd_func()->m_type;
	return *this;
}

void QStompResponseFrame::setType(QStompResponseFrame::ResponseType type)
{
	P_D(QStompResponseFrame);
	this->setValid(type != QStompResponseFrame::ResponseInvalid);
	d->m_type = type;
}

QStompResponseFrame::ResponseType QStompResponseFrame::type() const
{
	const P_D(QStompResponseFrame);
	return d->m_type;
}

bool QStompResponseFrame::parseHeaderLine(const QByteArray &line, int number)
{
	P_D(QStompResponseFrame);
	if (number != 0)
		return QStompFrame::parseHeaderLine(line, number);

	if (line == "CONNECTED")
		d->m_type = QStompResponseFrame::ResponseConnected;
	else if (line == "MESSAGE")
		d->m_type = QStompResponseFrame::ResponseMessage;
	else if (line == "RECEIPT")
		d->m_type = QStompResponseFrame::ResponseReceipt;
	else if (line == "ERROR")
		d->m_type = QStompResponseFrame::ResponseError;
	else
		return false;

	return true;
}

QByteArray QStompResponseFrame::toByteArray() const
{
	const P_D(QStompResponseFrame);
	if (!this->isValid())
		return QByteArray("");

	QByteArray ret;
	switch (d->m_type) {
		case QStompResponseFrame::ResponseInvalid:
			return QByteArray("");
		case QStompResponseFrame::ResponseConnected:
			ret = "CONNECTED\n"; break;
		case QStompResponseFrame::ResponseMessage:
			ret = "MESSAGE\n"; break;
		case QStompResponseFrame::ResponseReceipt:
			ret = "RECEIPT\n"; break;
		case QStompResponseFrame::ResponseError:
			ret = "ERROR\n"; break;
	}
	return ret + QStompFrame::toByteArray();
}

bool QStompResponseFrame::hasDestination() const
{
	return this->headerHasKey("destination");
}

QByteArray QStompResponseFrame::destination() const
{
	return this->headerValue("destination");
}

void QStompResponseFrame::setDestination(const QByteArray &value)
{
	this->setHeaderValue("destination", value);
}

bool QStompResponseFrame::hasSubscriptionId() const
{
	return this->headerHasKey("subscription");
}

QByteArray QStompResponseFrame::subscriptionId() const
{
	return this->headerValue("subscription");
}

void QStompResponseFrame::setSubscriptionId(const QByteArray &value)
{
	this->setHeaderValue("subscription", value);
}

bool QStompResponseFrame::hasMessageId() const
{
	return this->headerHasKey("message-id");
}

QByteArray QStompResponseFrame::messageId() const
{
	return this->headerValue("message-id");
}

void QStompResponseFrame::setMessageId(const QByteArray &value)
{
	this->setHeaderValue("message-id", value);
}

bool QStompResponseFrame::hasReceiptId() const
{
	return this->headerHasKey("receipt-id");
}

QByteArray QStompResponseFrame::receiptId() const
{
	return this->headerValue("receipt-id");
}

void QStompResponseFrame::setReceiptId(const QByteArray &value)
{
	this->setHeaderValue("receipt-id", value);
}

bool QStompResponseFrame::hasMessage() const
{
	return this->headerHasKey("message");
}

QByteArray QStompResponseFrame::message() const
{
	return this->headerValue("message");
}

void QStompResponseFrame::setMessage(const QByteArray &value)
{
	this->setHeaderValue("message", value);
}


QStompRequestFrame::QStompRequestFrame() : QStompFrame(new QStompRequestFramePrivate)
{
	this->setType(QStompRequestFrame::RequestInvalid);
}

QStompRequestFrame::QStompRequestFrame(const QStompRequestFrame &other) : QStompFrame(other, new QStompRequestFramePrivate)
{
	P_D(QStompRequestFrame);
	d->m_type = other.pd_func()->m_type;
}

QStompRequestFrame::QStompRequestFrame(const QByteArray &frame) : QStompFrame(new QStompRequestFramePrivate)
{
	this->setValid(this->parse(frame));
}

QStompRequestFrame::QStompRequestFrame(QStompRequestFrame::RequestType type) : QStompFrame(new QStompRequestFramePrivate)
{
	this->setType(type);
}

QStompRequestFrame & QStompRequestFrame::operator=(const QStompRequestFrame &other)
{
	QStompFrame::operator=(other);
	P_D(QStompRequestFrame);
	d->m_type = other.pd_func()->m_type;
	return *this;
}

void QStompRequestFrame::setType(QStompRequestFrame::RequestType type)
{
	P_D(QStompRequestFrame);
	this->setValid(type != QStompRequestFrame::RequestInvalid);
	d->m_type = type;
}

QStompRequestFrame::RequestType QStompRequestFrame::type() const
{
	const P_D(QStompRequestFrame);
	return d->m_type;
}

bool QStompRequestFrame::parseHeaderLine(const QByteArray &line, int number)
{
	P_D(QStompRequestFrame);
	if (number != 0)
		return QStompFrame::parseHeaderLine(line, number);

	if (line == "CONNECT")
		d->m_type = QStompRequestFrame::RequestConnect;
	else if (line == "SEND")
		d->m_type = QStompRequestFrame::RequestSend;
	else if (line == "SUBSCRIBE")
		d->m_type = QStompRequestFrame::RequestSubscribe;
	else if (line == "UNSUBSCRIBE")
		d->m_type = QStompRequestFrame::RequestUnsubscribe;
	else if (line == "BEGIN")
		d->m_type = QStompRequestFrame::RequestBegin;
	else if (line == "COMMIT")
		d->m_type = QStompRequestFrame::RequestCommit;
	else if (line == "ABORT")
		d->m_type = QStompRequestFrame::RequestAbort;
	else if (line == "ACK")
		d->m_type = QStompRequestFrame::RequestAck;
	else if (line == "DISCONNECT")
		d->m_type = QStompRequestFrame::RequestDisconnect;
	else
		return false;

	return true;
}

QByteArray QStompRequestFrame::toByteArray() const
{
	const P_D(QStompRequestFrame);
	if (!this->isValid())
		return QByteArray("");

	QByteArray ret;
	switch (d->m_type) {
		case QStompRequestFrame::RequestInvalid:
			return QByteArray("");
		case QStompRequestFrame::RequestConnect:
			ret = "CONNECT\n"; break;
		case QStompRequestFrame::RequestSend:
			ret = "SEND\n"; break;
		case QStompRequestFrame::RequestSubscribe:
			ret = "SUBSCRIBE\n"; break;
		case QStompRequestFrame::RequestUnsubscribe:
			ret = "UNSUBSCRIBE\n"; break;
		case QStompRequestFrame::RequestBegin:
			ret = "BEGIN\n"; break;
		case QStompRequestFrame::RequestCommit:
			ret = "COMMIT\n"; break;
		case QStompRequestFrame::RequestAbort:
			ret = "ABORT\n"; break;
		case QStompRequestFrame::RequestAck:
			ret = "ACK\n"; break;
		case QStompRequestFrame::RequestDisconnect:
			ret = "DISCONNECT\n"; break;
	}
	return ret + QStompFrame::toByteArray();
}

bool QStompRequestFrame::hasDestination() const
{
	return this->headerHasKey("destination");
}

QByteArray QStompRequestFrame::destination() const
{
	return this->headerValue("destination");
}

void QStompRequestFrame::setDestination(const QByteArray &value)
{
	this->setHeaderValue("destination", value);
}

bool QStompRequestFrame::hasTransactionId() const
{
	return this->headerHasKey("transaction");
}

QByteArray QStompRequestFrame::transactionId() const
{
	return this->headerValue("transaction");
}

void QStompRequestFrame::setTransactionId(const QByteArray &value)
{
	this->setHeaderValue("transaction", value);
}

bool QStompRequestFrame::hasMessageId() const
{
	return this->headerHasKey("message-id");
}

QByteArray QStompRequestFrame::messageId() const
{
	return this->headerValue("message-id");
}

void QStompRequestFrame::setMessageId(const QByteArray &value)
{
	this->setHeaderValue("message-id", value);
}

bool QStompRequestFrame::hasReceiptId() const
{
	return this->headerHasKey("receipt");
}

QByteArray QStompRequestFrame::receiptId() const
{
	return this->headerValue("receipt");
}

void QStompRequestFrame::setReceiptId(const QByteArray &value)
{
	this->setHeaderValue("receipt", value);
}

bool QStompRequestFrame::hasAckType() const
{
	return this->headerHasKey("ack");
}

QStompRequestFrame::AckType QStompRequestFrame::ackType() const
{
	if (this->headerValue("ack") == "client")
		return QStompRequestFrame::AckClient;
	return QStompRequestFrame::AckAuto;
}

void QStompRequestFrame::setAckType(QStompRequestFrame::AckType type)
{
	this->setHeaderValue("ack", (type == QStompRequestFrame::AckClient ? "client" : "auto"));
}

bool QStompRequestFrame::hasSubscriptionId() const
{
	return this->headerHasKey("id");
}

QByteArray QStompRequestFrame::subscriptionId() const
{
	return this->headerValue("id");
}

void QStompRequestFrame::setSubscriptionId(const QByteArray &value)
{
	this->setHeaderValue("id", value);
}


QStompClient::QStompClient(QObject *parent) : QObject(parent), pd_ptr(new QStompClientPrivate(this))
{
	P_D(QStompClient);
	m_bSubscribe = false;
	d->m_socket = NULL;
	d->m_websocket = NULL;
	d->m_textCodec = QTextCodec::codecForName("utf-8");
	connect(this, SIGNAL(frameReceived()), SLOT(OnFrameReceived()));
	m_pTimerHeart = new QTimer(this);
	m_pTimerHeart->setInterval(3000);
	m_bConnect = false;
	connect(m_pTimerHeart, &QTimer::timeout, this,
		&QStompClient::OnTimeout);
}

QStompClient::~QStompClient()
{
	delete this->pd_ptr;
}

void QStompClient::connectToHost(const QString &hostname, quint16 port)
{
	P_D(QStompClient);
	if (d->m_socket != NULL && d->m_socket->parent() == this)
		delete d->m_socket;
	d->m_socket = new QTcpSocket(this);
	connect(d->m_socket, &QTcpSocket::connected, this,
		&QStompClient::OnSocketConnected);
	connect(d->m_socket, SIGNAL(disconnected()), this, SIGNAL(socketDisconnected()));
	connect(d->m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SIGNAL(socketStateChanged(QAbstractSocket::SocketState)));
	connect(d->m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SIGNAL(socketError(QAbstractSocket::SocketError)));
	connect(d->m_socket, SIGNAL(readyRead()), this, SLOT(_q_socketReadyRead()));
	d->m_socket->connectToHost(hostname, port);
}

void QStompClient::ConnectWebSocket(const QString &url) {
	P_D(QStompClient);
	Unsubscribe();
	if (d->m_websocket != NULL ) {
		d->m_websocket->deleteLater();
		d->m_websocket = nullptr;
	}
	d->m_buffer.clear();
	d->m_framebuffer.clear();
	d->m_websocket = new QWebSocket();
	connect(d->m_websocket, SIGNAL(connected()), this,
		SLOT(OnWebSocketConnected()));
	connect(d->m_websocket, SIGNAL(disconnected()), this,
		SLOT(OnWebSocketDisconnected()));
	connect(d->m_websocket,
		SIGNAL(textMessageReceived(const QString &)), this,
		SLOT(OnWebSocketTextMessageReceived(const QString &)));
	connect(d->m_websocket,
		SIGNAL(binaryMessageReceived(const QByteArray &)), this,
		SLOT(OnWebSocketBinaryMessageReceived(const QByteArray &)));
	d->m_websocket->open(QUrl(url));
	d->m_strUrl = url;
}

void QStompClient::Finish() {
	P_D(QStompClient);
	Unsubscribe();
	if (d->m_websocket != NULL) {
		d->m_websocket->close();
		d->m_websocket->deleteLater();
		d->m_websocket = nullptr;
	}
}

void QStompClient::setSocket(QTcpSocket *socket)
{
	P_D(QStompClient);
	if (d->m_socket != NULL && d->m_socket->parent() == this)
		delete d->m_socket;
	d->m_socket = socket;
	connect(d->m_socket, SIGNAL(connected()), this, SLOT(on_socketConnected()));
	connect(d->m_socket, SIGNAL(disconnected()), this, SLOT(on_socketDisconnected()));
	connect(d->m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
	connect(d->m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(on_socketError(QAbstractSocket::SocketError)));
	connect(d->m_socket, SIGNAL(readyRead()), this, SLOT(on_socketReadyRead()));
}

QTcpSocket * QStompClient::socket() const
{
	const P_D(QStompClient);
	return d->m_socket;
}

void QStompClient::sendFrame(const QStompRequestFrame &frame)
{
	P_D(QStompClient);
	if (d->m_socket == NULL || d->m_socket->state() != QAbstractSocket::ConnectedState)
		return;
	QByteArray serialized = frame.toByteArray();
	serialized.append('\0');
	serialized.append('\n');
	d->m_socket->write(serialized);
}

void QStompClient::sendWebFrame(const QStompRequestFrame &frame)
{
	P_D(QStompClient);
	if (d->m_websocket == NULL ||
	    d->m_websocket->state() != QAbstractSocket::ConnectedState)
		return;
	QByteArray serialized = frame.toByteArray();
	serialized.append('\0');
	serialized.append('\n');
	d->m_websocket->sendBinaryMessage(serialized);
}

void QStompClient::login(const QByteArray &user, const QByteArray &password)
{
	QStompRequestFrame frame(QStompRequestFrame::RequestConnect);
	frame.setHeaderValue("login", user);
	frame.setHeaderValue("passcode", password);
	this->sendFrame(frame);
}

void QStompClient::logout()
{
	this->sendFrame(QStompRequestFrame(QStompRequestFrame::RequestDisconnect));
}

void QStompClient::ConnectFrame(const STCahtTokenRsp &rsp)
{
	P_D(QStompClient);
	m_stCahtTokenRsp = rsp;
	QStompRequestFrame frame(QStompRequestFrame::RequestConnect);
	frame.setContentEncoding("application/json;charset=utf-8");
	frame.setHeaderValue("Access-token", rsp.token.c_str());
	frame.setHeaderValue("id", rsp.stUserInfo.id.c_str());
	frame.setHeaderValue("userName", rsp.stUserInfo.nickName.c_str());
	frame.setHeaderValue("appKey", rsp.stUserInfo.appKey.c_str());
	char type[16] = {0};
	sprintf(type, "%d", rsp.stUserInfo.type);
	frame.setHeaderValue("type", type);
	frame.setHeaderValue("sourceDevice",
			     rsp.stUserInfo.sourceUserId.c_str());
	frame.setHeaderValue("ip",
			     rsp.stUserInfo.loginIp.c_str());
	frame.setHeaderValue("accept-version","1.1,1.0");
	this->sendWebFrame(frame);
}

void QStompClient::TestSend()
{
	QJsonObject root;
	root["appKey"] = QJsonValue(m_stCahtTokenRsp.stUserInfo.appKey.c_str());
	root["content"] = QJsonValue(QString("test send"));
	root["groupId"] =
		QJsonValue(QString::number(m_stCahtTokenRsp.stUserInfo.groupId));
	root["fromUserId"] = QJsonValue(m_stCahtTokenRsp.stUserInfo.id.c_str());
	root["contentType"] = QJsonValue(1);
	root["realUserNo"] = QJsonValue("");
	root["realUserName"] = QJsonValue("");
	root["fromUserType"] = QJsonValue(1);
	root["type"] = QJsonValue(1);
	QJsonDocument doc(root);
	QByteArray bytes = doc.toJson();
	SendChatMessage(bytes);
}

void QStompClient::SendChatMessage(const QByteArray &bytes) {
	QStompRequestFrame frame(QStompRequestFrame::RequestSend);
	frame.setDestination(SEND_DESTINATION.c_str());
	frame.setHeaderValue(QByteArray("token"),
			     QByteArray(m_stCahtTokenRsp.token.c_str()));
	QByteArray base64 = bytes.toBase64();
	int len = base64.size();
	frame.setRawBody(base64);
	frame.setContentLength(len);
	this->sendWebFrame(frame);
}

void QStompClient::send(const QByteArray &destination, const QString &body,
			const QByteArray &transactionId,
			const QStompHeaderList &headers)
{
	P_D(QStompClient);
	QStompRequestFrame frame(QStompRequestFrame::RequestSend);
	frame.setHeaderValues(headers);
	frame.setContentEncoding(d->m_textCodec);
	frame.setDestination(destination);
	frame.setBody(body);
	if (!transactionId.isNull())
		frame.setTransactionId(transactionId);
	this->sendWebFrame(frame);
}

void QStompClient::Subscribe() {
	if (!m_bSubscribe) {
		QStompRequestFrame frame(QStompRequestFrame::RequestSubscribe);
		frame.setHeaderValue(QByteArray("id"), QByteArray("1"));
		frame.setHeaderValue(
			QByteArray("token"),
			QByteArray(m_stCahtTokenRsp.token.c_str()));
		frame.setDestination(RECV_DESTINATION.c_str());
		frame.setAckType(QStompRequestFrame::AckClient);
		this->sendWebFrame(frame);
	}
}

void QStompClient::Unsubscribe()
{
	if (m_bSubscribe) {
		QStompRequestFrame frame(
			QStompRequestFrame::RequestUnsubscribe);
		frame.setHeaderValue(QByteArray("id"), QByteArray("1"));
		this->sendWebFrame(frame);
		m_bSubscribe = false;
	}
}

void QStompClient::subscribe(const QByteArray &destination, bool autoAck, const QStompHeaderList &headers)
{
	QStompRequestFrame frame(QStompRequestFrame::RequestSubscribe);
	frame.setHeaderValues(headers);
	frame.setDestination(destination);
	
	if (autoAck)
		frame.setAckType(QStompRequestFrame::AckAuto);
	else
		frame.setAckType(QStompRequestFrame::AckClient);
	this->sendWebFrame(frame);

}

void QStompClient::unsubscribe(const QByteArray &destination, const QStompHeaderList &headers)
{
	QStompRequestFrame frame(QStompRequestFrame::RequestUnsubscribe);
	frame.setHeaderValues(headers);
	frame.setDestination(destination);
	this->sendWebFrame(frame);
}

void QStompClient::commit(const QByteArray &transactionId, const QStompHeaderList &headers)
{
	QStompRequestFrame frame(QStompRequestFrame::RequestCommit);
	frame.setHeaderValues(headers);
	frame.setTransactionId(transactionId);
	this->sendFrame(frame);
}

void QStompClient::begin(const QByteArray &transactionId, const QStompHeaderList &headers)
{
	QStompRequestFrame frame(QStompRequestFrame::RequestBegin);
	frame.setHeaderValues(headers);
	frame.setTransactionId(transactionId);
	this->sendFrame(frame);
}

void QStompClient::abort(const QByteArray &transactionId, const QStompHeaderList &headers)
{
	QStompRequestFrame frame(QStompRequestFrame::RequestAbort);
	frame.setHeaderValues(headers);
	frame.setTransactionId(transactionId);
	this->sendFrame(frame);
}

void QStompClient::ack(const QByteArray &messageId, const QByteArray &transactionId, const QStompHeaderList &headers)
{
	QStompRequestFrame frame(QStompRequestFrame::RequestAck);
	frame.setHeaderValues(headers);
	frame.setMessageId(messageId);
	if (!transactionId.isNull())
		frame.setTransactionId(transactionId);
	this->sendFrame(frame);
}

int QStompClient::framesAvailable() const
{
	const P_D(QStompClient);
	return d->m_framebuffer.size();
}

QStompResponseFrame QStompClient::fetchFrame()
{
	P_D(QStompClient);
	if (d->m_framebuffer.size() > 0)
		return d->m_framebuffer.takeFirst();
	else
		return QStompResponseFrame();
}

QList<QStompResponseFrame> QStompClient::fetchAllFrames()
{
	P_D(QStompClient);
	QList<QStompResponseFrame> frames = d->m_framebuffer;
	d->m_framebuffer.clear();
	return frames;
}

QAbstractSocket::SocketState QStompClient::socketState() const
{
	const P_D(QStompClient);
	if (d->m_socket == NULL)
		return QAbstractSocket::UnconnectedState;
	return d->m_socket->state();
}

QAbstractSocket::SocketError QStompClient::socketError() const
{
	const P_D(QStompClient);
	if (d->m_socket == NULL)
		return QAbstractSocket::UnknownSocketError;
	return d->m_socket->error();
}

QString QStompClient::socketErrorString() const
{
	const P_D(QStompClient);
	if (d->m_socket == NULL)
		return QLatin1String("No socket");
	return d->m_socket->errorString();
}

QByteArray QStompClient::contentEncoding()
{
	P_D(QStompClient);
	return d->m_textCodec->name();
}

void QStompClient::setContentEncoding(const QByteArray & name)
{
	P_D(QStompClient);
	d->m_textCodec = QTextCodec::codecForName(name);
}

void QStompClient::setContentEncoding(const QTextCodec * codec)
{
	P_D(QStompClient);
	d->m_textCodec = codec;
}

void QStompClient::disconnectFromHost()
{
	P_D(QStompClient);
	if (d->m_socket != NULL)
		d->m_socket->disconnectFromHost();
}

void QStompClient::OnSocketConnected()
{
}

void QStompClient::OnWebSocketConnected() {
	P_D(QStompClient);
	m_bConnect = true;
	m_pTimerHeart->stop();
	emit websocketConnected();
}
void QStompClient::OnWebSocketDisconnected() {
	m_bConnect = false;
	m_pTimerHeart->start();
	emit websocketDisConnected();
}

void QStompClient::OnTimeout() {
	P_D(QStompClient);
	if (d->m_websocket != NULL) {
		if ((d->m_websocket->state() !=
			     QAbstractSocket::ConnectedState ||
		     d->m_websocket->state() !=
			     QAbstractSocket::ConnectingState)) {
#if 0
			ConnectWebSocket(QString(
				"ws://47.103.97.102:8081/weim/websocket"));
#else
			ConnectWebSocket(QString(
				"wss://im.keepfun.cn/apis/message/weim/websocket"));
#endif
		}
	}
}

void QStompClient::OnWebSocketTextMessageReceived(const QString &message)
{
	P_D(QStompClient);
	QByteArray bytes = message.toLocal8Bit();
	d->OnWebSocketRecv(bytes);
}
void QStompClient::OnWebSocketBinaryMessageReceived(const QByteArray& bytearray) {
	P_D(QStompClient);
	d->OnWebSocketRecv(bytearray);
}

void QStompClient::OnFrameReceived() {
	P_D(QStompClient);
	QStompResponseFrame rspFrame = d->TakeResponseFrame();
	switch (rspFrame.type())
	{
		case QStompResponseFrame::ResponseInvalid:{

		} break;
		case QStompResponseFrame::ResponseConnected:
		{
			m_frameConnected = rspFrame;
			Unsubscribe();
			Subscribe();
			QTimer::singleShot(500, [&]() { TestSend(); });
			
		} break;
		case QStompResponseFrame::ResponseMessage:
		{
		} break;
		case QStompResponseFrame::ResponseReceipt:
		{
			qDebug() << "ResponseReceipt";
		}
		case QStompResponseFrame::ResponseError:
		{
		} break;
		default:
			break;
	}
	emit messageFrame(rspFrame);
}

void QStompClientPrivate::_q_socketReadyRead()
{
	P_Q(QStompClient);
	QByteArray data = this->m_socket->readAll();
	this->m_buffer.append(data);
	qDebug() << "data:" << data;
	quint32 length;
	bool gotOne = false;
	while ((length = this->findMessageBytes())) {
		QStompResponseFrame frame(this->m_buffer.left(length));
		if (frame.isValid()) {
			this->m_framebuffer.append(frame);
			gotOne = true;
		}
		else
			qDebug("QStomp: Invalid frame received!");
		this->m_buffer.remove(0, length);
	}
	if (gotOne)
		emit q->frameReceived();
}

void QStompClientPrivate::OnWebSocketRecv(const QByteArray& byte) {
	P_Q(QStompClient);
	this->m_buffer.append(byte);
	qDebug() << "byte:" << byte;
	quint32 length;
	bool gotOne = false;
	while ((length = this->findMessageBytes())) {
		QStompResponseFrame frame(this->m_buffer.left(length));
		if (frame.isValid()) {
			std::lock_guard<std::mutex> lock(m_mutexResp);
			this->m_framebuffer.append(frame);
			gotOne = true;
		} else
			qDebug("QStomp: Invalid frame received!");
		this->m_buffer.remove(0, length);
	}
	if (gotOne)
		emit q->frameReceived();
}


quint32 QStompClientPrivate::findMessageBytes()
{
	// Buffer sanity check
	forever {
		if (this->m_buffer.isEmpty())
			return 0;
		int nl = this->m_buffer.indexOf('\n');
		if (nl == -1)
			break;
		QByteArray cmd = this->m_buffer.left(nl);
		if (VALID_COMMANDS.contains(cmd))
			break;
		else {
			qDebug("QStomp: Framebuffer corrupted, repairing...");
			int syncPos = this->m_buffer.indexOf(QByteArray("\0\n", 2));
			if (syncPos != -1)
				this->m_buffer.remove(0, syncPos+2);
			else {
				syncPos = this->m_buffer.indexOf(QByteArray("\0", 1));
				if (syncPos != -1)
					this->m_buffer.remove(0, syncPos+1);
				else
					this->m_buffer.clear();
			}
		}
	}

	// Look for content-length
	int headerEnd = this->m_buffer.indexOf("\n\n");
	int clPos = this->m_buffer.indexOf("\ncontent-length");
	if (clPos != -1 && headerEnd != -1 && clPos < headerEnd) {
		int colon = this->m_buffer.indexOf(':', clPos);
		int nl = this->m_buffer.indexOf('\n', clPos);
		if (colon != -1 && nl != -1 && nl > colon) {
			bool ok = false;
			quint32 cl = this->m_buffer.mid(colon, nl-colon).toUInt(&ok) + headerEnd+2;
			if (ok) {
				if ((quint32)this->m_buffer.size() >= cl)
					return cl;
				else
					return 0;
			}
		}
	}

	// No content-length, look for \0\n
	int end = this->m_buffer.indexOf(QByteArray("\0\n", 2));
	if (end == -1) {
		// look for \0
		end = this->m_buffer.indexOf(QByteArray("\0", 1));
		if (end == -1)
			return 0;
		else
			return (quint32) end+1;
	}
	else
		return (quint32) end+2;
}

QStompResponseFrame QStompClientPrivate::TakeResponseFrame() {
	QStompResponseFrame frame;
	int count = this->m_framebuffer.count();
	qDebug() << "count:" << count;
	if (this->m_framebuffer.count() > 0) {
		std::lock_guard<std::mutex> lock(m_mutexResp);
		frame = this->m_framebuffer.takeAt(0);
	}
	return frame;
}
