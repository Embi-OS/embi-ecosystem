#ifndef ALIASROLE_H
#define ALIASROLE_H

#include "singlerole.h"

namespace QQsfpm {

class AliasRole : public QQsfpm::SingleRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, -1)

public:
    explicit AliasRole(QObject *parent = nullptr);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;
    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
    bool setData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& roleName) override;
};

}

#endif // ALIASROLE_H
