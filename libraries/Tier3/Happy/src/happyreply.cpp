#include "happyreply.h"

HappyReply::HappyReply(QByteArray&& value, QHttpServerResponder::StatusCode status, bool result):
    result(result),
    rawData(std::move(value)),
    variant(QVariant()),
    status(status),
    action()
{

}

HappyReply::HappyReply(QVariant&& value, QHttpServerResponder::StatusCode status, bool result):
    result(result),
    rawData(value.typeId()==QMetaType::QByteArray ? value.toByteArray() : QByteArray()),
    variant(value.typeId()==QMetaType::QByteArray ? QVariant() : std::move(value)),
    status(status),
    action()
{

}

HappyReply::HappyReply(QVariant&& value, const QString& action, QHttpServerResponse::StatusCode status, bool result):
    result(result),
    rawData(value.typeId()==QMetaType::QByteArray ? value.toByteArray() : QByteArray()),
    variant(value.typeId()==QMetaType::QByteArray ? QVariant() : std::move(value)),
    status(status),
    action(action)
{

}

HappyReply HappyReply::success(const QString& message, QHttpServerResponder::StatusCode status)
{
    HappyReply reply;
    reply.message = message;
    reply.status = status;
    reply.result = true;
    return reply;
}

HappyReply HappyReply::error(const QString& message, QHttpServerResponder::StatusCode status)
{
    HappyReply reply;
    reply.message = message;
    reply.status = status;
    reply.result = false;
    return reply;
}

QByteArray HappyReply::data(const HappyHttpHeaders& headers) const
{
    if(!result)
        QByteArray();

    QByteArray contentType = RestHelper::ContentTypeJson;

    if(!headers.accept.isEmpty())
    {
        contentType = headers.accept;
    }
    else if(!headers.contentType.isEmpty())
    {
        contentType = headers.contentType;
    }

    if(status==QHttpServerResponse::StatusCode::NoContent)
    {
        return QByteArray();
    }

    if(!rawData.isEmpty())
        return rawData;

    if(variant.isValid())
    {
        if (contentType == RestHelper::ContentTypeJson || contentType == RestHelper::ContentTypeDefault) {
            return QUtils::Json::variantToJson(variant, true);
        }
        else if (contentType == RestHelper::ContentTypeCbor) {
            return QUtils::Cbor::variantToCbor(variant, QCborValue::UseFloat16);
        }
        else {
            if(variant.typeId()==QMetaType::QByteArray)
                return variant.toByteArray();
            return variant.toString().toUtf8();
        }
    }

    return QByteArray();
}

QHttpServerResponse HappyReply::response(const HappyHttpHeaders& headers) const
{
    if(!result)
    {
        return QHttpServerResponse(message, status);
    }

    QByteArray contentType = RestHelper::ContentTypeJson;
    QByteArray data = this->data(headers);

    if(!headers.accept.isEmpty())
    {
        contentType = headers.accept;
    }
    else if(!headers.contentType.isEmpty())
    {
        contentType = headers.contentType;
    }

    return QHttpServerResponse(contentType, std::move(data), status);
}
