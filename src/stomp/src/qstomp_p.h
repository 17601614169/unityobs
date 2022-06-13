#ifndef QSTOMP_P_H
#define QSTOMP_P_H

#include <QtCore/QObject>
#include <mutex>
class QStompFramePrivate
{
public:
	QStompHeaderList m_header;
	bool m_valid;
	QByteArray m_body;
	const QTextCodec * m_textCodec;
};

class QStompResponseFramePrivate : public QStompFramePrivate
{
public:
	QStompResponseFrame::ResponseType m_type;
};

class QStompRequestFramePrivate : public QStompFramePrivate
{
public:
	QStompRequestFrame::RequestType m_type;
};

class QStompClientPrivate
{
	P_DECLARE_PUBLIC(QStompClient)
public:
	QStompClientPrivate(QStompClient * q) : pq_ptr(q) {}
	QString m_strUrl;
	QTcpSocket * m_socket;
	QWebSocket *m_websocket;
	const QTextCodec * m_textCodec;

	QByteArray m_buffer;
	QList<QStompResponseFrame> m_framebuffer;

	quint32 findMessageBytes();

	void _q_socketReadyRead();
	void OnWebSocketRecv(const QByteArray& byte);
	QStompResponseFrame TakeResponseFrame();
private:
	QStompClient * const pq_ptr;
	std::mutex m_mutexResp;
};

#endif // QSTOMP_P_H
