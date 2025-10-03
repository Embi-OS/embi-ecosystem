#include "A02_ClockManager.h"

#include "restmanager.h"
#include "sqlmanager.h"

#include "ClockApi.h"
#include "Clock.h"
#include "ClockDisplay.h"
#include "ClockMedia.h"

#include <QMediaDevices>
#include <QAudioDevice>

A02_ClockManager::A02_ClockManager(QObject *parent) :
    MainManagerContainer(parent)
{

}

bool A02_ClockManager::init()
{
    MainManagerContainer::init();

    registerManager(RestManager::Get());

    if(RestManager::Get()->localhost())
    {
        SqlManager::Get()->setDbType(SqlDatabaseTypes::SQLite);
        SqlManager::Get()->setDbPath(Paths::database());
        SqlManager::Get()->setDbName("clock");

        registerManager(SqlManager::Get());
        registerManager(ClockApi::Get());
    }

    registerManager(ClockDisplay::Get());
    registerManager(ClockMedia::Get());
    registerManager(Clock::Get());

    return true;
}
