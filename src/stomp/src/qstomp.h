#ifndef QSTOMP_H
#define QSTOMP_H

#include "qstomp_global.h"
#include "base-define.h"
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QPair>
#include <QtNetwork/QAbstractSocket>

class QTcpSocket;
class QWebSocket;
class QAuthenticator;
class QTextCodec;

class QStompFramePrivate;
class QStompResponseFramePrivate;
class QStompRequestFramePrivate;
class QStompClientPrivate;
class QTimer;

typedef QList< QPair<QByteArray, QByteArray> > QStompHeaderList;

class QSTOMP_SHARED_EXPORT QStompFrame
{
	P_DECLARE_PRIVATE(QStompFrame)
public:
	virtual ~QStompFrame();

	QStompFrame &operator=(const QStompFrame &other);

	void setHeaderValue(const QByteArray &key, const QByteArray &value);
	void setHeaderValues(const QStompHeaderList &values);
	void addHeaderValue(const QByteArray &key, const QByteArray &value);
	QStompHeaderList header() const;
	bool headerHasKey(const QByteArray &key) const;
	QList<QByteArray> headerKeys() const;
	QByteArray headerValue(const QByteArray &key) const;
	QList<QByteArray> allHeaderValues(const QByteArray &key) const;
	void removeHeaderValue(const QByteArray &key);
	void removeAllHeaderValues(const QByteArray &key);

	bool hasContentLength() const;
	uint contentLength() const;
	void setContentLength(uint len);
	bool hasContentType() const;
	QByteArray contentType() const;
	void setContentType(const QByteArray &type);
	bool hasContentEncoding() const;
	QByteArray contentEncoding() const;
	void setContentEncoding(const QByteArray & name);
	void setContentEncoding(const QTextCodec * codec);

	virtual QByteArray toByteArray() const;
	bool isValid() const;

	QString body() const;
	QByteArray rawBody() const;

	void setBody(const QString &body);
	void setRawBody(const QByteArray &body);

protected:
	virtual bool parseHeaderLine(const QByteArray &line, int number);
	bool parse(const QByteArray &str);
	void setValid(bool);

protected:
	QStompFrame(QStompFramePrivate * d);
	QStompFrame(const QStompFrame &other, QStompFramePrivate * d);

	QStompFramePrivate * const pd_ptr;
};

class QSTOMP_SHARED_EXPORT QStompResponseFrame : public QStompFrame
{
	P_DECLARE_PRIVATE(QStompResponseFrame)
public:
	enum ResponseType {
		ResponseInvalid = 0,
		ResponseConnected,
		ResponseMessage,
		ResponseReceipt,
		ResponseError
	};

	QStompResponseFrame();
	QStompResponseFrame(const QStompResponseFrame &other);
	QStompResponseFrame(const QByteArray &frame);
	QStompResponseFrame(ResponseType type);
	QStompResponseFrame &operator=(const QStompResponseFrame &other);

	void setType(ResponseType type);
	ResponseType type() const;

	bool hasDestination() const;
	QByteArray destination() const;
	void setDestination(const QByteArray &value);

	bool hasSubscriptionId() const;
	QByteArray subscriptionId() const;
	void setSubscriptionId(const QByteArray &value);

	bool hasMessageId() const;
	QByteArray messageId() const;
	void setMessageId(const QByteArray &value);

	bool hasReceiptId() const;
	QByteArray receiptId() const;
	void setReceiptId(const QByteArray &value);

	bool hasMessage() const;
	QByteArray message() const;
	void setMessage(const QByteArray &value);

	QByteArray toByteArray() const;

protected:
	bool parseHeaderLine(const QByteArray &line, int number);
};

class QSTOMP_SHARED_EXPORT QStompRequestFrame : public QStompFrame
{
	P_DECLARE_PRIVATE(QStompRequestFrame)
public:
	enum RequestType {
		RequestInvalid = 0,
		RequestConnect,
		RequestSend,
		RequestSubscribe,
		RequestUnsubscribe,
		RequestBegin,
		RequestCommit,
		RequestAbort,
		RequestAck,
		RequestDisconnect
	};
	enum AckType {
		AckAuto = 0,
		AckClient
	};

	QStompRequestFrame();
	QStompRequestFrame(const QStompRequestFrame &other);
	QStompRequestFrame(const QByteArray &frame);
	QStompRequestFrame(RequestType type);
	QStompRequestFrame &operator=(const QStompRequestFrame &other);

	void setType(RequestType type);
	RequestType type() const;

	bool hasDestination() const;
	QByteArray destination() const;
	void setDestination(const QByteArray &value);

	bool hasTransactionId() const;
	QByteArray transactionId() const;
	void setTransactionId(const QByteArray &value);

	bool hasMessageId() const;
	QByteArray messageId() const;
	void setMessageId(const QByteArray &value);

	bool hasReceiptId() const;
	QByteArray receiptId() const;
	void setReceiptId(const QByteArray &value);

	bool hasAckType() const;
	AckType ackType() const;
	void setAckType(AckType type);

	bool hasSubscriptionId() const;
	QByteArray subscriptionId() const;
	void setSubscriptionId(const QByteArray &value);

	QByteArray toByteArray() const;

protected:
	bool parseHeaderLine(const QByteArray &line, int number);
};

class QSTOMP_SHARED_EXPORT QStompClient : public QObject
{
	Q_OBJECT
	P_DECLARE_PRIVATE(QStompClient)
public:

	explicit QStompClient(QObject *parent = 0);
	virtual ~QStompClient();

	enum Error {
		NoError,
		UnknownError,
		HostNotFound,
		ConnectionRefused,
		UnexpectedClose
	};

	void connectToHost(const QString &hostname, quint16 port = 61613);
	void ConnectWebSocket(const QString &url);
	void Finish();
	void setSocket(QTcpSocket *socket);
	QTcpSocket * socket() const;

	void sendFrame(const QStompRequestFrame &frame);
	void sendWebFrame(const QStompRequestFrame &frame);
	void Subscribe();
	void Unsubscribe();
	void ConnectFrame(const STCahtTokenRsp &);

	void login(const QByteArray &user = QByteArray(), const QByteArray &password = QByteArray());
	void logout();
	void send(const QByteArray &destination, const QString &body, const QByteArray &transactionId = QByteArray(), const QStompHeaderList &headers = QStompHeaderList());
	void subscribe(const QByteArray &destination, bool autoAck, const QStompHeaderList &headers = QStompHeaderList());
	void unsubscribe(const QByteArray &destination, const QStompHeaderList &headers = QStompHeaderList());
	void commit(const QByteArray &transactionId, const QStompHeaderList &headers = QStompHeaderList());
	void begin(const QByteArray &transactionId, const QStompHeaderList &headers = QStompHeaderList());
	void abort(const QByteArray &transactionId, const QStompHeaderList &headers = QStompHeaderList());
	void ack(const QByteArray &messageId, const QByteArray &transactionId = QByteArray(), const QStompHeaderList &headers = QStompHeaderList());

	int framesAvailable() const;
	QStompResponseFrame fetchFrame();
	QList<QStompResponseFrame> fetchAllFrames();

	QAbstractSocket::SocketState socketState() const;
	QAbstractSocket::SocketError socketError() const;
	QString socketErrorString() const;

	QByteArray contentEncoding();
	void setContentEncoding(const QByteArray & name);
	void setContentEncoding(const QTextCodec * codec);
	void TestSend();
	void SendChatMessage(const QByteArray& bytes);
public Q_SLOTS:
	void disconnectFromHost();
	void OnSocketConnected();

	void OnWebSocketConnected();
	void OnWebSocketDisconnected();
	void OnWebSocketTextMessageReceived(const QString &);
	void OnWebSocketBinaryMessageReceived(const QByteArray &);
	void OnFrameReceived();
	void OnTimeout();
Q_SIGNALS:
	void socketConnected();
	void socketDisconnected();
	void socketError(QAbstractSocket::SocketError);
	void socketStateChanged(QAbstractSocket::SocketState);
	void websocketConnected();
	void websocketDisConnected();
	void frameReceived();
	void messageFrame(const QStompResponseFrame & frame);

private:
	QStompClientPrivate * const pd_ptr;
	Q_PRIVATE_SLOT(pd_func(), void _q_socketReadyRead());
	Q_PRIVATE_SLOT(pd_func(), void OnWebSocketRecv(const QByteArray &byte));
	STCahtTokenRsp m_stCahtTokenRsp;
	QStompResponseFrame m_frameConnected;
	bool m_bSubscribe;
	QTimer *m_pTimerHeart;
	bool m_bConnect;
};

// Include private header so MOC won't complain
#ifdef QSTOMP_P_INCLUDE
#  include "qstomp_p.h"
#endif

#endif // QSTOMP_H
