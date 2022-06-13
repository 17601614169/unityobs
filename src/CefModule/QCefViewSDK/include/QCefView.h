#ifndef QCEFVIEW_H
#define QCEFVIEW_H
#pragma once

#pragma region std_headers
#include <memory>
#pragma endregion std_headers

#pragma region qt_headers
#include <QtCore/qglobal.h>
#include <QWidget>
#include <QVariantList>
#pragma endregion qt_headers

#include "QCefQuery.h"
#include "QCefEvent.h"

#ifdef QCEFVIEW_LIB
#define QCEFVIEW_EXPORT Q_DECL_EXPORT
#else
#define QCEFVIEW_EXPORT Q_DECL_IMPORT
#endif

/** Outline of QCefView:
 **
 **		+--------------------------------------------------------------+
 **		| QCefView(QWidget)                                            |
 **		|  						                                       |
 **		|    +----------------------------------------------------+    |
 **		|    | WindowWrapper(QWidget)                             |    |
 **		|    |                                                    |    |
 **		|    |    +-------------------------------------------+   |    |
 **		|    |	  | CefWindow(QWindow)                        |   |    |
 **		|    |	  |					                          |   |    |
 **		|    |	  |					                          |   |    |
 **		|    |	  |					                          |   |    |
 **		|    |	  +-------------------------------------------+   |    |
 **		|    |                                                    |    |
 **		|    +----------------------------------------------------+    |
 **		|                                                              |
 **		+--------------------------------------------------------------+
 **
 **	Remarks:
 **		The WindowWrapper and CefWindow are transparent to upper layer user.
 **
 **/

/// <summary>
/// 
/// </summary>
class QCEFVIEW_EXPORT QCefView
	: public QWidget
{
	/// <summary>
	/// 
	/// </summary>
	Q_OBJECT

public:
	//This enum defines the open policies a webpage can have when activating links and emitting the linkClicked() signal.
	//要在自己定制的窗口打开链接的话，用默认值就可以了，需要绑定linkClicked()信号。
	enum LinkOpenPolicy
	{
		// 不自动打开链接，并且emit linkClicked()信号.(default)
		eLinkDontOpenByCef,
		// 在cef source frame中打开链接.
		eLinkOpenInSourceFrame,
		// 在cef新窗口中打开链接.(只有这种不会拦截cef的popup窗口)
		eLinkOpenInNewWindow,
		// 在默认浏览器中打开链接.
		eLinkOpenInLocalBrowser,
	};

public:
	/// <summary>
	/// 
	/// </summary>
	QCefView(const QString url, QWidget* parent = 0, bool osr = false);

	/// <summary>
	/// 
	/// </summary>
	~QCefView();

	/// <summary>
	/// 
	/// </summary>
	/// <returns>WId</returns>
	WId getCefWinId();

	/// <summary>
	/// 
	/// </summary>
	void closeAllBrowsers();

	/// <summary>
	/// Navigates to the content.
	/// </summary>
	/// <param name="content"></param>
	/// <param name="url"></param>
	void navigateToString(const QString& content, const QString& url);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="url"></param>
	void navigateToUrl(const QString& url);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool browserCanGoBack();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool browserCanGoForward();

	/// <summary>
	/// 
	/// </summary>
	void browserGoBack();

	/// <summary>
	/// 
	/// </summary>
	void browserGoForward();

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	bool browserIsLoading();

	/// <summary>
	/// 
	/// </summary>
	void browserReload();

	/// <summary>
	/// 
	/// </summary>
	void browserStopLoad();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="frameId"></param>
	/// <param name="name"></param>
	/// <param name="event"></param>
	/// <returns></returns>
	bool triggerEvent(int frameId, const QString& name, const QCefEvent& event);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="name"></param>
	/// <param name="event"></param>
	/// <returns></returns>
	bool broadcastEvent(const QString& name, const QCefEvent& event);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="query"></param>
	/// <returns></returns>
	bool responseQCefQuery(const QCefQuery& query);

public:
	/////// Add by vilas, 2018/4/3 ///////////////////////////////////////
	virtual void setCefContextMenuEnable(bool bEnable);

	// Osr模式下，需要同时设置setLinkOpenPolicy(QCefView::eLinkOpenInNewWindow).
	virtual void setCefDevToolsEnable(bool bEnable);

	// exec js.
	virtual void execJaveScript(const QString& js);

	//详情参见 LinkOpenPolicy
	virtual void setLinkOpenPolicy(LinkOpenPolicy policy);

	/* Fun: 设置滚动条样式
	// example: 
		setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
		setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
	*/
	virtual void setScrollBarPolicy(Qt::Orientation orientation, Qt::ScrollBarPolicy policy);

protected:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="event"></param>
	void changeEvent(QEvent * event) Q_DECL_OVERRIDE;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="watched"></param>
	/// <param name="event"></param>
	/// <returns></returns>
	bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

protected Q_SLOTS:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="isLoading"></param>
	/// <param name="canGoBack"></param>
	/// <param name="canGoForward"></param>
	virtual void onLoadingStateChanged(bool isLoading, bool canGoBack, bool canGoForward);

	/// <summary>
	/// 
	/// </summary>
	virtual void onLoadStart(const QString& url, bool bIsMainFrame);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="httpStatusCode"></param>
	virtual void onLoadEnd(int httpStatusCode, bool bIsMainFrame);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="errorCode"></param>
	/// <param name="errorMsg"></param>
	/// <param name="failedUrl"></param>
	virtual void onLoadError(int errorCode, const QString& errorMsg, const QString& failedUrl, bool bIsMainFrame);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="exitCode"></param>
	virtual void onRenderProcessTerminated(int exitCode);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="url"></param>
	virtual void onQCefUrlRequest(const QString& url);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="query"></param>
	virtual void onQCefQueryRequest(const QCefQuery& query);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="browserId"></param>
	/// <param name="frameId"></param>
	/// <param name="method"></param>
	/// <param name="arguments"></param>
	virtual void onInvokeMethodNotify(int browserId, int frameId, const QString& method, const QVariantList& arguments);

Q_SIGNALS:
	// For cef close procedure.
	void mainBrowerOnClose();

	// 链接点击的url. see: void setLinkOpenPolicy()
	void linkClicked(const QString & url);

	// 起凡协议网页链接信号
	void qfProtocolUrlNotify(const QString& strUrl);

private:
	/// <summary>
	/// 
	/// </summary>
	class Implementation;
	std::unique_ptr<Implementation> pImpl_;
	bool osr_;
	QMap<Qt::Orientation, Qt::ScrollBarPolicy> m_mapScrollBarPolicy;
};

#endif // QCEFVIEW_H
