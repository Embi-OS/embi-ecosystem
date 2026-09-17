#include "restsocket.h"
#include "rest_log.h"

#include "restrequestbuilder.h"
#include "restclient.h"

#include <QWebSocketHandshakeOptions>
#include <QRegularExpression>

namespace {

bool isHandshakeAuthenticationError(const QString& errorString)
{
    // QWebSocket has no public HTTP response status. Match only explicit Qt
    // handshake diagnostics, using Qt's translation context as well.
    if(errorString == QString("QWebSocket::processHandshake: Host requires authentication")
        || errorString == QString("QWebSocketPrivate::processHandshake: Unsupported WWW-Authenticate challenge encountered.")
        || errorString == QString("QWebSocketPrivate::processHandshake: Unsupported WWW-Authenticate challenges encountered.")) {
        return true;
    }

    const QString statusPattern = QRegularExpression::escape(QString("QWebSocketPrivate::processHandshake: Unhandled http status code: %1 (%2)."))
                                      .replace(QRegularExpression::escape(QStringLiteral("%1")), QStringLiteral("(?:401|403)"))
                                      .replace(QRegularExpression::escape(QStringLiteral("%2")), QStringLiteral(".*"));
    return QRegularExpression(QRegularExpression::anchoredPattern(statusPattern)).match(errorString).hasMatch();
}

}

Q_GLOBAL_STATIC_WITH_ARGS(bool, g_restSocketGloballyEnabled, (true))
void RestSocket::setGloballyEnabled(bool globallyEnabled)
{
    *g_restSocketGloballyEnabled = globallyEnabled;
    if(globallyEnabled) {
        RESTLOG_INFO()<<"RestSocket has been globally enabled";
    }
    else {
        RESTLOG_INFO()<<"RestSocket has been globally disabled";
    }
}
bool RestSocket::globallyEnabled()
{
    return *g_restSocketGloballyEnabled;
}

RestSocket::RestSocket(QObject *parent):
    RestSocket("", parent)
{

}

RestSocket::RestSocket(const QString& path, QObject *parent):
    QObject(parent),
    m_path(path),
    m_socket(new QWebSocket(QString(), QWebSocketProtocol::VersionLatest, this)),
    m_openWatchdog(new QTimer(this)),
    m_reconnectTimer(new QTimer(this)),
    m_dataMode(RestDataModes::Json)
{
    m_openWatchdog->setSingleShot(true);
    connect(m_openWatchdog, &QTimer::timeout, this, &RestSocket::handleOpenTimeout);

    m_reconnectTimer->setSingleShot(true);
    connect(m_reconnectTimer, &QTimer::timeout, this, &RestSocket::handleReconnectTimeout);

    connect(m_socket, &QWebSocket::textMessageReceived, this, &RestSocket::handleTextMessageReceived);
    connect(m_socket, &QWebSocket::binaryMessageReceived, this, &RestSocket::handleBinaryMessageReceived);
    connect(m_socket, &QWebSocket::errorOccurred, this, &RestSocket::handleError);
#if QT_CONFIG(ssl)
    connect(m_socket, &QWebSocket::sslErrors, this, &RestSocket::handleSslErrors);
#endif
    connect(m_socket, &QWebSocket::stateChanged, this, &RestSocket::handleStateChanged);
    connect(m_socket, &QWebSocket::pong, this, &RestSocket::pong);
    connect(m_socket, &QWebSocket::connected, this, &RestSocket::handleConnected);
    connect(m_socket, &QWebSocket::disconnected, this, &RestSocket::handleDisconnected);

    connect(this, &RestSocket::connectionChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::enabledChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::pathChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::methodChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::parametersChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::bindWhenChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::requestedProtocolsChanged, this, &RestSocket::queueBind);
    connect(this, &RestSocket::reconnectChanged, this, [this](bool reconnect) {
        if(!reconnect)
            cancelReconnect();
    });
    connect(this, &RestSocket::reconnectDelayMsChanged, this, [this]() {
        resetReconnectDelay();
    });
    connect(this, &RestSocket::reconnectMaxDelayMsChanged, this, [this]() {
        resetReconnectDelay();
    });
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

bool RestSocket::shouldBeOpen() const
{
    return RestSocket::globallyEnabled() && m_enabled && m_bindWhen;
}

void RestSocket::resetReconnectDelay()
{
    m_currentReconnectDelayMs = qMax(0, m_reconnectDelayMs);
}

void RestSocket::cancelReconnect()
{
    if(m_reconnectTimer->isActive()) {
        RESTLOG_DEBUG()<<"REST socket pending reconnect cancelled"<<this<<"remainingMs"<<m_reconnectTimer->remainingTime();
    }
    m_reconnectReason.clear();
    m_reconnectTimer->stop();
}

bool RestSocket::isIntentionalClose() const
{
    return m_manualClose || m_rebindAfterClose || !shouldBeOpen();
}

void RestSocket::scheduleReconnect(const QString& reason, bool resetDelay)
{
    if(!m_reconnect || m_authenticationRejected || !shouldBeOpen() || m_manualClose) {
        RESTLOG_DEBUG()<<"REST socket reconnect skipped"<<this<<"reconnect"<<m_reconnect
                       <<"authenticationRejected"<<m_authenticationRejected<<"shouldBeOpen"<<shouldBeOpen()<<"manualClose"<<m_manualClose;
        return;
    }

    if(resetDelay || m_currentReconnectDelayMs <= 0)
        resetReconnectDelay();

    const int maxDelayMs = (m_reconnectMaxDelayMs > 0) ? qMax(m_reconnectDelayMs, m_reconnectMaxDelayMs) : m_currentReconnectDelayMs;
    const int delayMs = qBound(0, m_currentReconnectDelayMs, maxDelayMs);
    m_reconnectReason = reason;
    m_reconnectNeeded = true;

    if(m_reconnectTimer->isActive()) {
        RESTLOG_TRACE()<<"REST socket reconnect already scheduled"<<this<<"remainingMs"<<m_reconnectTimer->remainingTime();
        return;
    }

    RESTLOG_DEBUG()<<"REST socket reconnect scheduled"<<m_url<<"delayMs"<<delayMs<<"maxDelayMs"<<maxDelayMs<<reason;
    m_reconnectTimer->start(delayMs);
    emit this->reconnectScheduled(delayMs, reason);

    if(delayMs > 0 && m_reconnectMaxDelayMs > 0) {
        const qint64 nextDelay = static_cast<qint64>(delayMs) * 2;
        m_currentReconnectDelayMs = int(qMin<qint64>(maxDelayMs, qMax<qint64>(m_reconnectDelayMs, nextDelay)));
    }
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

    m_openWatchdog->stop();
    m_lastSocketError = error;

    const QString errorString = m_socket->errorString();
    if(!m_connected && error == QAbstractSocket::ConnectionRefusedError
        && isHandshakeAuthenticationError(errorString)) {
        // Do not retry rejected credentials. Recreate the socket to start a
        // new authentication session.
        setAuthenticationRejected(true);
        m_rebindAfterClose = false;
        cancelReconnect();
    }

    if(error == QAbstractSocket::RemoteHostClosedError) {
        RESTLOG_DEBUG()<<"REST socket remote host closed"<<m_url;
        return;
    }

    setPhase(RestSocketPhases::Faulted);
    setError(QString("%1\n%2 (%3)").arg(m_url, errorString).arg(error));
    setStatus(RestSocketStates::Error);

    emit this->error();

    switch (error) {
    case QAbstractSocket::DatagramTooLargeError:
        break;
    default:
        if (m_socket->state() == QAbstractSocket::UnconnectedState)
            scheduleReconnect(m_error);
        else if(m_socket->state() == QAbstractSocket::ConnectingState) {
            // Some handshake errors leave QWebSocket in ConnectingState.
            // Let Qt finish its error handler before aborting that attempt.
            QMetaObject::invokeMethod(this, [this]() {
                if(m_status == RestSocketStates::Error && m_socket->state() == QAbstractSocket::ConnectingState)
                    m_socket->abort();
            }, Qt::QueuedConnection);
        }
        break;
    }
}

void RestSocket::handleSslErrors(const QList<QSslError> &errors)
{
#if QT_CONFIG(ssl)
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
        if(m_status != RestSocketStates::Error)
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

void RestSocket::handleConnected()
{
    if(m_reconnectNeeded) {
        RESTLOG_DEBUG()<<"REST socket reconnected"<<m_url;
    }
    else {
        RESTLOG_DEBUG()<<"REST socket connected"<<m_url;
    }

    m_manualClose = false;
    m_rebindAfterClose = false;
    m_openWatchdog->stop();
    cancelReconnect();
    resetReconnectDelay();
    m_lastSocketError = QAbstractSocket::UnknownSocketError;
    setPhase(RestSocketPhases::Connected);

    if(!m_error.isEmpty())
        resetError();

    setConnected(true);
    emit this->connected();

    if(m_reconnectNeeded)
        emit this->reconnected();
    m_reconnectNeeded = false;
}

void RestSocket::handleDisconnected()
{
    const QWebSocketProtocol::CloseCode closeCode = m_socket->closeCode();
    const QString closeReason = m_socket->closeReason();
    const bool intentionalClose = isIntentionalClose();
    const bool cleanCloseCode = closeCode == QWebSocketProtocol::CloseCodeNormal
                                || closeCode == QWebSocketProtocol::CloseCodeGoingAway;
    const bool cleanClose = m_connected && cleanCloseCode
                            && (m_lastSocketError == QAbstractSocket::UnknownSocketError
                                || m_lastSocketError == QAbstractSocket::RemoteHostClosedError)
                            && m_error.isEmpty();
    const bool shouldReconnect = !intentionalClose && m_reconnect && !m_authenticationRejected
                                 && (!cleanClose || m_reconnectOnCleanClose);
    const bool transientDisconnect = cleanClose && shouldReconnect;
    RESTLOG_DEBUG()<<"REST socket close policy"<<this<<"closeCode"<<int(closeCode)
                   <<"intentional"<<intentionalClose<<"clean"<<cleanClose<<"reconnect"<<shouldReconnect;
    const QString reason = m_error.isEmpty()
                               ? (cleanClose
                                      ? (!closeReason.isEmpty()
                                             ? QString("%1\n%2 (%3)").arg(m_url, closeReason).arg(int(closeCode))
                                             : QString("%1\nSocket closed (%2)").arg(m_url).arg(int(closeCode)))
                                      : QString("Socket disconnected"))
                               : m_error;

    if(!intentionalClose) {
        if(transientDisconnect) {
            RESTLOG_DEBUG()<<"REST socket closed cleanly, reconnecting"<<m_url<<closeCode<<closeReason;
            setPhase(RestSocketPhases::Reconnecting);
        }
        else {
            RESTLOG_DEBUG()<<"REST socket disconnected"<<m_url<<reason;
            setPhase(RestSocketPhases::Faulted);
        }
    }
    else {
        setPhase(RestSocketPhases::Inactive);
    }

    m_openWatchdog->stop();
    setConnected(false);
    emit this->disconnected();

    if(m_rebindAfterClose && !m_authenticationRejected) {
        RESTLOG_DEBUG()<<"REST socket reopening after requested close"<<this;
        m_rebindAfterClose = false;
        m_manualClose = false;
        m_lastSocketError = QAbstractSocket::UnknownSocketError;
        setPhase(RestSocketPhases::Connecting);
        QMetaObject::invokeMethod(this, &RestSocket::open, Qt::QueuedConnection);
        return;
    }

    m_manualClose = false;
    m_lastSocketError = QAbstractSocket::UnknownSocketError;

    if(!intentionalClose && !transientDisconnect && m_error.isEmpty()) {
        setError(reason);
        setStatus(RestSocketStates::Error);
        emit this->error();
    }

    if(shouldReconnect)
        scheduleReconnect(reason);
    else
        cancelReconnect();
}

void RestSocket::handleOpenTimeout()
{
    if(m_socket->state() != QAbstractSocket::ConnectingState)
        return;

    const QString reason = QString("Socket connection timeout after %1 ms").arg(m_openTimeoutMs);
    RESTLOG_WARNING()<<"REST socket handshake timeout"<<this<<"timeoutMs"<<m_openTimeoutMs<<"reconnecting"<<m_reconnectNeeded;

    setPhase(RestSocketPhases::Faulted);
    setError(reason);
    setStatus(RestSocketStates::Error);
    emit this->error();

    // abort() terminates a stalled handshake immediately. handleDisconnected()
    // schedules the next attempt through the normal reconnect policy.
    m_socket->abort();
}

void RestSocket::handleReconnectTimeout()
{
    m_reconnectTimer->stop();

    if(!m_reconnect || m_authenticationRejected || !shouldBeOpen() || m_manualClose) {
        RESTLOG_DEBUG()<<"REST socket scheduled reconnect abandoned"<<this<<"reconnect"<<m_reconnect
                       <<"authenticationRejected"<<m_authenticationRejected<<"shouldBeOpen"<<shouldBeOpen()<<"manualClose"<<m_manualClose;
        setPhase(RestSocketPhases::Inactive);
        return;
    }

    RESTLOG_DEBUG()<<"REST socket reconnect"<<m_url<<m_reconnectReason;
    open();
}

void RestSocket::queueBind()
{
    if(!m_completed)
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

    if(shouldBeOpen())
        open();
    else
        close();
}

bool RestSocket::waitForBind(int timeout)
{
    if(!RestSocket::globallyEnabled())
        return true;

    if(m_status==RestSocketStates::Open)
        return true;

    if(!shouldBeOpen() || m_authenticationRejected
        || m_status==RestSocketStates::Error || m_status==RestSocketStates::Closed)
        return false;

    QEventLoop loop;
    connect(this, &RestSocket::connected, &loop, &QEventLoop::quit, Qt::QueuedConnection);
    connect(this, &RestSocket::error, &loop, &QEventLoop::quit, Qt::QueuedConnection);
    connect(this, &RestSocket::disconnected, &loop, &QEventLoop::quit, Qt::QueuedConnection);
    if(timeout>=0)
        QTimer::singleShot(timeout, &loop, &QEventLoop::quit);
    loop.exec(QEventLoop::AllEvents);

    return m_status==RestSocketStates::Open;
}

void RestSocket::unbind()
{
    close();
}

void RestSocket::reconnectNow(const QString& reason)
{
    RESTLOG_DEBUG()<<"REST socket immediate reconnect requested"<<this<<"state"<<m_socket->state();
    m_openWatchdog->stop();
    cancelReconnect();

    if(!shouldBeOpen() || m_authenticationRejected)
        return;

    if(!reason.isEmpty())
        setError(reason);

    if (m_socket->state() == QAbstractSocket::UnconnectedState) {
        resetReconnectDelay();
        open();
        return;
    }

    m_manualClose = false;
    m_rebindAfterClose = true;
    m_socket->close(QWebSocketProtocol::CloseCodeGoingAway, reason.isEmpty() ? QString("Socket reconnect requested") : reason.left(123));
}

qint64 RestSocket::sendTextMessage(const QString &message)
{
    if(!RestSocket::globallyEnabled())
        return 0;

    if (m_status != RestSocketStates::Open) {
        setError("Messages can only be sent when the socket is open");
        setStatus(RestSocketStates::Error);
        return 0;
    }

    RESTLOG_DEBUG()<<message;

    return m_socket->sendTextMessage(message);
}

qint64 RestSocket::sendBinaryMessage(const QByteArray &message)
{
    if(!RestSocket::globallyEnabled())
        return 0;

    if (m_status != RestSocketStates::Open) {
        setError("Messages can only be sent when the socket is open");
        setStatus(RestSocketStates::Error);
        return 0;
    }

    RESTLOG_DEBUG()<<message;

    return m_socket->sendBinaryMessage(message);
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
    if(!RestSocket::globallyEnabled())
        return;

    if (m_status != RestSocketStates::Open) {
        setError("Ping can only be sent when the socket is open");
        setStatus(RestSocketStates::Error);
        return;
    }

    m_socket->ping(payload);
}

QVariantMap RestSocket::bindParameters() const
{
    return m_parameters;
}

void RestSocket::open()
{
    cancelReconnect();

    if(!shouldBeOpen() || m_authenticationRejected)
        return;
    m_lastSocketError = QAbstractSocket::UnknownSocketError;

    if(m_phase != RestSocketPhases::Reconnecting && m_phase != RestSocketPhases::Faulted)
        setPhase(RestSocketPhases::Connecting);

    RestRequestBuilder builder = RestHelper::apiClient(m_connection)->builder();

    m_dataMode = builder.dataMode();

    builder.addPath(m_path);
    builder.addPath(m_method);
    builder.addParameters(bindParameters());

    QNetworkRequest request = builder.buildRequest();
    QUrl url = request.url();

    if(url.scheme()=="http")
        url.setScheme("ws");
    else if(url.scheme()=="https")
        url.setScheme("wss");
    request.setUrl(url);

    const QUrl currentTargetUrl = m_targetUrl;
    setUrl(url.toString());
    m_targetUrl = url;

    if(!url.isValid() || url.isEmpty()) {
        setPhase(RestSocketPhases::Faulted);
        setError(QString("Invalid socket url: %1").arg(url.toString()));
        setStatus(RestSocketStates::Error);
        emit this->error();
        return;
    }

    const QAbstractSocket::SocketState socketState = m_socket->state();
    const bool sameUrl = (currentTargetUrl == url);

    if(socketState == QAbstractSocket::ConnectedState && sameUrl)
        return;

    if(socketState == QAbstractSocket::ConnectingState && sameUrl)
        return;

    if(socketState != QAbstractSocket::UnconnectedState) {
        m_manualClose = false;
        m_rebindAfterClose = true;
        m_socket->close(QWebSocketProtocol::CloseCodeGoingAway, QString("Socket rebind"));
        return;
    }

    QWebSocketHandshakeOptions options;
    options.setSubprotocols(m_requestedProtocols);

    setConnected(false);
    setNegotiatedProtocol({});
    m_manualClose = false;
    m_rebindAfterClose = false;

    resetError();
    setPhase(RestSocketPhases::Connecting);

    if(m_openTimeoutMs > 0)
        m_openWatchdog->start(m_openTimeoutMs);
    else
        m_openWatchdog->stop();

    // qNotice().noquote()<<RestHelper::dumpRequest(request);

    m_socket->open(request, options);
}

void RestSocket::close()
{
    cancelReconnect();
    m_openWatchdog->stop();
    m_rebindAfterClose = false;
    m_manualClose = true;
    m_lastSocketError = QAbstractSocket::UnknownSocketError;
    setPhase(RestSocketPhases::Inactive);

    if(m_status==RestSocketStates::Closed && m_socket->state()==QAbstractSocket::UnconnectedState)
        return;

    m_socket->close();
}
