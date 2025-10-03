#ifndef RESTCLASS_H
#define RESTCLASS_H

#include <QDefs>
#include "rest_helpertypes.h"
#include "restreply.h"
#include "restrequestbuilder.h"

Q_DECLARE_OPAQUE_POINTER(RestClient*)
class RestClass: public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_PTR_PROPERTY(RestClient, client, Client, nullptr)
    Q_WRITABLE_REF_PROPERTY(QString, path, Path, {})
    Q_WRITABLE_VAR_PROPERTY(bool, replyAutoDelete, ReplyAutoDelete, true)
    Q_WRITABLE_VAR_PROPERTY(bool, replyAllowEmpty, ReplyAllowEmpty, false)
    Q_WRITABLE_VAR_PROPERTY(int, replyTimeout, ReplyTimeout, 0)

    Q_READONLY_VAR_PROPERTY(bool, loading, Loading, false)

    Q_READONLY_VAR_PROPERTY(int, downloadProgress, DownloadProgress, 0)
    Q_READONLY_VAR_PROPERTY(int, uploadProgress, UploadProgress, 0)
    Q_READONLY_VAR_PROPERTY(int, httpStatus, HttpStatus, 0)
    Q_READONLY_VAR_PROPERTY(int, errorCode, ErrorCode, 0)
    Q_READONLY_VAR_PROPERTY(RestReplyErrorTypes::Enum, errorType, ErrorType, RestReplyErrorTypes::Unknown)
    Q_READONLY_REF_PROPERTY(QString, errorString, ErrorString, "")
    Q_READONLY_VAR_PROPERTY(QNetworkReply::NetworkError, networkError, NetworkError, QNetworkReply::NoError)
    Q_READONLY_REF_PROPERTY(QVariant, reply, Reply, {})

public:
    explicit RestClass(QObject *parent=nullptr);
    explicit RestClass(const QString& path, QObject *parent=nullptr);
    explicit RestClass(const QString& path, RestClient* client, QObject *parent=nullptr);

    RestClass *subClass(const QString &path, QObject *parent = nullptr);

    virtual RestRequestBuilder builder();

    Q_INVOKABLE RestReply* get(const QString &methodPath, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* get(const QVariantMap &parameters = {}, const QVariantMap &headers = {});

    Q_INVOKABLE RestReply* post(const QString &methodPath, const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* post(const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});

    Q_INVOKABLE RestReply* put(const QString &methodPath, const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* put(const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});

    Q_INVOKABLE RestReply* deleteResource(const QString &methodPath, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* deleteResource(const QVariantMap &parameters = {}, const QVariantMap &headers = {});

    Q_INVOKABLE RestReply* patch(const QString &methodPath, const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* patch(const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});

    Q_INVOKABLE RestReply* head(const QString &methodPath, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* head(const QVariantMap &parameters = {}, const QVariantMap &headers = {});

    Q_INVOKABLE RestReply* options(const QString &methodPath, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* options(const QVariantMap &parameters = {}, const QVariantMap &headers = {});

    RestReply* call(QNetworkReply* networkReply, QObject* parent=nullptr);
    RestReply* call(const QByteArray &verb, const QString &methodPath, const QVariantMap &parameters = {}, const QVariantMap &headers = {}, bool paramsAsBody = false);
    RestReply* call(const QByteArray &verb, const QString &methodPath, const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* call(const QByteArray &verb, const QVariantMap &parameters = {}, const QVariantMap &headers = {}, bool paramsAsBody = false);
    RestReply* call(const QByteArray &verb, const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});
    RestReply* call(const QByteArray &verb, const QUrl &relativeUrl, const QVariantMap &parameters = {}, const QVariantMap &headers = {}, bool paramsAsBody = false);
    RestReply* call(const QByteArray &verb, const QUrl &relativeUrl, const QVariant &body, const QVariantMap &parameters = {}, const QVariantMap &headers = {});

private:
    QNetworkReply* create(RestRequestBuilder& builder);
    QNetworkReply* create(const QByteArray &verb, const QString &methodPath, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers);
    QNetworkReply* create(const QByteArray &verb, const QString &methodPath, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody);
    QNetworkReply* create(const QByteArray &verb, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody);
    QNetworkReply* create(const QByteArray &verb, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers);
    QNetworkReply* create(const QByteArray &verb, const QUrl &relativeUrl, const QVariantMap &parameters, const QVariantMap &headers, bool paramsAsBody);
    QNetworkReply* create(const QByteArray &verb, const QUrl &relativeUrl, const QVariant &body, const QVariantMap &parameters, const QVariantMap &headers);

    RestRequestBuilder m_lastRequestBuilder;
};

#endif // RESTCLASS_H
