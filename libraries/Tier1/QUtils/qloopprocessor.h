#ifndef QLOOPPROCESSOR_H
#define QLOOPPROCESSOR_H

#include <QDefs>

class QLoopProcessor : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(LoopProcessor)

    Q_WRITABLE_VAR_PROPERTY(bool, autoDelete, AutoDelete, true)
    Q_WRITABLE_VAR_PROPERTY(bool, asynchronous, Asynchronous, true)
    Q_WRITABLE_VAR_PROPERTY(int, timeout, Timeout, 16)

    Q_READONLY_VAR_PROPERTY(int, index, Index, 0)
    Q_READONLY_VAR_PROPERTY(int, release, Release, 0)
    Q_READONLY_VAR_PROPERTY(bool, running, Running, false)

public:
    explicit QLoopProcessor(QObject *parent = nullptr);

    QLoopProcessor* onStart(std::function<bool()> callback);
    QLoopProcessor* onIterate(std::function<bool()> callback);
    QLoopProcessor* onRelease(std::function<void()> callback);
    QLoopProcessor* onEnd(std::function<void()> callback);

public slots:
    bool run();

private slots:
    bool start();
    void iterate();
    void release();
    void end();

private:
    bool m_mustRelease=false;
    QElapsedTimer m_elapsedSinceLastRelease;
    QElapsedTimer m_elapsedTotal;

    std::function<bool()> m_startCallback=nullptr;
    std::function<bool()> m_iterateCallback=nullptr;
    std::function<void()> m_releaseCallback=nullptr;
    std::function<void()> m_endCallback=nullptr;
};

#endif // QLOOPPROCESSOR_H
