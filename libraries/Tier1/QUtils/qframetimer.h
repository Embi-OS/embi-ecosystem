#ifndef QFRAMETIMER_H
#define QFRAMETIMER_H

#include <QElapsedTimer>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QDefs>

class QFrameTimer : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(FrameTimer)

    Q_PROPERTY(double averageFps READ averageFps NOTIFY updated FINAL)
    Q_PROPERTY(double minimumFps READ minimumFps NOTIFY updated FINAL)
    Q_PROPERTY(double maximumFps READ maximumFps NOTIFY updated FINAL)
    Q_PROPERTY(double jitterFps READ jitterFps NOTIFY updated FINAL)

    Q_PROPERTY(QObject* window READ window WRITE setWindow NOTIFY windowChanged FINAL)

    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged FINAL)
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged FINAL)

public:
    QFrameTimer(QObject *parent = nullptr);

    Q_INVOKABLE void update();

    double averageFps() const;
    double minimumFps() const;
    double maximumFps() const;
    double jitterFps() const;

    QObject *window() const;
    void setWindow(QObject *value);

    bool running() const;
    void setRunning(bool value);

    int interval() const;
    void setInterval(int value);

public slots:
    void reset();

signals:
    void updated();
    void intervalChanged();
    void runningChanged();
    void windowChanged();

protected slots:
    void recalculateFps();

protected:
    QPointer<QObject> m_window;
    bool connectToQuickWindow();

private:

    int m_count = 0;
    int m_sum = 0;
    int m_min = std::numeric_limits<int>::max();
    int m_max = 0;
    double m_jitter = 0.0;

    QElapsedTimer m_timer;

    QTimer m_updateTimer;

    double m_averageFps = 0;
    double m_minimumFps = 0;
    double m_maximumFps = 0;
    double m_jitterFps = 0;

    static const int IdealFrameTime;
    static const double MicrosInSec;
};

#endif // QFRAMETIMER_H
