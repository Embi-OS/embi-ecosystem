#ifndef CONNMAN_MANAGER_INTERFACE_H
#define CONNMAN_MANAGER_INTERFACE_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtDBus>

#include "connmancommon.h"

/*
 * Proxy class for interface net.connman.Manager
 */
class NetConnmanManagerInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "net.connman.Manager"; }

public:
    NetConnmanManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~NetConnmanManagerInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<QDBusObjectPath> CreateSession(const QVariantMap &settings, const QDBusObjectPath &notifier)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(settings)<<QVariant::fromValue(notifier);
        return asyncCallWithArgumentList(QStringLiteral("CreateSession"), argumentList);
    }

    inline QDBusPendingReply<> DestroySession(const QDBusObjectPath &session)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(session);
        return asyncCallWithArgumentList(QStringLiteral("DestroySession"), argumentList);
    }

    inline QDBusPendingReply<QVariantMap> GetProperties()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetProperties"), argumentList);
    }

    inline QDBusPendingReply<ConnmanMapStructList> GetSavedServices()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetSavedServices"), argumentList);
    }

    inline QDBusPendingReply<ConnmanMapStructList> GetServices()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetServices"), argumentList);
    }

    inline QDBusPendingReply<ConnmanMapStructList> GetTechnologies()
    {
        QList<QVariant> argumentList;
        return asyncCallWithArgumentList(QStringLiteral("GetTechnologies"), argumentList);
    }

    inline QDBusPendingReply<> RegisterAgent(const QDBusObjectPath &path)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(path);
        return asyncCallWithArgumentList(QStringLiteral("RegisterAgent"), argumentList);
    }

    inline QDBusPendingReply<> RegisterCounter(const QDBusObjectPath &path, uint accuracy, uint period)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(path)<<QVariant::fromValue(accuracy)<<QVariant::fromValue(period);
        return asyncCallWithArgumentList(QStringLiteral("RegisterCounter"), argumentList);
    }

    inline QDBusPendingReply<> ReleasePrivateNetwork(const QDBusObjectPath &path)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(path);
        return asyncCallWithArgumentList(QStringLiteral("ReleasePrivateNetwork"), argumentList);
    }

    inline QDBusPendingReply<> RemoveSavedService(const QString &identifier)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(identifier);
        return asyncCallWithArgumentList(QStringLiteral("RemoveSavedService"), argumentList);
    }

    inline QDBusPendingReply<QDBusObjectPath, QVariantMap, int> RequestPrivateNetwork(const QVariantMap &options)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(options);
        return asyncCallWithArgumentList(QStringLiteral("RequestPrivateNetwork"), argumentList);
    }
    inline QDBusReply<QDBusObjectPath> RequestPrivateNetwork(const QVariantMap &options, QVariantMap &dict, int &fd)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(options);
        QDBusMessage reply = callWithArgumentList(QDBus::Block, QStringLiteral("RequestPrivateNetwork"), argumentList);
        if (reply.type() == QDBusMessage::ReplyMessage && reply.arguments().size() == 3) {
            dict = qdbus_cast<QVariantMap>(reply.arguments().at(1));
            fd = qdbus_cast<int>(reply.arguments().at(2));
        }
        return reply;
    }

    inline QDBusPendingReply<> ResetCounters(const QString &type)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(type);
        return asyncCallWithArgumentList(QStringLiteral("ResetCounters"), argumentList);
    }

    inline QDBusPendingReply<> SetProperty(const QString &name, const QDBusVariant &value)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(name)<<QVariant::fromValue(value);
        return asyncCallWithArgumentList(QStringLiteral("SetProperty"), argumentList);
    }

    inline QDBusPendingReply<> UnregisterAgent(const QDBusObjectPath &path)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(path);
        return asyncCallWithArgumentList(QStringLiteral("UnregisterAgent"), argumentList);
    }

    inline QDBusPendingReply<> UnregisterCounter(const QDBusObjectPath &path)
    {
        QList<QVariant> argumentList;
        argumentList<<QVariant::fromValue(path);
        return asyncCallWithArgumentList(QStringLiteral("UnregisterCounter"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void PropertyChanged(const QString &name, const QDBusVariant &value);
    void SavedServicesChanged(ConnmanMapStructList changed);
    void ServicesChanged(ConnmanMapStructList changed, const QList<QDBusObjectPath> &removed);
    void TechnologyAdded(const QDBusObjectPath &technology, const QVariantMap &properties);
    void TechnologyRemoved(const QDBusObjectPath &technology);
};

namespace net {
  namespace connman {
    using Manager = ::NetConnmanManagerInterface;
  }
}
#endif
