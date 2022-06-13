#ifndef CUSTOMCEFVIEW_H
#define CUSTOMCEFVIEW_H

#include "QCefViewSDK/include/QCefView.h"
#include <QPushButton>
#include "cefmodule_global.h"
class CEFMODULE_EXPORT CefWebView : public QCefView
{
	Q_OBJECT

public:
	CefWebView(QWidget *parent = 0, bool osr = false);
	virtual ~CefWebView();

	void load(const QString& url);
	void reload();
	bool isLoading();
	void stop();
	void back();
	void forward();

	/*//This enum defines the open policies a webpage can have when activating links and emitting the linkClicked() signal.
	//要在自己定制的窗口打开link的话，用默认值就可以了，需要绑定linkClicked()信号。
	enum LinkOpenPolicy
	{
		// (default). 不自动打开link，并且emit linkClicked()信号
		eLinkDontOpenByCef,
		// 在主frame中打开link
		eLinkOpenInSourceFrame,
		// 在新窗口中打开link
		eLinkOpenInNewWindow,
	};*/
	void setLinkOpenPolicy(QCefView::LinkOpenPolicy policy) Q_DECL_OVERRIDE;

	/* Fun: 设置滚动条样式
	// example: 
		setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
		setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	*/
	void setScrollBarPolicy(Qt::Orientation orientation, Qt::ScrollBarPolicy policy) Q_DECL_OVERRIDE;

	/* Fun: exec normal JaveScrip
	// example: 
			execJaveScript("document.body.style.overflow = 'hidden';");
	*/
	void execJaveScript(const QString& js) Q_DECL_OVERRIDE;

	/* Fun: c++ call js (custom event)
	//  c++:
	//	QCefEvent event;
	//	event.setStringProperty("hunli", "200");
	//	event.setStringProperty("hufu", "300");
	//	callJsEvent("itemChangedEvent", event);
	//
	//  js:
	//	<script> 
	//		function itemChanged(event){
	//			var hunli = event["hunli"]; 
	//			var hufu = event["hufu"];
	//		}
	//		function onLoad() {
	//			if (typeof(QCefClient) == 'undefined') { 
	//				return;
	//			}
	//			QCefClient.addEventListener("itemChangedEvent", itemChanged);
	//		}
	//	</script>
	*/
	void callJsEvent(const QString& eventName, const QCefEvent& event);

protected Q_SLOTS:
	void onLoadStart(const QString& url, bool bIsMainFrame) Q_DECL_OVERRIDE;

	void onLoadEnd(int httpStatusCode, bool bIsMainFrame) Q_DECL_OVERRIDE;

	void onLoadError(int errorCode, const QString& errorMsg, const QString& failedUrl, bool bIsMainFrame) Q_DECL_OVERRIDE;

	// 0: TS_ABNORMAL_TERMINATION; 1: TS_PROCESS_WAS_KILLED; 2: TS_PROCESS_CRASHED
	void onRenderProcessTerminated(int exitCode) Q_DECL_OVERRIDE;

	void onQCefUrlRequest(const QString& url) Q_DECL_OVERRIDE;

	void onQCefQueryRequest(const QCefQuery& query) Q_DECL_OVERRIDE;

	virtual void onInvokeMethodNotify(
		int browserId, 
		int frameId, 
		const QString& method, 
		const QVariantList& arguments) Q_DECL_OVERRIDE;

Q_SIGNALS:
	//js call c++
	void signalJsCallCxx(const QString& method, const QVariantList& arguments);

	void loadStarted(const QString& url, bool bIsMainFrame);
	void loadFinished(int httpStatusCode, bool bIsMainFrame);
	void loadError(int errorCode, const QString& errorMsg, const QString& failedUrl, bool bIsMainFrame);
	void renderProcessTerminated(int exitCode);
};


//////////////////////////////////////////////////////////////////////////
// 带关闭，刷新等按钮的CefView
//////////////////////////////////////////////////////////////////////////
class CEFMODULE_EXPORT QxCefViewContainer : public QWidget
{
	Q_OBJECT

public:
	explicit QxCefViewContainer(QWidget *parent = 0, bool bNeedToolBar = true);
	~QxCefViewContainer();

	void browerLoad(const QString& url);
	void browerReload();
	bool browerIsLoading();
	void browerStop();
	void browerBack();
	void browerForward();

	void execJaveScript(const QString& js);
	void setScrollBarPolicy(Qt::Orientation orientation, Qt::ScrollBarPolicy policy);

	void setNeedToolBar(bool need);
	void setBkground(const QString& strBg);

private:
	void initCtrls();
	void initConnections();

protected:
	void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
	CefWebView *m_pCefView;
	QPushButton *m_pBtnRefresh;
	QPushButton *m_pBtnClose;

private:
	QString m_strUrl;
	bool m_bNeedToolBar;// 是否要工具栏（刷新、关闭按钮）

	QPixmap m_pmBg;
	QPoint m_ptLastPos;
	bool m_bIsLeftPress;
};


class CEFMODULE_EXPORT WebViewWidget : public QWidget {
	Q_OBJECT
public:
	WebViewWidget(QWidget* parent=0);
	~WebViewWidget();
	void Load(const std::string & url);
	void Reload();
protected:
	virtual void resizeEvent(QResizeEvent* event) ;
private:
	CefWebView *m_pCefView;
};

/////////////////////////////////////////////////////////////////////////

#endif // CUSTOMCEFVIEW_H
