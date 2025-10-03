#include "connman_technology_interface.h"

/*
 * Implementation of interface class NetConnmanTechnologyInterface
 */

NetConnmanTechnologyInterface::NetConnmanTechnologyInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

NetConnmanTechnologyInterface::~NetConnmanTechnologyInterface()
{
}

