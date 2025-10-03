#include "switchrole.h"
#include "qqmlsortfilterproxymodel.h"
#include "filters/filter.h"

#include <QQmlInfo>

namespace QQsfpm {

/*!
    \qmltype SwitchRole
    \inherits SingleRole
    \inqmlmodule SortFilterProxyModel
    \ingroup ProxyRoles
    \ingroup FilterContainer
    \brief A role using \l Filter to conditionnaly compute its data.

    A SwitchRole is a \l ProxyRole that computes its data with the help of \l Filter.
    Each top level filters specified in the \l SwitchRole is evaluated on the rows of the model, if a \l Filter evaluates to true, the data of the \l SwitchRole for this row will be the one of the attached \l {value} {SwitchRole.value} property.
    If no top level filters evaluate to true, the data will default to the one of the \l defaultRoleName (or the \l defaultValue if no \l defaultRoleName is specified).

    In the following example, the \c favoriteOrFirstNameSection role is equal to \c * if the \c favorite role of a row is true, otherwise it's the same as the \c firstName role :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       proxyRoles: SwitchRole {
           name: "favoriteOrFirstNameSection"
           filters: ValueFilter {
               roleName: "favorite"
               value: true
               SwitchRole.value: "*"
           }
           defaultRoleName: "firstName"
        }
    }
    \endcode
    \sa FilterContainer
*/
SwitchRoleAttached::SwitchRoleAttached(QObject* parent) : QObject (parent)
{
    if (!qobject_cast<Filter*>(parent))
        qmlInfo(parent)<<"SwitchRole must be attached to a Filter";
}

SwitchRole::SwitchRole(QObject *parent) : SingleRole(parent)
{
    connect(this, &SwitchRole::defaultRoleNameChanged, this, &SwitchRole::roleNamesInvalidated);
    connect(this, &SwitchRole::defaultRoleChanged, this, &SwitchRole::invalidate);
}

/*!
    \qmlproperty list<Filter> SwitchRole::filters
    \default

    This property holds the list of filters for this proxy role.
    The data of this role will be equal to the attached \l {value} {SwitchRole.value} property of the first filter that matches the model row.

    \sa Filter, FilterContainer
*/

void SwitchRole::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    for (Filter* filter : std::as_const(m_filters))
        filter->proxyModelCompleted(proxyModel);
}

void SwitchRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    int role = proxyModel.roleForName(m_defaultRoleName);
    setDefaultRole(role);

    ProxyRole::updateRoles(proxyModel);
}

SwitchRoleAttached* SwitchRole::qmlAttachedProperties(QObject* object)
{
    return new SwitchRoleAttached(object);
}

QVariant SwitchRole::data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel &proxyModel)
{
    for (auto filter: std::as_const(m_filters)) {
        if (!filter->getEnabled())
            continue;
        if (filter->filterAcceptsRow(sourceIndex, proxyModel)) {
            auto attached = static_cast<SwitchRoleAttached*>(qmlAttachedPropertiesObject<SwitchRole>(filter, false));
            if (!attached) {
                qWarning()<<"No SwitchRole.value provided for this filter"<<filter;
                continue;
            }
            QVariant value = attached->getValue();
            if (!value.isValid()) {
                qWarning()<<"No SwitchRole.value provided for this filter"<<filter;
                continue;
            }
            return value;
        }
    }
    if (m_defaultRole>=0)
        return proxyModel.sourceData(sourceIndex, m_defaultRole);
    return getDefaultValue();
}

void SwitchRole::onFilterAppended(Filter *filter)
{
    connect(filter, &Filter::invalidated, this, &SwitchRole::invalidate);
    auto attached = static_cast<SwitchRoleAttached*>(qmlAttachedPropertiesObject<SwitchRole>(filter, true));
    connect(attached, &SwitchRoleAttached::valueChanged, this, &SwitchRole::invalidate);
    invalidate();
}

void SwitchRole::onFilterRemoved(Filter *filter)
{
    Q_UNUSED(filter)
    invalidate();
}

void SwitchRole::onFiltersCleared()
{
    invalidate();
}

}
