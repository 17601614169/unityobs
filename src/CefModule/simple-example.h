#ifndef SIMPLE_EXAMPLE_H_2020_12_06_00_54
#define SIMPLE_EXAMPLE_H_2020_12_06_00_54
#include "QCefViewSDK/include/QCefView.h"
#include "cefmodule_global.h"
class CEFMODULE_EXPORT SimpleExample : public QCefView {
	Q_OBJECT
public:
	SimpleExample(const QString url, QWidget* parent = 0);
	~SimpleExample();
protected:
	virtual void onQCefUrlRequest(const QString& url) override;

	virtual void onQCefQueryRequest(const QCefQuery& query) override;

	virtual void onInvokeMethodNotify(int browserId, int frameId,
		const QString& method,
		const QVariantList& arguments) override;
};

#endif
