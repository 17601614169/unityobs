#ifndef WEBENGINE_WIDGET_H
#define WEBENGINE_WIDGET_H

#include <QtGlobal>
#include "base-web-widget.h"
#include "qtenginemodule_global.h"
#include <QWebEngineView>
class QWebChannel;
class QHBoxLayout;
class QVBoxLayout;
#include <QWebEnginePage>

#include <QObject>

class WebJsObject : public QObject
{
    Q_OBJECT
public:
    static WebJsObject *instance();
public slots:
      Q_INVOKABLE virtual QString Invoke(const QString& script);
Q_SIGNALS:
    void closeRoom();
    void enterRoomSignal(const QString & roomId,const QString & liveId);
    void liveDetailSignal(const QString & anchorName,const QString& openTimeStart,const QString& openTimeEnd);
private:
    explicit WebJsObject(QObject *parent = 0);
};

class QTENGINEMODULE_EXPORT WebEngineView : public QWebEngineView {
    Q_OBJECT
public:
    WebEngineView(QWidget* parent):QWebEngineView(parent){}
    ~WebEngineView(){
        qDebug()<<"destroy";
    }
protected:
    QWebEngineView* createWindow(QWebEnginePage::WebWindowType);
};

class QTENGINEMODULE_EXPORT WebEngineWidget : public BaseWebWidget {
    Q_OBJECT

public:
    explicit WebEngineWidget(QWidget* parent = nullptr);
    virtual ~WebEngineWidget();

    virtual void Load(const QString& url) override;
    virtual void LoadLocalFile(const QString& file_name);
    virtual void SetUrl(const QString& url)override;
    virtual QString Url()override final;
    virtual void Reload() override final;
    virtual void ForBack() override final;
    virtual void ForWard()override final;

    virtual bool CallJavaScript(const QString& script)override final;
    virtual bool CallJavaScript(const QString& script, JavaScriptResultCallback* callback) override final;
    QWebEngineView * createWindow(QWebEnginePage::WebWindowType type) ;
public slots:
    virtual void LoadStarted()override;
    virtual void LoadFinished(bool ok) override;
    virtual void LinkClicked(const QUrl& url) override;
    virtual void LoadError(int error_code) override;
    void OnUrlChanged(const QUrl&);
public:
    void SetBgTransparent();
    void ResetWebView(WebEngineView* view) ;
    void InitWebView(WebEngineView* view = nullptr) ;
    void ResetView() ;
protected:
	QWebChannel* m_pChannel;
	WebEngineView* m_pWebEngineView;
	QVBoxLayout* m_pMainLayout;
};

/**
* 具体功能都放WebBrowserWidget或者另写类
*/
class QTENGINEMODULE_EXPORT WebBrowserWidget : public WebEngineWidget {
    Q_OBJECT
public:
    explicit WebBrowserWidget(QWidget* parent = nullptr);
    ~WebBrowserWidget();
Q_SIGNALS:
    void closeRoom();
    void enterRoomSignal(const QString & roomId,const QString & liveId);
    void liveDetailSignal(const QString & anchorName,const QString& openTimeStart,const QString& openTimeEnd);
};

#endif // !WEBENGINE_WIDGET_H
