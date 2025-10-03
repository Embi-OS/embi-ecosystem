#include "abstractmanager.h"

AbstractManager::AbstractManager(QObject *parent) :
    QObject(parent)
{

}

QString AbstractManager::managerName() const
{
    return metaObject()->className();
}

void AbstractManager::postInit()
{
    emitInitDone(true);
}

void AbstractManager::emitInitDone(bool result)
{
    setInitializing(false);
    setReady(result);

    emit this->initDone(result);
}
