#ifndef JOINROLE_H
#define JOINROLE_H

#include "singlerole.h"

namespace QQsfpm {

class JoinRole : public QQsfpm::SingleRole
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QStringList, roleNames, RoleNames, {})
    Q_WRITABLE_REF_PROPERTY(QList<int>, roles, Roles, {})
    Q_WRITABLE_REF_PROPERTY(QString, separator, Separator, " ")

public:
    explicit JoinRole(QObject *parent = nullptr);

    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;
};

}

#endif // JOINROLE_H
