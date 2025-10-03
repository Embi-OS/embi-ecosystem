#include "happyhttpheaders.h"

HappyHttpHeaders::HappyHttpHeaders(const QHttpServerRequest &request)
{
    const QHttpHeaders httpHeaders = request.headers();
    const QList<QPair<QByteArray, QByteArray>> headers = httpHeaders.toListOfPairs();
    fromHeadersList(headers);
}

HappyHttpHeaders::HappyHttpHeaders(const QNetworkRequest &request)
{
    const QList<QByteArray> rawHeaderList = request.rawHeaderList();
    QList<QPair<QByteArray, QByteArray>> headers;
    for(const QByteArray& rawHeader: rawHeaderList)
        headers.append(QPair<QByteArray, QByteArray>(rawHeader, request.rawHeader(rawHeader)));
    fromHeadersList(headers);
}

void HappyHttpHeaders::fromHeadersList(const QList<QPair<QByteArray, QByteArray>>& headers)
{
    for(const QPair<QByteArray, QByteArray>& header: headers)
    {
        const QByteArray key = header.first;
        const QByteArray value = header.second;

        if(key.compare("Host", Qt::CaseInsensitive)==0)
        {
            host = value;
        }
        else if(key.compare("Accept", Qt::CaseInsensitive)==0)
        {
            accept = value;
        }
        else if(key.compare("Content-Type", Qt::CaseInsensitive)==0)
        {
            contentType = value;
        }
        else if(key.compare("Content-Length", Qt::CaseInsensitive)==0)
        {
            contentLength = value.toLongLong();
        }
        else if(key.compare("Authorization", Qt::CaseInsensitive)==0)
        {
            authorization = value;
        }
        else if(key.compare("Connection", Qt::CaseInsensitive)==0)
        {
            connection = value;
        }
        else if(key.compare("Accept-Encoding", Qt::CaseInsensitive)==0)
        {
            acceptEncoding = value.split(',');
        }
        else if(key.compare("Accept-Language", Qt::CaseInsensitive)==0)
        {
            acceptLanguage = value.split(',');
        }
        else if(key.compare("User-Agent", Qt::CaseInsensitive)==0)
        {
            userAgent = value;
        }
        else
        {
            this->headers.insert(key, value);
        }
    }
}

QString HappyHttpHeaders::toString() const
{
    QString str;

    if(!host.isEmpty())
        str.append(QString("         Host %1\n").arg(host));
    if(!accept.isEmpty())
        str.append(QString("         Accept %1\n").arg(accept));
    if(!contentType.isEmpty())
        str.append(QString("         Content-Type %1\n").arg(contentType));

    return str;
}
