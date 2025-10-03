#ifndef ROLEFILTER_H
#define ROLEFILTER_H

#include "filter.h"

namespace QQsfpm {

class RoleFilter : public QQsfpm::Filter
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, -1)

public:
    explicit RoleFilter(QObject *parent = nullptr);

    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

protected:
    QVariant sourceData(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const;
};

}

#endif // ROLEFILTER_H
