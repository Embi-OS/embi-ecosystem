#ifndef HAPPYHTTPHEADERS_H
#define HAPPYHTTPHEADERS_H

#include <QDefs>
#include <QHttpServerRequest>
#include <QNetworkRequest>

class HappyHttpHeaders
{
    Q_GADGET
    QML_VALUE_TYPE(happyHttpHeaders)
    QML_STRUCTURED_VALUE

    Q_MEMBER_PROPERTY(QString, host, {})
    Q_MEMBER_PROPERTY(QByteArray, accept, {})
    Q_MEMBER_PROPERTY(QByteArray, contentType, {})
    Q_MEMBER_PROPERTY(long long, contentLength, {})
    Q_MEMBER_PROPERTY(QByteArray, authorization, {})
    Q_MEMBER_PROPERTY(QByteArray, connection, {})
    Q_MEMBER_PROPERTY(QByteArrayList, acceptEncoding, {})
    Q_MEMBER_PROPERTY(QByteArrayList, acceptLanguage, {})
    Q_MEMBER_PROPERTY(QByteArray, userAgent, {})
    Q_MEMBER_PROPERTY(QVariantMap, headers, {})

public:
    ~HappyHttpHeaders() = default;
    HappyHttpHeaders() = default;
    HappyHttpHeaders(const HappyHttpHeaders& other) = default;
    HappyHttpHeaders(const QHttpServerRequest &request);
    HappyHttpHeaders(const QNetworkRequest &request);

    void fromHeadersList(const QList<QPair<QByteArray, QByteArray>>& headers);

    Q_INVOKABLE QString toString() const;

    HappyHttpHeaders& operator=(const HappyHttpHeaders &other) = default;
    bool operator==(const HappyHttpHeaders&) const = default;
    bool operator!=(const HappyHttpHeaders&) const = default;
};

#endif // HAPPYHTTPHEADERS_H
