#include "restsocketwatcher.h"
#include "rest_log.h"

RestSocketWatcher::RestSocketWatcher(RestSocket *parent):
    QObject(parent),
    m_socket(parent),
    m_pingCaller(new QTimer(this)),
    m_pingWatchdog(new QTimer(this))
{
    m_pingCaller->setSingleShot(true);
    connect(m_pingCaller, &QTimer::timeout, this, &RestSocketWatcher::sendPing);

    m_pingWatchdog->setSingleShot(true);
    connect(m_pingWatchdog, &QTimer::timeout, this, &RestSocketWatcher::onPingTimeout);

    applyTimersConfiguration();

    connect(this, &RestSocketWatcher::enabledChanged, this, [this]() {
        if(!m_enabled) {
            resetPingState();
            return;
        }

        applyTimersConfiguration();
        if(m_socket->getStatus() == RestSocketStates::Open)
            queueSendPing();
    });
    connect(this, &RestSocketWatcher::pingIntervalMsChanged, this, &RestSocketWatcher::applyTimersConfiguration);
    connect(this, &RestSocketWatcher::pongTimeoutMsChanged, this, &RestSocketWatcher::applyTimersConfiguration);

    connect(m_socket, &RestSocket::pong, this, &RestSocketWatcher::onPongReceived);
    connect(m_socket, &RestSocket::connected, this, &RestSocketWatcher::onSocketConnected);
    connect(m_socket, &RestSocket::disconnected, this, &RestSocketWatcher::onSocketDisconnected);
    connect(m_socket, &RestSocket::statusChanged, this, &RestSocketWatcher::onSocketStatusChanged);
    connect(m_socket, &RestSocket::phaseChanged, this, &RestSocketWatcher::handleSocketDisconnected, Qt::QueuedConnection);
    connect(m_socket, &RestSocket::reconnectChanged, this, &RestSocketWatcher::handleSocketDisconnected, Qt::QueuedConnection);
    connect(m_socket, &RestSocket::error, this, [this]() {
        setDisconnected(m_socket->getError());
    });

    if(m_socket->getStatus()==RestSocketStates::Open)
        queueSendPing();
}

void RestSocketWatcher::resetPingState()
{
    m_pingCaller->stop();
    m_pingWatchdog->stop();
    m_lastPingPayload.clear();
    m_pingTimeoutCount = 0;
}

void RestSocketWatcher::applyTimersConfiguration()
{
    m_pingCaller->setInterval(qMax(1000, m_pingIntervalMs));
    m_pingWatchdog->setInterval(qMax(1000, m_pongTimeoutMs));
}

void RestSocketWatcher::queueSendPing()
{
    if(!m_enabled)
        return;

    m_pingCaller->stop();
    m_pingCaller->start();
}

void RestSocketWatcher::sendPing()
{
    m_pingCaller->stop();

    if(!m_enabled)
        return;

    if(m_socket->getStatus()!=RestSocketStates::Open)
        return;

    m_lastPingPayload = QUuid::createUuid().toByteArray(QUuid::WithoutBraces);
    m_socket->ping(m_lastPingPayload);
    m_pingWatchdog->start();
}

void RestSocketWatcher::setDisconnected(const QString& reason)
{
    resetPingState();

    if(setConnected(false)) {
        RESTLOG_DEBUG()<<"REST socket watcher connection lost"<<m_socket<<"phase"<<m_socket->getPhase();
        emit this->connectionLost(reason);
    }
}

void RestSocketWatcher::onSocketConnected()
{
    RESTLOG_DEBUG()<<"REST Socket connected"<<m_socket->getUrl();
    m_pingTimeoutCount = 0;
    setConnected(true);
    queueSendPing();
}

void RestSocketWatcher::onSocketDisconnected()
{
    resetPingState();
    QMetaObject::invokeMethod(this, &RestSocketWatcher::handleSocketDisconnected, Qt::QueuedConnection);
}

void RestSocketWatcher::onSocketStatusChanged(RestSocketStates::Enum status)
{
    if(status == RestSocketStates::Connecting || status == RestSocketStates::Closing)
        resetPingState();
}

void RestSocketWatcher::handleSocketDisconnected()
{
    if(m_socket->getStatus() == RestSocketStates::Open)
        return;

    const RestSocketPhases::Enum phase = m_socket->getPhase();
    const bool connectionPending = phase == RestSocketPhases::Connecting
                                   || (phase == RestSocketPhases::Reconnecting && m_socket->getReconnect());
    // A clean server rotation keeps the logical connection alive while waiting
    // for the retry and its handshake. Errors and cancellation end that grace.
    if(connectionPending && m_socket->getError().isEmpty()
        && RestSocket::globallyEnabled() && m_socket->getEnabled() && m_socket->getBindWhen()) {
        RESTLOG_TRACE()<<"REST socket watcher awaiting reconnect"<<m_socket<<"phase"<<phase<<"connected"<<m_connected;
        return;
    }

    const QString reason = m_socket->getError().isEmpty() ? QString("Socket disconnected") : m_socket->getError();
    setDisconnected(reason);
}

void RestSocketWatcher::onPongReceived(quint64 elapsedTime, const QByteArray &payload)
{
    if(payload != m_lastPingPayload)
        return;

    if(m_pingTimeoutCount>0) {
        RESTLOG_WARNING()<<"Socket pong received after"<<(m_pingTimeoutCount+1)<<"retry";
    }
    m_pingTimeoutCount = 0;
    m_pingWatchdog->stop();
    queueSendPing();

    setConnected(true);
    emit this->pongReceived(elapsedTime);
}

void RestSocketWatcher::onPingTimeout()
{
    m_pingTimeoutCount++;
    if(m_pingTimeoutCount<qMax(1, m_maxPingTimeoutCount))
    {
        RESTLOG_WARNING()<<"Socket ping timeout"<<m_pingTimeoutCount;
        m_pingWatchdog->stop();
        sendPing();
        return;
    }

    const QString reason = QString("Socket ping timeout");
    RESTLOG_CRITICAL()<<"Socket ping error after"<<m_pingTimeoutCount<<"retries";
    setDisconnected(reason);

    if(m_reconnectOnPingTimeout) {
        RESTLOG_DEBUG()<<"REST socket watcher requesting reconnect after ping failure"<<m_socket;
        m_socket->reconnectNow(reason);
    }
    else {
        RESTLOG_DEBUG()<<"REST socket watcher continuing pings without reconnect"<<m_socket;
        queueSendPing();
    }
}
