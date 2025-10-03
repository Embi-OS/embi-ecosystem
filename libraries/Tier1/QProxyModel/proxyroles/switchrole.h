#ifndef SWITCHROLE_H
#define SWITCHROLE_H

#include "singlerole.h"
#include "filters/filtercontainer.h"

namespace QQsfpm {

class SwitchRoleAttached : public QObject
{
    Q_OBJECT

    Q_WRITABLE_REF_PROPERTY(QVariant, value, Value, {})

public:
    explicit SwitchRoleAttached(QObject* parent);
};

class SwitchRole : public QQsfpm::SingleRole, public QQsfpm::FilterContainer
{
    Q_OBJECT
    Q_INTERFACES(QQsfpm::FilterContainer)

    Q_WRITABLE_REF_PROPERTY(QString, defaultRoleName, DefaultRoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, defaultRole, DefaultRole, -1)

    Q_PROPERTY(QQmlListProperty<QQsfpm::Filter> filters READ filtersListProperty CONSTANT FINAL)
    Q_CLASSINFO("DefaultProperty", "filters")

public:
    explicit SwitchRole(QObject* parent = nullptr);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;
    void updateRoles(const QQmlSortFilterProxyModel& proxyModel) override;

    static SwitchRoleAttached* qmlAttachedProperties(QObject* object);

private:
    QVariant data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) override;

    void onFilterAppended(Filter *filter) override;
    void onFilterRemoved(Filter *filter) override;
    void onFiltersCleared() override;
};

}

#endif // SWITCHROLE_H
