#ifndef BASEWEBWIDGET_H
#define BASEWEBWIDGET_H
#include <QWidget>
#include <QUrl>
#include <functional>
#include "qtcustom_global.h"

class QTCUSTOM_EXPORT BaseWebWidget : public QWidget {
    Q_OBJECT

public:
    explicit BaseWebWidget(QWidget* parent = nullptr);
    virtual ~BaseWebWidget();
    virtual void Load(const QString& url) = 0;
    virtual void LoadLocalFile(const QString& file_name) = 0;
    virtual void SetUrl(const QString& url) = 0;
    virtual QString Url() = 0;
    virtual void Reload() = 0;
    virtual void ForBack() = 0;
    virtual void ForWard() = 0;
    virtual void SetContent(const QByteArray& data, const QString& mimeType = QString(), const QUrl& baseUrl = QUrl())=0;
    virtual void SetHtml(const QByteArray& data,const QUrl& baseUrl = QUrl())=0;

    using JavaScriptResultCallback = std::function<void(const QVariant&)>;

    virtual bool CallJavaScript(const QString& script) = 0;
    virtual bool CallJavaScript(const QString& script, JavaScriptResultCallback* callback) = 0;
public slots:
    virtual void LoadStarted() = 0;
    virtual void LoadFinished(bool ok) = 0;
    virtual void LinkClicked(const QUrl& url) = 0;
    virtual void LoadError(int error_code) = 0;
};

#endif  // WEB_WIDGET_H_

