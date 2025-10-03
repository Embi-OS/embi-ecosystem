#include "qframetimer.h"

#include <QQuickWindow>
#include <QQmlInfo>

const int QFrameTimer::IdealFrameTime = 16667; // usec - could be made configurable via an env variable
const double QFrameTimer::MicrosInSec = double(1000 * 1000);

QFrameTimer::QFrameTimer(QObject *parent)
    : QObject(parent)
{
    m_updateTimer.setInterval(1000);
    connect(&m_updateTimer, &QTimer::timeout, this, &QFrameTimer::update);
}

QObject *QFrameTimer::window() const
{
    return m_window;
}

void QFrameTimer::setWindow(QObject *value)
{
    if (m_window == value)
        return;

    if (m_window)
        disconnect(m_window, nullptr, this, nullptr);

    m_window = value;

    if (m_window) {
        if (!connectToQuickWindow())
            qmlWarning(this)<<"The given window is neither a QQuickWindow nor a WindowObject.";
    }

    emit windowChanged();
}

bool QFrameTimer::connectToQuickWindow()
{
    QQuickWindow *quickWindow = qobject_cast<QQuickWindow*>(m_window);
    if (!quickWindow)
        return false;

    connect(quickWindow, &QQuickWindow::frameSwapped, this, &QFrameTimer::recalculateFps, Qt::UniqueConnection);

    return true;
}

void QFrameTimer::recalculateFps()
{
    int frameTime = m_timer.isValid() ? qMax(1, int(m_timer.nsecsElapsed() / 1000)) : IdealFrameTime;
    m_timer.restart();

    m_count++;
    m_sum += frameTime;
    m_min = qMin(m_min, frameTime);
    m_max = qMax(m_max, frameTime);
    m_jitter += qAbs(MicrosInSec / IdealFrameTime - MicrosInSec / frameTime);
}

void QFrameTimer::reset()
{
    m_count = 0;
    m_sum = 0;
    m_max = 0;
    m_jitter = 0;
    m_min = std::numeric_limits<int>::max();
}

double QFrameTimer::averageFps() const
{
    return m_averageFps;
}

double QFrameTimer::minimumFps() const
{
    return m_minimumFps;
}

double QFrameTimer::maximumFps() const
{
    return m_maximumFps;
}

double QFrameTimer::jitterFps() const
{
    return m_jitterFps;
}

void QFrameTimer::update()
{
    m_averageFps = m_sum ? MicrosInSec * m_count / m_sum : double(0);
    m_minimumFps = m_max ? MicrosInSec / m_max : double(0);
    m_maximumFps = m_min ? MicrosInSec / m_min : double(0);
    m_jitterFps = m_count ? m_jitter / m_count :  double(0);

    // Start counting again for the next sampling period but keep m_timer running because
    // we still need the diff between the last rendered frame and the upcoming one.
    reset();

    emit updated();
}

bool QFrameTimer::running() const
{
    return m_updateTimer.isActive();
}

void QFrameTimer::setRunning(bool value)
{
    if (value && !m_updateTimer.isActive()) {
        m_updateTimer.start();
        emit runningChanged();
        reset();
    } else if (!value && m_updateTimer.isActive()) {
        m_updateTimer.stop();
        emit runningChanged();
    }
}

int QFrameTimer::interval() const
{
    return m_updateTimer.interval();
}

void QFrameTimer::setInterval(int value)
{
    if (value != m_updateTimer.interval()) {
        m_updateTimer.setInterval(value);
        emit intervalChanged();
    }
}
