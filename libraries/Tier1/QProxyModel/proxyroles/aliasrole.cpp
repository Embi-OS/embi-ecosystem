#include "aliasrole.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

AliasRole::AliasRole(QObject *parent) : SingleRole(parent)
{
    connect(this, &AliasRole::roleNameChanged, this, &AliasRole::roleNamesInvalidated);
    connect(this, &AliasRole::roleChanged, this, &AliasRole::invalidate);
}

void AliasRole::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{

}

void AliasRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    int role = proxyModel.roleForName(m_roleName);
    setRole(role);

    ProxyRole::updateRoles(proxyModel);
}

QVariant AliasRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    return proxyModel.sourceData(sourceIndex, m_role);
}

bool AliasRole::setData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& roleName)
{
    return proxyModel.setSourceData(sourceIndex, value, m_role);
}

}
