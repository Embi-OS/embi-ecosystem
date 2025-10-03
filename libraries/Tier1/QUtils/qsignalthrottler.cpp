#include "qsignalthrottler.h"

QGenericSignalThrottler::QGenericSignalThrottler(Type type,
                                     EmissionPolicy emissionPolicy,
                                     QObject *parent)
    : QObject(parent)
    , m_type(type)
    , m_emissionPolicy(emissionPolicy)
    , m_timer(this)
    , m_hasPendingEmission(false)
{
    switch (m_emissionPolicy) {
    case EmissionPolicy::Leading:
        m_timer.setSingleShot(false);
        break;
    case EmissionPolicy::Trailing:
        m_timer.setSingleShot(true);
        break;
    }
    connect(&m_timer, &QTimer::timeout, this, &QGenericSignalThrottler::maybeEmitTriggered);
}

QGenericSignalThrottler::~QGenericSignalThrottler()
{
    maybeEmitTriggered();
}

int QGenericSignalThrottler::timeout() const
{
    return m_timer.interval();
}

void QGenericSignalThrottler::setTimeout(int timeout)
{
    if (m_timer.interval() == timeout)
        return;
    m_timer.setInterval(timeout);
    emit this->timeoutChanged(timeout);
}

Qt::TimerType QGenericSignalThrottler::timerType() const
{
    return m_timer.timerType();
}

void QGenericSignalThrottler::setTimerType(Qt::TimerType timerType)
{
    if (m_timer.timerType() == timerType)
        return;
    m_timer.setTimerType(timerType);
    emit this->timerTypeChanged(timerType);
}

void QGenericSignalThrottler::throttle()
{
    m_hasPendingEmission = true;

    switch (m_emissionPolicy) {
    case EmissionPolicy::Leading:
        // Emit only if we haven't emitted already. We know if that's
        // the case by checking if the timer is running.
        if (!m_timer.isActive())
            emitTriggered();
        break;
    case EmissionPolicy::Trailing:
        break;
    }

    // The timer is started in all cases. If we got a signal,
    // and we're Leading, and we did emit because of that,
    // then we don't re-emit when the timer fires (unless we get ANOTHER
    // signal).
    switch (m_type) {
    case Type::Throttler:
        if (!m_timer.isActive())
            m_timer.start(); // = actual start, not restart
        break;
    case Type::Debouncer:
        m_timer.start(); // = restart
        break;
    }

    Q_ASSERT(m_timer.isActive());
}

void QGenericSignalThrottler::cancel()
{
    m_hasPendingEmission = false;
    m_timer.stop();
}

void QGenericSignalThrottler::maybeEmitTriggered()
{
    if (m_hasPendingEmission)
        emitTriggered();
    else
        m_timer.stop();
}

void QGenericSignalThrottler::emitTriggered()
{
    Q_ASSERT(m_hasPendingEmission);
    m_hasPendingEmission = false;
    emit this->triggered();
}
