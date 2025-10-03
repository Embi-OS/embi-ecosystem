#include "restreply.h"
#include "rest_log.h"

#ifdef QT_CONCURRENT_LIB
#include <QFuture>
#include <QtConcurrentRun>
#endif

RestReply::RestReply(QNetworkReply *networkReply, QObject *parent) :
    QObject(parent),
    m_networkReply(networkReply)
{
    connectReply();

    connect(this, &RestReply::finished, this, [this](int status){
        if(m_autoDelete)
            this->deleteLater();
    }, Qt::QueuedConnection);
}

RestReply::~RestReply()
{
    if (m_networkReply)
        m_networkReply->deleteLater();
}

RestReply* RestReply::onFinished(FinishedFunction handler, Qt::ConnectionType connection)
{
    return onFinished(this, std::move(handler), connection);
}
RestReply* RestReply::onFinished(const QObject *scope, FinishedFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::finished, scope, [xHandler = std::move(handler)](int httpStatus){
        xHandler(httpStatus);
    }, connection);

    return this;
}

RestReply* RestReply::onSucceeded(CompletedFunction handler, Qt::ConnectionType connection)
{
    return onSucceeded(this, std::move(handler), connection);
}
RestReply* RestReply::onSucceeded(const QObject *scope, CompletedFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::succeeded, scope, [xHandler = std::move(handler)](int httpStatus, const QVariant &reply){
        xHandler(httpStatus, reply);
    }, connection);

    return this;
}
RestReply* RestReply::onFailed(CompletedFunction handler, Qt::ConnectionType connection)
{
    return onFailed(this, std::move(handler), connection);
}
RestReply* RestReply::onFailed(const QObject *scope, CompletedFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::failed, scope, [xHandler = std::move(handler)](int httpStatus, const QVariant &reason){
        xHandler(httpStatus, reason);
    }, connection);

    return this;
}
RestReply* RestReply::onCanceled(CanceledFunction handler, Qt::ConnectionType connection)
{
    return onCanceled(this, std::move(handler), connection);
}
RestReply* RestReply::onCanceled(const QObject *scope, CanceledFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::canceled, scope, [xHandler = std::move(handler)](){
        xHandler();
    }, connection);

    return this;
}

RestReply *RestReply::onError(ErrorFunction handler, Qt::ConnectionType connection)
{
    return onError(this, std::move(handler), connection);
}
RestReply *RestReply::onError(const QObject *scope, ErrorFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::error, scope, [xHandler = std::move(handler)](const QString &errorString, int error, RestReplyErrorTypes::Enum type, const QVariant &reply){
        xHandler(errorString, error, type, reply);
    }, connection);

    return this;
}

RestReply *RestReply::onAllErrors(ErrorFunction handler, Qt::ConnectionType connection)
{
    return onAllErrors(this, handler, connection);
}
RestReply *RestReply::onAllErrors(const QObject *scope, ErrorFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    this->onError(scope, handler, connection);
    this->onFailed(scope, [xHandler = std::move(handler)](int httpStatus, const QVariant& reason){
        xHandler(RestHelper::parseBody(reason), httpStatus, RestReplyErrorTypes::Failure, reason);
    }, connection);

    return this;
}

RestReply *RestReply::onNetworkErrors(NetworkErrorFunction handler, Qt::ConnectionType connection)
{
    return onNetworkErrors(this, std::move(handler), connection);
}
RestReply *RestReply::onNetworkErrors(const QObject *scope, NetworkErrorFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::networkError, scope, [xHandler = std::move(handler)](QNetworkReply::NetworkError error){
        xHandler(error);
    }, connection);

    return this;
}

RestReply *RestReply::onSslErrors(SslErrorFunction handler, Qt::ConnectionType connection)
{
    return onSslErrors(this, std::move(handler), connection);
}
RestReply *RestReply::onSslErrors(const QObject *scope, SslErrorFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::sslErrors, scope, [xHandler = std::move(handler)](const QList<QSslError> &errors, bool ignoreErrors){
        xHandler(errors, ignoreErrors);
    }, connection);

    return this;
}

RestReply *RestReply::onDownloadProgress(ProgressFunction handler, Qt::ConnectionType connection)
{
    return onDownloadProgress(this, std::move(handler), connection);
}
RestReply *RestReply::onDownloadProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::downloadProgress, scope, [xHandler = std::move(handler)](qint64 bytesReceived, qint64 bytesTotal){
        xHandler(bytesReceived, bytesTotal);
    }, connection);

    return this;
}

RestReply *RestReply::onUploadProgress(ProgressFunction handler, Qt::ConnectionType connection)
{
    return onUploadProgress(this, std::move(handler), connection);
}
RestReply *RestReply::onUploadProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::uploadProgress, scope, [xHandler = std::move(handler)](qint64 bytesSent, qint64 bytesTotal){
        xHandler(bytesSent, bytesTotal);
    }, connection);

    return this;
}

RestReply *RestReply::onParsingProgress(ProgressFunction handler, Qt::ConnectionType connection)
{
    return onParsingProgress(this, std::move(handler), connection);
}
RestReply *RestReply::onParsingProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection)
{
    if(!handler)
        return this;

    connect(this, &RestReply::parsingProgress, scope, [xHandler = std::move(handler)](qint64 bytesSent, qint64 bytesTotal){
        xHandler(bytesSent, bytesTotal);
    }, connection);

    return this;
}

void RestReply::abort()
{
    if (m_networkReply && m_networkReply->isRunning()) {
        m_networkReply->abort();
    }
}

int RestReply::waitForFinished(int timeout, QEventLoop::ProcessEventsFlags flags)
{
    if (!m_networkReply)
        return -1;

    QEventLoop loop;
    connect(this, &RestReply::finished, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    return loop.exec(flags);
}

void RestReply::connectReply()
{
    m_elapsed.start();

    m_networkReply->setParent(this);

    // forward some signals
    connect(m_networkReply, &QNetworkReply::errorOccurred, this, &RestReply::networkError);
#if QT_CONFIG(ssl)
    connect(m_networkReply, &QNetworkReply::sslErrors, this, &RestReply::onHandleSslErrors);
#endif
    connect(m_networkReply, &QNetworkReply::downloadProgress, this, &RestReply::downloadProgress);
    connect(m_networkReply, &QNetworkReply::uploadProgress, this, &RestReply::uploadProgress);

    connect(m_networkReply, &QNetworkReply::readyRead, this, &RestReply::onReplyReadyRead);
    connect(m_networkReply, &QNetworkReply::finished, this, &RestReply::onReplyFinished);
}

QByteArray RestReply::parseContentType(QByteArray contentType, RestParserError* restParseErrorPtr)
{
    RestParserError restParseError;

    // verify content type
    if (const QByteArrayList cList = contentType.split(';'); cList.size() > 1)
    {
        contentType = cList.first().trimmed();
        for (auto i = 1; i < cList.size(); ++i)
        {
            const QByteArrayList args = cList[i].trimmed().split('=');
            if (args.size() == 2 && args[0] == "charset")
            {
                if (args[1].toLower() != "utf-8")
                {
                    restParseError.code = -1;
                    restParseError.error = QString("Unsupported charset: %1").arg(QString::fromUtf8(args[1]));
                    break;
                }
            }
            else
            {
                RESTLOG_WARNING()<<"Unknown content type directive:"<<args[0];
            }
        }
    }

    if(restParseErrorPtr)
        *restParseErrorPtr = restParseError;

    return contentType;
}

QVariant RestReply::parseReply(const QVariantMap& reply, RestParserError* restParseErrorPtr)
{
    const QByteArray content = reply.value("content").toByteArray();
    const QByteArray contentType = reply.value("contentType").toByteArray();
    const long long contentLength = reply.value("contentLength").toLongLong();
    const int status = reply.value("status").toInt();
    const bool allowEmpty = reply.value("allowEmpty").toBool();

    return RestReply::parseReply(content, contentType, contentLength, status, allowEmpty, restParseErrorPtr);
}

QVariant RestReply::parseReply(const QByteArray& readData, QByteArray contentType, long long contentLength, int status, bool allowEmpty, RestParserError* restParseErrorPtr)
{
    RestParserError restParseError;

    contentType = parseContentType(contentType, &restParseError);

    QVariant data;
    if (restParseError.code!=0)
    {
        // means content type is invalid -> do nothing, but is here to skip the rest
    }
    else if (status<=0)
    {
        // means no answer
    }
    else if (contentLength==0 && (status == 204 || status >= 300 || allowEmpty)) // 204 = NO_CONTENT
    {
        // ok, nothing to do, but is here to skip the rest
    }
    else if (contentType == RestHelper::ContentTypeEmpty)
    {
        // ok, nothing to do, but is here to skip the rest
    }
    else if (contentType == RestHelper::ContentTypeCbor)
    {
        QCborParserError parseError;
        data = QUtils::Cbor::cborToVariant(readData, &parseError);

        if(parseError.error!=QCborError::NoError)
        {
            restParseError.code = parseError.error.c;
            restParseError.error = QString("Error loading cbor: %1").arg(parseError.errorString());
        }
    }
    else if (contentType == RestHelper::ContentTypeJson || contentType == RestHelper::ContentTypeDefault)
    {
        QJsonParseError parseError;
        data = QUtils::Json::jsonToVariant(readData, &parseError);

        if (parseError.error!=QJsonParseError::NoError)
        {
            restParseError.code = parseError.error;
            restParseError.error = QString("Error loading json: %1").arg(parseError.errorString());
        }
    }
    else if (contentType == RestHelper::ContentTypeHtml)
    {
        const QString text = QString::fromUtf8(readData);
        data = text;
    }
    else if (contentType == RestHelper::ContentTypeText)
    {
        const QString text = QString::fromUtf8(readData);
        data = text;
    }
    else
    {
        restParseError.code = -1;
        restParseError.error = QString("Unsupported content type: %1").arg(QString::fromUtf8(contentType));

        RESTLOG_WARNING()<<readData;
    }

    if(restParseErrorPtr)
        *restParseErrorPtr = restParseError;

    return data;
}

void RestReply::onReplyReadyRead()
{
    if (!m_networkReply)
        return;
}

void RestReply::onReplyFinished()
{
    if (!m_networkReply)
        return;

    const int status = m_networkReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray contentType = m_networkReply->header(QNetworkRequest::ContentTypeHeader).toByteArray().trimmed();
    const long long contentLength = m_networkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    const QByteArray readData = m_networkReply->isOpen() ? m_networkReply->readAll() : QByteArray();

    RESTLOG_DEBUG()<<"Received reply with status"<<status<<"and content of type"<<contentType<<"with a size of"<<bytes(contentLength)<<"after"<<m_elapsed.nsecsElapsed()/1000000.0<<"ms";
    m_elapsed.restart();

    if(!m_autoParse)
    {
        QVariantMap reply;
        reply.insert("content", readData);
        reply.insert("contentType", contentType);
        reply.insert("contentLength", contentLength);
        reply.insert("status", status);
        reply.insert("allowEmpty", m_allowEmpty);
        onParseFinished(reply, status, contentLength, RestParserError());
        return;
    }

    emit this->parsingProgress(0, contentLength);

#ifdef QT_CONCURRENT_LIB

    RestParserError* restParseError = new RestParserError;
    auto future = QtConcurrent::run([](const QByteArray& readData, const QByteArray& contentType, int contentLength, int status, bool allowEmpty, RestParserError* restParseError) {
        return RestReply::parseReply(readData, contentType, contentLength, status, allowEmpty, restParseError);
    }, readData, contentType, contentLength, status, m_allowEmpty, restParseError);

    future.then(this, [this, restParseError, status, contentLength](const QVariant& data) {
        onParseFinished(data, status, contentLength, *restParseError);
        delete restParseError;
    });

#else

    RestParserError restParseError;
    const QVariant data = RestReply::parseReply(readData, contentType, contentLength, status, m_allowEmpty, &restParseError);
    onParseFinished(data, status, contentLength, restParseError);

#endif
}

void RestReply::onParseFinished(const QVariant& data, int status, long long contentLength, RestParserError restParseError)
{
    RESTLOG_DEBUG()<<"Parsing of"<<bytes(contentLength)<<"took"<<m_elapsed.nsecsElapsed()/1000000.0<<"ms";
    m_elapsed.restart();

    emit this->parsingProgress(contentLength, contentLength);

    setContentLength(contentLength);
    setStatus(status);

    // check "http errors", because they can have data, but only if json is valid
    if (restParseError.code==0 && status>=300 && data.isValid()) // first: status code error + valid data
    {
        setErrorString("Request failure");
        setError(status);
        setErrorType(RestReplyErrorTypes::Failure);
        emit this->failed(status, data);
    }
    else if (m_networkReply->error() != QNetworkReply::NoError)  // next: check normal network errors
    {
        if(m_networkReply->error()==QNetworkReply::OperationCanceledError)
        {
            resetErrorString();
            resetError();
            resetErrorType();
            emit this->canceled();
        }
        else
        {
            setErrorString(m_networkReply->errorString());
            setError(m_networkReply->error());
            setErrorType(RestReplyErrorTypes::Network);
            emit this->error(m_networkReply->errorString(), m_networkReply->error(), RestReplyErrorTypes::Network, data);
        }
    }
    else if(restParseError.code!=0) // next: parsing errors
    {
        setErrorString(restParseError.error);
        setError(restParseError.code);
        setErrorType(RestReplyErrorTypes::Parser);
        emit this->error(restParseError.error, restParseError.code, RestReplyErrorTypes::Parser, data);
    }
    else if (status >= 300 && !data.isValid()) // only pass as failed without data if any other error does not match
    {
        setErrorString("Unknown error");
        setError(status);
        setErrorType(RestReplyErrorTypes::Unknown);
        emit this->failed(status, data);
    }
    else // no errors, succeeded!
    {
        resetErrorString();
        resetError();
        resetErrorType();
        emit this->succeeded(status, data);
    }

    emit this->finished(status);

    RESTLOG_DEBUG()<<"Emit this->finished took"<<m_elapsed.nsecsElapsed()/1000000.0<<"ms";
    m_elapsed.restart();
}

void RestReply::onHandleSslErrors(const QList<QSslError> &errors)
{
#if QT_CONFIG(ssl)
    // for(const QSslError& error: errors)
    // {
    //     RESTLOG_TRACE()<<error;
    //     RESTLOG_TRACE()<<error.errorString();
    //     RESTLOG_TRACE()<<error.certificate();
    //     RESTLOG_TRACE()<<int(error.error());
    // }

    bool ignore = true;
    emit this->sslErrors(errors, ignore);
    if (ignore)
        m_networkReply->ignoreSslErrors(errors);
#endif
}
