#ifndef CONNMAN_TECHNOLOGY_INTERFACE_H
#define CONNMAN_TECHNOLOGY_INTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtDBus>

/*
 * Proxy class for interface net.connman.Technology
 */
class NetConnmanTechnologyInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "net.connman.Technology"; }

public:
    NetConnmanTechnologyInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~NetConnmanTechnologyInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<QVariantMap> GetProperties()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetProperties"), argumentList);
    }

    inline QDBusPendingReply<> Scan()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Scan"), argumentList);
    }

    inline QDBusPendingReply<> SetProperty(const QString &name, const QDBusVariant &value)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(name)<<QVariant::fromValue(value);
        return asyncCallWithArgumentList(QStringLiteral("SetProperty"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void PropertyChanged(const QString &name, const QDBusVariant &value);
};

namespace net {
  namespace connman {
    using Technology = ::NetConnmanTechnologyInterface;
  }
}
#endif
