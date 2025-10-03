#ifndef HAPPYREPLY_H
#define HAPPYREPLY_H

#include <QDefs>
#include <Rest>
#include <QHttpServerResponse>

#include "happyhttpheaders.h"

class HappyReply
{
    Q_GADGET

    Q_MEMBER_PROPERTY(bool, result, true)
    Q_MEMBER_PROPERTY(QByteArray, rawData, {})
    Q_MEMBER_PROPERTY(QVariant, variant, {})
    Q_MEMBER_PROPERTY(QString, message, {})
    Q_MEMBER_PROPERTY(QHttpServerResponse::StatusCode, status, {})
    Q_MEMBER_PROPERTY(QString, action, {})

public:
    ~HappyReply() = default;
    HappyReply() = default;
    HappyReply(const HappyReply& other) = default;
    HappyReply(HappyReply&& other) = default;
    HappyReply(QByteArray&& value, QHttpServerResponse::StatusCode status=QHttpServerResponse::StatusCode::Ok, bool result=true);
    HappyReply(QVariant&& value, QHttpServerResponse::StatusCode status=QHttpServerResponse::StatusCode::Ok, bool result=true);
    HappyReply(QVariant&& value, const QString& action, QHttpServerResponse::StatusCode status=QHttpServerResponse::StatusCode::Ok, bool result=true);

    static HappyReply success(const QString& message, QHttpServerResponse::StatusCode status);
    static HappyReply error(const QString& message, QHttpServerResponse::StatusCode status);

    QByteArray data(const HappyHttpHeaders& headers) const;
    QHttpServerResponse response(const HappyHttpHeaders& headers) const;

    HappyReply& operator=(const HappyReply &other) = default;
    bool operator==(const HappyReply&) const = default;
    bool operator!=(const HappyReply&) const = default;
};

#endif // HAPPYREPLY_H
