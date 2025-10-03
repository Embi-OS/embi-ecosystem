#ifndef QQMLSORTFILTERPROXYMODEL_H
#define QQMLSORTFILTERPROXYMODEL_H

#include "qproxymodel.h"
#include "filters/filtercontainer.h"
#include "sorters/sortercontainer.h"
#include "proxyroles/proxyrolecontainer.h"

class QQmlSortFilterProxyModel : public QProxyModel,
                                 public QQsfpm::FilterContainer,
                                 public QQsfpm::SorterContainer,
                                 public QQsfpm::ProxyRoleContainer
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SortFilterProxyModel)

    Q_INTERFACES(QQsfpm::FilterContainer)
    Q_INTERFACES(QQsfpm::SorterContainer)
    Q_INTERFACES(QQsfpm::ProxyRoleContainer)

    Q_PROPERTY(QQmlListProperty<QQsfpm::Filter> filters READ filtersListProperty CONSTANT FINAL)
    Q_PROPERTY(QQmlListProperty<QQsfpm::Sorter> sorters READ sortersListProperty CONSTANT FINAL)
    Q_PROPERTY(QQmlListProperty<QQsfpm::ProxyRole> proxyRoles READ proxyRolesListProperty CONSTANT FINAL)

public:
    explicit QQmlSortFilterProxyModel(QObject* parent = nullptr);

    QVariant sourceData(const QModelIndex& sourceIndex, const QString& roleName) const final override;
    QVariant sourceData(const QModelIndex& sourceIndex, int role) const final override;
    bool setSourceData(const QModelIndex& sourceIndex, const QVariant &value, const QString& roleName) const final override;
    bool setSourceData(const QModelIndex& sourceIndex, const QVariant &value, int role) const final override;

public Q_SLOTS:
    void invalidate() final override;

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const final override;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const final override;

protected Q_SLOTS:
    void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles) final override;

    void updateRoleNames() final override;
    void updateRoles() final override;
    void updateObjectRoles();
    void queueInvalidateProxyRoles();
    void invalidateProxyRoles();

private:
    void onFilterAppended(QQsfpm::Filter* filter) final override;
    void onFilterRemoved(QQsfpm::Filter* filter) final override;
    void onFiltersCleared() final override;

    void onSorterAppended(QQsfpm::Sorter* sorter) final override;
    void onSorterRemoved(QQsfpm::Sorter* sorter) final override;
    void onSortersCleared() final override;

    void onProxyRoleAppended(QQsfpm::ProxyRole *proxyRole) final override;
    void onProxyRoleRemoved(QQsfpm::ProxyRole *proxyRole) final override;
    void onProxyRolesCleared() final override;

    int m_proxyRolesOffset;
    QHash<int, QPair<QQsfpm::ProxyRole*, QString>> m_proxyRoleMap;
    QVector<int> m_proxyRoleNumbers;

    bool m_invalidateProxyRolesQueued = false;
};

#endif // QQMLSORTFILTERPROXYMODEL_H
