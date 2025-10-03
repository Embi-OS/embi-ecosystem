#include "happyserverworker.h"
#include "happy_log.h"

HappyServerWorker::HappyServerWorker(QObject *parent) :
    QObject(parent)
{

}

bool HappyServerWorker::start(HappyServer* server)
{
    if (m_running)
    {
        HAPPYLOG_CRITICAL()<<"HappyServerWorker is already running";
        return false;
    }

    if (!server)
    {
        HAPPYLOG_CRITICAL()<<"HappyServerWorker needs a server to run";
        return false;
    }

    m_server = server;

    QEventLoop loop;
    connect(m_server, &HappyServer::started, &loop, &QEventLoop::exit, Qt::QueuedConnection);
    m_thread = new QThread(this);
    m_server->moveToThread(m_thread);
    connect(m_thread, &QThread::started, m_server, [this] {
#if defined(Q_OS_BOOT2QT) && defined(Q_DEVICE_APALIS_IMX8) && defined(Q_MANUAL_CORE_AFFINITY)
        QUtils::resetCoreAffinity();
#endif

        m_server->init();
        m_server->start();
    }, Qt::QueuedConnection);

    m_thread->start();

    int status = loop.exec();

    setRunning(true);

    if(status!=1)
        stop();

    return status==1;
}

bool HappyServerWorker::stop()
{
    if (!m_running)
    {
        HAPPYLOG_WARNING()<<"HappyServerWorker is not running";
        return true;
    }

    m_server->deleteLater();
    m_thread->quit();
    m_thread->wait(2000);
    delete m_thread;

    m_thread = nullptr;
    m_server = nullptr;

    return true;
}
