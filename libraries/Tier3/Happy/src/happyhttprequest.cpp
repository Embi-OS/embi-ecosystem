#include "happyhttprequest.h"
#include "happy_log.h"

HappyHttpRequest::HappyHttpRequest(const QHttpServerRequest &request)
{
    parameters = HappyHttpParameters(request);
    headers = HappyHttpHeaders(request);

    url = request.url();
    method = request.method();
    body = request.body();
    data = HappyHttpRequest::parseRequestBody(body, headers, &parseOk);
    remoteAddress = request.remoteAddress().toString();
    remotePort = request.remotePort();
    localAddress = request.localAddress().toString();
    localPort = request.localPort();
}

QString HappyHttpRequest::toString() const
{
    QString str;

    str.append(QString("complete network query looks like:\n"));
    str.append(QString("%1\n").arg(qLogLine()));
    str.append(QString("    URL           : %1\n").arg(url.toString()));
    if(!url.userName().isEmpty())
        str.append(QString("    URL user      : %1\n").arg(url.userName()));
    if(!url.password().isEmpty())
        str.append(QString("    URL pass      : %1\n").arg(url.password()));
    if(!url.query().isEmpty())
        str.append(QString("    URL query     : %1\n").arg(url.query()));
    str.append(QString("    Method        : %1\n").arg(QHttpServerRequest::staticMetaObject.enumerator(0).valueToKey((int)method)));
    if(!body.isEmpty())
        str.append(QString("    Body          : %1\n").arg(body));
    str.append(QString("    Headers       :\n%1").arg(headers.toString()));
    str.append(QString("%1").arg(qLogLine()));

    return str;
}

QVariant HappyHttpRequest::parseRequestBody(const QByteArray &body, const HappyHttpHeaders& headers, bool* result)
{
    RestParserError restParseError;
    QByteArray contentType = RestHelper::ContentTypeJson;
    QVariant data;

    if(!headers.accept.isEmpty())
    {
        contentType = headers.accept;
    }
    else if(!headers.contentType.isEmpty())
    {
        contentType = headers.contentType;
    }

    if (body.isEmpty() || contentType == RestHelper::ContentTypeEmpty)
    {
        // ok, nothing to do, but is here to skip the rest
    }
    else if (contentType == RestHelper::ContentTypeCbor)
    {
        QCborParserError parseError;
        data = QUtils::Cbor::cborToVariant(body, &parseError);

        if(parseError.error!=QCborError::NoError)
        {
            restParseError.code = parseError.error.c;
            restParseError.error = QString("Error loading cbor: %1").arg(parseError.errorString());
        }
    }
    else if (contentType == RestHelper::ContentTypeJson || contentType == RestHelper::ContentTypeDefault)
    {
        QJsonParseError parseError;
        data = QUtils::Json::jsonToVariant(body, &parseError);

        if (parseError.error!=QJsonParseError::NoError)
        {
            restParseError.code = parseError.error;
            restParseError.error = QString("Error loading json: %1").arg(parseError.errorString());
        }
    }
    else if (contentType == RestHelper::ContentTypeHtml)
    {
        const QString text = QString::fromUtf8(body);
        data = text;
    }
    else if (contentType == RestHelper::ContentTypeText)
    {
        const QString text = QString::fromUtf8(body);
        data = text;
    }
    else
    {
        restParseError.code = -1;
        restParseError.error = QString("Unsupported content type: %1").arg(QString::fromUtf8(contentType));

        HAPPYLOG_WARNING()<<body;
    }

    if (restParseError.code!=0)
    {
        data = QString("%1 (%2)").arg(restParseError.error).arg(restParseError.code);
    }

    if(result)
        *result = restParseError.code==0;

    return data;
}
