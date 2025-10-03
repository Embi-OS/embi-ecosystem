#include "animatednode.h"

#include <QtQuick/qquickitem.h>
#include <QtQuick/qquickwindow.h>

// based on qtdeclarative/examples/quick/scenegraph/threadedanimation

AnimatedNode::AnimatedNode(QQuickItem *target)
    : m_window(target->window())
{
}

bool AnimatedNode::isRunning() const
{
    return m_running;
}

int AnimatedNode::currentTime() const
{
    int time = m_currentTime;
    if (m_running)
        time += m_timer.elapsed();
    return time;
}

void AnimatedNode::setCurrentTime(int time)
{
    m_currentTime = time;
    m_timer.restart();
}

int AnimatedNode::duration() const
{
    return m_duration;
}

void AnimatedNode::setDuration(int duration)
{
    m_duration = duration;
}

int AnimatedNode::loopCount() const
{
    return m_loopCount;
}

void AnimatedNode::setLoopCount(int count)
{
    m_loopCount = count;
}

void AnimatedNode::sync(QQuickItem *target)
{
    Q_UNUSED(target);
}

QQuickWindow *AnimatedNode::window() const
{
    return m_window;
}

void AnimatedNode::start(int duration)
{
    if (m_running)
        return;

    m_running = true;
    m_currentLoop = 0;
    m_timer.restart();
    if (duration > 0)
        m_duration = duration;

    connect(m_window, &QQuickWindow::beforeRendering, this, &AnimatedNode::advance, Qt::DirectConnection);
    connect(m_window, &QQuickWindow::frameSwapped, this, &AnimatedNode::update, Qt::DirectConnection);

    // If we're inside a QQuickWidget, this call is necessary to ensure the widget
    // gets updated for the first time.
    m_window->update();

    emit started();
}

void AnimatedNode::restart()
{
    stop();
    start();
}

void AnimatedNode::stop()
{
    if (!m_running)
        return;

    m_running = false;
    disconnect(m_window, &QQuickWindow::beforeRendering, this, &AnimatedNode::advance);
    disconnect(m_window, &QQuickWindow::frameSwapped, this, &AnimatedNode::update);
    emit stopped();
}

void AnimatedNode::updateCurrentTime(int time)
{
    Q_UNUSED(time);
}

void AnimatedNode::advance()
{
    int time = currentTime();
    if (time > m_duration) {
        time = 0;
        setCurrentTime(0);

        if (m_loopCount > 0 && ++m_currentLoop >= m_loopCount) {
            time = m_duration; // complete
            stop();
        }
    }
    updateCurrentTime(time);

    // If we're inside a QQuickWidget, this call is necessary to ensure the widget gets updated.
    m_window->update();
}

void AnimatedNode::update()
{
    if (m_running)
        m_window->update();
}
