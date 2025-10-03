#ifndef CONNMANCOMMON_H
#define CONNMANCOMMON_H

#include <QMetaType>
#include <QtDBus>
#include <QStringLiteral>

#define Q_PropertyAgentPath QStringLiteral("/ConnmanAgent")
#define Q_PropertyState QStringLiteral("State")
#define Q_PropertyName QStringLiteral("Name")
#define Q_PropertyType QStringLiteral("Type")
#define Q_PropertyConnected QStringLiteral("Connected")
#define Q_PropertyPowered QStringLiteral("Powered")
#define Q_PropertyAutoConnect QStringLiteral("AutoConnect")

#define Q_AttributeWifi QStringLiteral("wifi")
#define Q_AttributeEthernet QStringLiteral("ethernet")
#define Q_AttributeBluetooth QStringLiteral("bluetooth")
#define Q_AttributeIdle QStringLiteral("idle")
#define Q_AttributeFailure QStringLiteral("failure")
#define Q_AttributeAssociation QStringLiteral("association")
#define Q_AttributeConfiguration QStringLiteral("configuration")
#define Q_AttributeReady QStringLiteral("ready")
#define Q_AttributeDisconnect QStringLiteral("disconnect")
#define Q_AttributeOneline QStringLiteral("online")

class NetworkSettingsType;
class NetworkSettingsState;

struct ConnmanMapStruct {
    QDBusObjectPath objectPath;
    QVariantMap propertyMap;
};

QDBusArgument &operator<<(QDBusArgument &argument, const ConnmanMapStruct &obj);
const QDBusArgument &operator>>(const QDBusArgument &argument, ConnmanMapStruct &obj);

const QString &operator>>(const QString &argument, NetworkSettingsType &obj);
const QString &operator>>(const QString  &argument, NetworkSettingsState &obj);

typedef QList<ConnmanMapStruct> ConnmanMapStructList;

Q_DECLARE_METATYPE( ConnmanMapStruct )
Q_DECLARE_METATYPE( ConnmanMapStructList )

#endif // CONNMANCOMMON_H
