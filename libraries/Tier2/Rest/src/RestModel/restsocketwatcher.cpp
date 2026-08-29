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

    if(setConnected(false))
        emit this->connectionLost(reason);
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

    if(m_socket->getPhase() == RestSocketPhases::Connecting || m_socket->getPhase() == RestSocketPhases::Reconnecting) {
        RESTLOG_DEBUG()<<"Socket reconnect in progress"<<m_socket->getUrl();
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

    if(m_reconnectOnPingTimeout)
        m_socket->reconnectNow(reason);
    else
        queueSendPing();
}
