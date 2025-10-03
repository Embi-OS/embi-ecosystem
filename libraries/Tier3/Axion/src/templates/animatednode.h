#ifndef ANIMATEDNODE_H
#define ANIMATEDNODE_H

#include <QtCore/qobject.h>
#include <QtQuick/qsgnode.h>
#include <QtCore/qelapsedtimer.h>

class QQuickItem;
class QQuickWindow;

class AnimatedNode : public QObject, public QSGTransformNode
{
    Q_OBJECT

public:
    explicit AnimatedNode(QQuickItem *target);

    bool isRunning() const;

    int currentTime() const;
    void setCurrentTime(int time);

    int duration() const;
    void setDuration(int duration);

    enum LoopCount { Infinite = -1 };

    int loopCount() const;
    void setLoopCount(int count);

    virtual void sync(QQuickItem *target);

    QQuickWindow *window() const;

    // must be called from sync() or updatePaintNode()
    void start(int duration = 0);
    void restart();
    void stop();

Q_SIGNALS:
    void started();
    void stopped();

protected:
    virtual void updateCurrentTime(int time);

private Q_SLOTS:
    void advance();
    void update();

private:
    bool m_running = false;
    int m_duration = 0;
    int m_loopCount = 1;
    int m_currentTime = 0;
    int m_currentLoop = 0;
    QElapsedTimer m_timer;
    QQuickWindow *m_window = nullptr;
};

#endif // QQUICKANIMATEDNODE_P_H
