#include "ClockApi.h"

ClockApi::ClockApi(QObject* parent) :
    AbstractManager(parent),
    m_worker(new HappyServerWorker(this))
{

}

bool ClockApi::init()
{
    if(m_worker->getRunning())
        return true;

    HappyServer* server = QQmlLoader::load<HappyServer>("L02_Clock", "ClockApiPreparator");

    return m_worker->start(server);
}

bool ClockApi::unInit()
{
    return m_worker->stop();
}
