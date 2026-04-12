#include "abstractmanager.h"

AbstractManager::AbstractManager(QObject *parent) :
    QObject(parent)
{

}

QString AbstractManager::managerName() const
{
    return metaObject()->className();
}
