#include "qwatchdog.h"
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>
#include <QMutex>
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>

#include <unistd.h>
#ifdef Q_OS_LINUX
#include <sys/reboot.h>
#endif

#include "qutils_log.h"

#ifdef Q_OS_WIN
# include <Windows.h>
#endif

#ifndef APPCONTROLLER_CMD
#define APPCONTROLLER_CMD "appcontroller"
#endif

#define FREQUENCY_MSECS 1000 // ms
#define WARNING_TIME_BLOCKED 3000 // ms
#define MAX_TIME_BLOCKED 60000 // ms

class QWatchdogWorker : public QObject
{
public:
    enum Option {
        OptionNone = 0,
        OptionDebugBreak = 1
    };
    typedef int Options;

private:
    QWatchdogWorker(Options options=OptionNone)
        : QObject()
        , m_watchTimer(new QTimer(this))
        , m_options(options)
    {
        connect(m_watchTimer, &QTimer::timeout, this, &QWatchdogWorker::checkUI);
    }

    ~QWatchdogWorker()
    {
        stop();
    }

    void start(int frequency_msecs)
    {
        m_watchTimer->start(frequency_msecs);
        m_elapsedTimeSinceLastBeat.start();
    }

    void stop()
    {
        m_watchTimer->stop();
    }

    void checkUI()
    {
        qint64 elapsed;

        {
            QMutexLocker l(&m_mutex);
            elapsed = m_elapsedTimeSinceLastBeat.elapsed();
        }

        if (elapsed > WARNING_TIME_BLOCKED) {
            QUTILSLOG_WARNING()<<"UI is blocked !"<<elapsed;  // Add custom action here!
            if ((m_options & OptionDebugBreak))
                debugBreak();
        }
        if (elapsed > MAX_TIME_BLOCKED) {
            QUTILSLOG_CRITICAL()<<"UI was blocked for more than"<<MAX_TIME_BLOCKED/1000 <<"seconds!";
            stop();
            restart();
        }
    }

    void debugBreak()
    {
#ifdef Q_OS_WIN
        DebugBreak();
#endif
    }

    void restart()
    {
        QUTILSLOG_INFO()<<"Restarting application...";
        qApp->exit();
#if defined(Q_OS_BOOT2QT)
        QProcess::startDetached(APPCONTROLLER_CMD, QStringList("--restart"));
#elif defined(Q_OS_LINUX)
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
#endif
    }

    void reset()
    {
        QMutexLocker l(&m_mutex);
        m_elapsedTimeSinceLastBeat.restart();
    }

    QTimer *const m_watchTimer;
    QElapsedTimer m_elapsedTimeSinceLastBeat;
    QMutex m_mutex;
    const Options m_options;
    friend class QWatchdog;
};

//──────────────────────────────────────────────────────────────────────-----------------
// QWatchdog
//──────────────────────────────────────────────────────────────────────-----------------

QWatchdog::QWatchdog(QObject *parent)
    : QObject(parent)
    , m_uiTimer(new QTimer(this))
{
    connect(this, &QWatchdog::enabledChanged, this, &QWatchdog::onEnabledChanged);
    connect(m_uiTimer, &QTimer::timeout, this, &QWatchdog::onUiBeat);
}

QWatchdog::~QWatchdog()
{
    stop();
}

void QWatchdog::classBegin()
{

}
void QWatchdog::componentComplete()
{
    m_completed = true;
    restart(FREQUENCY_MSECS);
}

void QWatchdog::restart(int frequency_ms)
{
    if(!m_completed)
        return;

    stop();

    if(m_enabled)
    {
        start(frequency_ms);
    }
}

void QWatchdog::start(int frequency_msecs)
{
    if (m_worker)
        return;

    m_uiTimer->start(frequency_msecs);

    m_worker = new QWatchdogWorker();
    m_watchDogThread = new QThread(this);
    m_worker->moveToThread(m_watchDogThread);
    m_watchDogThread->start();

    connect(m_watchDogThread, &QThread::started, m_worker, [this, frequency_msecs] {

        // Ensure that watchdog is running on any core
#if defined(Q_OS_BOOT2QT) && defined(Q_DEVICE_APALIS_IMX8) && defined(Q_MANUAL_CORE_AFFINITY)
        int pid = syscall(SYS_gettid);
        cpu_set_t mask;
        CPU_ZERO(&mask);
        for (int i = 0; i < CPU_SETSIZE; i++) {
            CPU_SET(i, &mask);
        }
        if (sched_setaffinity(pid, sizeof(cpu_set_t), &mask) == -1) {
            qCritical() << "Error while executing sched_setaffinity to reset affinity";
            assert(false);
        }
#endif

        m_worker->start(frequency_msecs);
    });
}

void QWatchdog::stop()
{
    if (!m_worker)
        return;

    m_uiTimer->stop();
    m_worker->deleteLater();
    m_watchDogThread->quit();
    m_watchDogThread->wait(2000);
    delete m_watchDogThread;

    m_watchDogThread = nullptr;
    m_worker = nullptr;
}

void QWatchdog::onEnabledChanged()
{
    restart(FREQUENCY_MSECS);
}

void QWatchdog::onUiBeat()
{
    m_worker->reset();
}
