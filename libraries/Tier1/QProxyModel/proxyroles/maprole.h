#ifndef MAPROLE_H
#define MAPROLE_H

#include "proxyrole.h"
#include <QVariantMap>

namespace QQsfpm {

class MapRole : public QQsfpm::ProxyRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, -1)
    Q_WRITABLE_REF_PROPERTY(QString, aliasName, AliasName, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, customNames, CustomNames, {})
    Q_WRITABLE_REF_PROPERTY(QString, separator, Separator, "_")
    Q_WRITABLE_REF_PROPERTY(QVariantMap, defaultObject, DefaultObject, {})

public:
    explicit MapRole(QObject *parent = nullptr);

    QStringList names() override;
    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

private:
    QVariant data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel, const QString &roleName) override;
    bool setData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& roleName) override;
};

}

#endif // MAPROLE_H
