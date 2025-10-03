#ifndef HAPPYHTTPREQUEST_H
#define HAPPYHTTPREQUEST_H

#include <QDefs>
#include <Rest>
#include <QHttpServerRequest>

#include "happyhttpparameters.h"
#include "happyhttpheaders.h"

class HappyHttpRequest
{
    Q_GADGET
    QML_VALUE_TYPE(happyHttpRequest)
    QML_STRUCTURED_VALUE

    Q_MEMBER_PROPERTY(HappyHttpParameters, parameters, {})
    Q_MEMBER_PROPERTY(HappyHttpHeaders, headers, {})

    Q_MEMBER_PROPERTY(QUrl, url, {})
    Q_MEMBER_PROPERTY(QHttpServerRequest::Method, method, {})
    Q_MEMBER_PROPERTY(QByteArray, body, {})
    Q_MEMBER_PROPERTY(QVariant, data, {})
    Q_MEMBER_PROPERTY(QString, remoteAddress, {})
    Q_MEMBER_PROPERTY(int, remotePort, {})
    Q_MEMBER_PROPERTY(QString, localAddress, {})
    Q_MEMBER_PROPERTY(int, localPort, {})
    Q_MEMBER_PROPERTY(bool, parseOk, {})

public:
    ~HappyHttpRequest() = default;
    HappyHttpRequest() = default;
    HappyHttpRequest(const HappyHttpRequest& other) = default;
    HappyHttpRequest(const QHttpServerRequest &request);

    Q_INVOKABLE QString toString() const;

    static QVariant parseRequestBody(const QByteArray &body, const HappyHttpHeaders& headers, bool* result=nullptr);

    HappyHttpRequest& operator=(const HappyHttpRequest &other) = default;
    bool operator==(const HappyHttpRequest&) const = default;
    bool operator!=(const HappyHttpRequest&) const = default;
};

#endif // HAPPYHTTPREQUEST_H
