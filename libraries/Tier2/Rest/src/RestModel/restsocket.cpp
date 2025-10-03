#include "restsocket.h"
#include "rest_log.h"

#include "restrequestbuilder.h"
#include "restclient.h"

#include <QWebSocketHandshakeOptions>

RestSocket::RestSocket(QObject *parent):
    RestSocket("", parent)
{

}

RestSocket::RestSocket(const QString& path, QObject *parent):
    QObject(parent),
    m_path(path),
    m_socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this)),
    m_dataMode(RestDataModes::Json)
{
    connect(m_socket, &QWebSocket::textMessageReceived, this, &RestSocket::handleTextMessageReceived);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &RestSocket::handleBinaryMessageReceived);
    connect(m_socket, &QWebSocket::errorOccurred, this, &RestSocket::handleError);
#if QT_CONFIG(ssl)
    connect(m_socket, &QWebSocket::sslErrors, this, &RestSocket::handleSslErrors);
#endif
    connect(m_socket, &QWebSocket::stateChanged, this, &RestSocket::handleStateChanged);
    connect(m_socket, &QWebSocket::pong, this, &RestSocket::pong);

    connect(m_socket, &QWebSocket::connected, this, &RestSocket::connected);
    connect(m_socket, &QWebSocket::disconnected, this, &RestSocket::disconnected);

    connect(this, &RestSocket::connected, this, [this](){ setConnected(true); });
    connect(this, &RestSocket::disconnected, this, [this](){ setConnected(false); });

    connect(this, &RestSocket::connectionChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::enabledChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::pathChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::methodChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::parametersChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::bindWhenChanged, this, &RestSocket::queueBind);
}

RestSocket::~RestSocket()
{

}

void RestSocket::classBegin()
{

}

void RestSocket::componentComplete()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    if(m_enabled && m_bindWhen)
        bind();
}

bool RestSocket::isCompleted() const
{
    return m_completed;
}

void RestSocket::handleTextMessageReceived(const QString& message)
{
    if(!m_enabled)
        return;

    RESTLOG_TRACE().noquote()<<message;

    emit this->textMessageReceived(message);
}

void RestSocket::handleBinaryMessageReceived(const QByteArray& message)
{
    if(!m_enabled)
        return;

    RESTLOG_TRACE().noquote()<<message;

    emit this->binaryMessageReceived(message);

    RestParserError restParseError;
    QVariant data;

    if (m_dataMode == RestDataModes::Json) {
        QJsonParseError parseError;
        data = QUtils::Json::jsonToVariant(message, &parseError);

        if (parseError.error!=QJsonParseError::NoError)
        {
            restParseError.code = parseError.error;
            restParseError.error = QString("Error loading json: %1").arg(parseError.errorString());
        }
    }
    else if (m_dataMode == RestDataModes::Cbor) {
        QCborParserError parseError;
        data = QUtils::Cbor::cborToVariant(message, &parseError);

        if(parseError.error!=QCborError::NoError)
        {
            restParseError.code = parseError.error.c;
            restParseError.error = QString("Error loading cbor: %1").arg(parseError.errorString());
        }
    }
    else {
        const QString text = QString::fromUtf8(message);
        data = text;
    }

    if(restParseError.code!=0)
    {
        setError(QString("parse error: %1 (%2)").arg(restParseError.error).arg(restParseError.code));
        setStatus(RestSocketStates::Error);
        emit this->error();
        return;
    }

    emit this->messageReceived(data);
}

void RestSocket::handleError(QAbstractSocket::SocketError error)
{
    RESTLOG_WARNING()<<"REST socket"<<m_url<<error;

    setError(QString("%1 (%2)").arg(m_socket->errorString()).arg(m_socket->error()));
    setStatus(RestSocketStates::Error);

    emit this->error();

    switch (error) {
    case QAbstractSocket::DatagramTooLargeError:
        break;
    default:
        if (m_reconnect && !m_bindQueued) {
            m_bindQueued = true;
            QTimer::singleShot(5000, this, &RestSocket::bind);
        }
        break;
    }
}

void RestSocket::handleSslErrors(const QList<QSslError> &errors)
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
        m_socket->ignoreSslErrors(errors);
#endif
}

void RestSocket::handleStateChanged(QAbstractSocket::SocketState state)
{
    RESTLOG_DEBUG()<<"REST socket"<<m_url<<state;

    switch (state) {
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::BoundState:
    case QAbstractSocket::HostLookupState:
        setStatus(RestSocketStates::Connecting);
        break;
    case QAbstractSocket::UnconnectedState:
        setStatus(RestSocketStates::Closed);
        break;
    case QAbstractSocket::ConnectedState:
        setStatus(RestSocketStates::Open);
        setNegotiatedProtocol(m_socket->subprotocol());
        break;
    case QAbstractSocket::ClosingState:
        setStatus(RestSocketStates::Closing);
        break;
    default:
        setStatus(RestSocketStates::Connecting);
        break;
    }
}

void RestSocket::queueBind()
{
    if(!m_completed || !m_bindWhen)
        return;

    if (m_delayed) {
        if (!m_bindQueued) {
            m_bindQueued = true;
            QMetaObject::invokeMethod(this, &RestSocket::bind, Qt::QueuedConnection);
        }
    } else {
        bind();
    }
}

void RestSocket::bind()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    m_bindQueued = false;

    if(m_enabled && m_bindWhen)
        open();
    else
        close();
}

bool RestSocket::waitForBind(int timeout)
{
#ifndef REST_NO_WEBSOCKET
    if(m_status==RestSocketStates::Open)
        return true;

    QEventLoop loop;
    connect(this, &RestSocket::connected, &loop, &QEventLoop::quit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::AllEvents);

    return m_status==RestSocketStates::Open;
#endif

    return true;
}

void RestSocket::unbind()
{
    close();
}

qint64 RestSocket::sendTextMessage(const QString &message)
{
#ifndef REST_NO_WEBSOCKET
    if (m_status != RestSocketStates::Open) {
        setError("Messages can only be sent when the socket is open");
        setStatus(RestSocketStates::Error);
        return 0;
    }

    RESTLOG_DEBUG()<<message;

    return m_socket->sendTextMessage(message);
#endif

    return 0;
}

qint64 RestSocket::sendBinaryMessage(const QByteArray &message)
{
#ifndef REST_NO_WEBSOCKET
    if (m_status != RestSocketStates::Open) {
        setError("Messages can only be sent when the socket is open");
        setStatus(RestSocketStates::Error);
        return 0;
    }

    RESTLOG_DEBUG()<<message;

    return m_socket->sendBinaryMessage(message);
#endif

    return 0;
}

qint64 RestSocket::sendMessage(const QVariant &message)
{
    if(!message.isValid())
        return -1;

    QByteArray data;

    if (m_dataMode == RestDataModes::Json) {
        data = QUtils::Json::variantToJson(message);
    }
    else if (m_dataMode == RestDataModes::Cbor) {
        data = QUtils::Cbor::variantToCbor(message, QCborValue::UseFloat16);
    }
    else {
        data = message.toString().toUtf8();
    }

    return sendBinaryMessage(data);
}

void RestSocket::ping(const QByteArray &payload)
{
#ifndef REST_NO_WEBSOCKET
    if (m_status != RestSocketStates::Open) {
        setError("Ping can only be sent when the socket is open");
        setStatus(RestSocketStates::Error);
        return;
    }

    m_socket->ping(payload);
#endif
}

void RestSocket::open()
{
#ifndef REST_NO_WEBSOCKET
    RestRequestBuilder builder = RestHelper::apiClient(m_connection)->builder();

    m_dataMode = builder.dataMode();

    builder.addPath(m_path);
    builder.addPath(m_method);
    builder.addParameters(m_parameters);

    QUrl url = builder.buildUrl();

    if(url.scheme()=="http")
        url.setScheme("ws");
    else if(url.scheme()=="https")
        url.setScheme("wss");

    setUrl(url.toString());

    if(!url.isValid())
        return;

    QNetworkRequest request(url);
    builder.prepareRequest(request);

    QWebSocketHandshakeOptions options;
    options.setSubprotocols(m_requestedProtocols);

    // qNotice()<<this<<"REST socket open"<<url.toString();
    m_socket->open(request, options);
#endif
}

void RestSocket::close()
{
    if(m_status==RestSocketStates::Closed)
        return;

    m_socket->close();
}

void RestSocket::closeGoingAway()
{
    if(m_status==RestSocketStates::Closed)
        return;

    m_socket->close(QWebSocketProtocol::CloseCodeGoingAway, tr("Connection closed"));
}
