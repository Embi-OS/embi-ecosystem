#ifndef CONNMAN_SERVICE_INTERFACE_H
#define CONNMAN_SERVICE_INTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtDBus>

/*
 * Proxy class for interface net.connman.Service
 */
class NetConnmanServiceInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "net.connman.Service"; }

public:
    NetConnmanServiceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~NetConnmanServiceInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<> ClearProperty(const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(name);
        return asyncCallWithArgumentList(QStringLiteral("ClearProperty"), argumentList);
    }

    inline QDBusPendingReply<> Connect()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Connect"), argumentList);
    }

    inline QDBusPendingReply<> Disconnect()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Disconnect"), argumentList);
    }

    inline QDBusPendingReply<QVariantMap> GetProperties()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetProperties"), argumentList);
    }

    inline QDBusPendingReply<> MoveAfter(const QDBusObjectPath &service)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(service);
        return asyncCallWithArgumentList(QStringLiteral("MoveAfter"), argumentList);
    }

    inline QDBusPendingReply<> MoveBefore(const QDBusObjectPath &service)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(service);
        return asyncCallWithArgumentList(QStringLiteral("MoveBefore"), argumentList);
    }

    inline QDBusPendingReply<> Remove()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("Remove"), argumentList);
    }

    inline QDBusPendingReply<> ResetCounters()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("ResetCounters"), argumentList);
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
    using Service = ::NetConnmanServiceInterface;
  }
}
#endif
