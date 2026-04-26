#include "connman_service_interface.h"

/*
 * Implementation of interface class NetConnmanServiceInterface
 */

NetConnmanServiceInterface::NetConnmanServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

NetConnmanServiceInterface::~NetConnmanServiceInterface()
{
}

