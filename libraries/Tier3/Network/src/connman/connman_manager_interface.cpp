#include "connman_manager_interface.h"

/*
 * Implementation of interface class NetConnmanManagerInterface
 */

NetConnmanManagerInterface::NetConnmanManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

NetConnmanManagerInterface::~NetConnmanManagerInterface()
{
}

