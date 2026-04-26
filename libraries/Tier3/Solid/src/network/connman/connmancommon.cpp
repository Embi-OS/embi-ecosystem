#include "connmancommon.h"

#include "networksettings.h"

//Conversion operations for datatypes
const QDBusArgument &operator>>(const QDBusArgument &argument, ConnmanMapStruct &obj)
{
    argument.beginStructure();
    argument >> obj.objectPath >> obj.propertyMap;
    argument.endStructure();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const ConnmanMapStruct &obj)
{
    argument.beginStructure();
    argument<<obj.objectPath<<obj.propertyMap;
    argument.endStructure();
    return argument;
}

const QString &operator>>(const QString &argument, NetworkSettingsType &obj)
{
    if (argument == Q_AttributeEthernet) {
        obj.setType(NetworkSettingsType::Wired);
    }
    else if (argument == Q_AttributeWifi) {
        obj.setType(NetworkSettingsType::Wifi);
    }
    else if (argument == Q_AttributeBluetooth) {
        obj.setType(NetworkSettingsType::Bluetooth);
    }
    else {
        obj.setType(NetworkSettingsType::Unknown);
    }

    return argument;
}

const QString &operator>>(const QString &argument, NetworkSettingsState &obj)
{
    if (argument == Q_AttributeIdle) {
        obj.setState(NetworkSettingsState::Idle);
    }
    else if (argument == Q_AttributeFailure) {
        obj.setState(NetworkSettingsState::Failure);
    }
    else if (argument == Q_AttributeAssociation) {
        obj.setState(NetworkSettingsState::Association);
    }
    else if (argument == Q_AttributeConfiguration) {
        obj.setState(NetworkSettingsState::Configuration);
    }
    else if (argument == Q_AttributeReady) {
        obj.setState(NetworkSettingsState::Ready);
    }
    else if (argument == Q_AttributeDisconnect) {
        obj.setState(NetworkSettingsState::Disconnect);
    }
    else if (argument == Q_AttributeOneline) {
        obj.setState(NetworkSettingsState::Online);
    }
    else {
        obj.setState(NetworkSettingsState::Undefined);
    }
    return argument;
}
