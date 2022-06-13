#include "simple-example.h"

SimpleExample::SimpleExample(const QString url, QWidget* parent)
	:QCefView(url, parent) {

}

SimpleExample::~SimpleExample() {

}

void SimpleExample::onQCefUrlRequest(const QString& url) {
	QString title("QCef Url Request");
	QString text = QString(
		"Current Thread: QT_UI\r\n"
		"Url: %1").arg(url);
}

void SimpleExample::onQCefQueryRequest(const QCefQuery& query) {
	QString title("QCef Query Request");
	QString text = QString(
		"Current Thread: QT_UI\r\n"
		"Query: %1").arg(query.reqeust());

	QString response = query.reqeust().toUpper();
	query.setResponseResult(true, response);
	responseQCefQuery(query);
}

void SimpleExample::onInvokeMethodNotify(int browserId, int frameId,
	const QString& method,
	const QVariantList& arguments) {
	
}