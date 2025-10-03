#ifndef ROLESORTER_H
#define ROLESORTER_H

#include "sorter.h"

namespace QQsfpm {

class RoleSorter : public QQsfpm::Sorter
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, -1)
    Q_WRITABLE_VAR_PROPERTY(Qt::CaseSensitivity, caseSensitivity, CaseSensitivity, Qt::CaseInsensitive)
    Q_WRITABLE_VAR_PROPERTY(bool, localeAware, LocaleAware, false)

public:
    explicit RoleSorter(QObject *parent = nullptr);

    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

protected:
    QPair<QVariant, QVariant> sourceData(const QModelIndex &sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const;
    int compare(const QModelIndex& sourceLeft, const QModelIndex& sourceRight, const QQmlSortFilterProxyModel& proxyModel) const override;
};

}

#endif // ROLESORTER_H
