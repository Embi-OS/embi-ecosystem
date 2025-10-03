#ifndef RESTBROWSER_H
#define RESTBROWSER_H

#include <QDefs>
#include <Rest>
#include <QAuthenticator>
#include <QNetworkProxy>

class RestBrowser : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_PTR_PROPERTY(RestClient, client, Client, nullptr)

    Q_WRITABLE_REF_PROPERTY(QString, verb, Verb, {})
    Q_WRITABLE_REF_PROPERTY(QString, path, path, {})
    Q_WRITABLE_REF_PROPERTY(QString, bodyJson, BodyJson, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, params, Params, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, headers, Headers, {})

    Q_READONLY_REF_PROPERTY(QString, replyJson, ReplyJson, {})
    Q_READONLY_REF_PROPERTY(QString, replyUrl, ReplyUrl, {})
    Q_READONLY_REF_PROPERTY(QString, networkCode, NetworkCode, {})
    Q_READONLY_REF_PROPERTY(QString, networkError, NetworkError, {})
    Q_READONLY_REF_PROPERTY(QString, networkMessage, NetworkMessage, {})
    Q_READONLY_VAR_PROPERTY(QString, networkDebug, NetworkDebug, {})

    Q_READONLY_VAR_PROPERTY(int, upload, Upload, 0)
    Q_READONLY_VAR_PROPERTY(int, download, Download, 0)
    Q_READONLY_VAR_PROPERTY(int, parsing, Parsing, 0)

public:
    explicit RestBrowser(QObject* parent=nullptr);

public slots:
    void execute();

private slots:
    void onClientAboutToChange(RestClient* oldClient, RestClient* newClient);

    void authenticateReply(QNetworkReply *reply, QAuthenticator *auth);
    void authenticateProxy(const QNetworkProxy &proxy, QAuthenticator *auth);
    void processReply(const QVariant& value);
    void processViewText(const QVariant& value);
    void zeroBars();

private:
    QVariant parseBody() const;
};

#endif // RESTBROWSER_H
