#include "joinrole.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

/*!
    \qmltype JoinRole
    \inherits SingleRole
    \inqmlmodule SortFilterProxyModel
    \ingroup ProxyRoles
    \brief a role made from concatenating other roles.

    A JoinRole is a simple \l ProxyRole that concatenates other roles.

    In the following example, the \c fullName role is computed by the concatenation of the \c firstName role and the \c lastName role separated by a space :
    \code
    SortFilterProxyModel {
       sourceModel: contactModel
       proxyRoles: JoinRole {
           name: "fullName"
           roleNames: ["firstName", "lastName"]
      }
    }
    \endcode

*/

JoinRole::JoinRole(QObject *parent) : SingleRole(parent)
{
    connect(this, &JoinRole::roleNamesChanged, this, &JoinRole::roleNamesInvalidated);
    connect(this, &JoinRole::separatorChanged, this, &JoinRole::invalidate);
}

void JoinRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    QList<int> roles;
    for (const QString& roleName : std::as_const(m_roleNames)) {
        int role = proxyModel.roleForName(roleName);
        roles << role;
    }
    setRoles(roles);

    SingleRole::updateRoles(proxyModel);
}

QVariant JoinRole::data(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    QStringList result;

    for (int role : std::as_const(m_roles))
        result.append(proxyModel.sourceData(sourceIndex, role).toString());

    return result.join(m_separator);
}

}
