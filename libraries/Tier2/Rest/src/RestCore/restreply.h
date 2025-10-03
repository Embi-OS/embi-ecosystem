#ifndef RESTREPLY_H
#define RESTREPLY_H

#include <QDefs>
#include "rest_helpertypes.h"

class RestReply : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_COMPOSITION_PROPERTY(QNetworkReply, networkReply, nullptr)

    Q_WRITABLE_VAR_PROPERTY(bool, autoDelete, AutoDelete, true)
    Q_WRITABLE_VAR_PROPERTY(bool, autoParse, AutoParse, true)
    Q_WRITABLE_VAR_PROPERTY(bool, allowEmpty, AllowEmpty, false)

    Q_READONLY_REF_PROPERTY(QString, networkRequest, NetworkRequest, "")

    Q_READONLY_VAR_PROPERTY(int, status, Status, 0)
    Q_READONLY_VAR_PROPERTY(long long, contentLength, ContentLength, 0)
    Q_READONLY_REF_PROPERTY(QString, errorString, ErrorString, {})
    Q_READONLY_VAR_PROPERTY(int, error, Error, 0)
    Q_READONLY_VAR_PROPERTY(RestReplyErrorTypes::Enum, errorType, ErrorType, RestReplyErrorTypes::NoError)

public:
    typedef std::function<void(int)> FinishedFunction;
    typedef std::function<void(int, const QVariant&)> CompletedFunction;
    typedef std::function<void()> CanceledFunction;
    typedef std::function<void(const QString&, int, RestReplyErrorTypes::Enum, const QVariant&)> ErrorFunction;
    typedef std::function<void(QNetworkReply::NetworkError)> NetworkErrorFunction;
    typedef std::function<void(const QList<QSslError>&, bool)> SslErrorFunction;
    typedef std::function<void(qint64, qint64)> ProgressFunction;

    explicit RestReply(QNetworkReply *networkReply, QObject *parent=nullptr);
    ~RestReply() override;

    RestReply* onFinished(FinishedFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onFinished(const QObject *scope, FinishedFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onSucceeded(CompletedFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onSucceeded(const QObject *scope, CompletedFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onFailed(CompletedFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onFailed(const QObject *scope, CompletedFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onCanceled(CanceledFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onCanceled(const QObject *scope, CanceledFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onError(ErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onError(const QObject *scope, ErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onAllErrors(ErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onAllErrors(const QObject *scope, ErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onNetworkErrors(NetworkErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onNetworkErrors(const QObject *scope, NetworkErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onSslErrors(SslErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onSslErrors(const QObject *scope, SslErrorFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onDownloadProgress(ProgressFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onDownloadProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onUploadProgress(ProgressFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onUploadProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    RestReply* onParsingProgress(ProgressFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);
    RestReply* onParsingProgress(const QObject *scope, ProgressFunction handler, Qt::ConnectionType connection = REST_DEFAULT_CONNECTION_TYPE);

    static QByteArray parseContentType(QByteArray contentType, RestParserError* restParseErrorPtr=nullptr);
    static QVariant parseReply(const QByteArray& readData, QByteArray contentType, long long contentLength, int status, bool allowEmpty, RestParserError* restParseErrorPtr=nullptr);
    static QVariant parseReply(const QVariantMap& reply, RestParserError* restParseErrorPtr=nullptr);

public slots:
    void abort();
    int waitForFinished(int timeout=-1, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);

signals:
    void finished(int httpStatus);

    void succeeded(int httpStatus, const QVariant &reply);
    void failed(int httpStatus, const QVariant &reply);
    void canceled();

    void error(const QString &errorString, int error, RestReplyErrorTypes::Enum errorType, const QVariant &reply);

    void networkError(QNetworkReply::NetworkError error);
    void sslErrors(const QList<QSslError> &errors, bool ignoreErrors);

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void parsingProgress(qint64 bytesSent, qint64 bytesTotal);

private slots:
    void onReplyReadyRead();
    void onReplyFinished();
    void onParseFinished(const QVariant& data, int status, long long contentLength, RestParserError restParseError);
    void onHandleSslErrors(const QList<QSslError> &errors);

private:
    void connectReply();

    QElapsedTimer m_elapsed;
};

#endif // RESTREPLY_H
