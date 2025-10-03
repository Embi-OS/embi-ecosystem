#include "maprole.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

MapRole::MapRole(QObject *parent) :
    ProxyRole(parent)
{
    connect(this, &MapRole::roleNameChanged, this, &MapRole::roleNamesInvalidated);
    connect(this, &MapRole::roleChanged, this, &MapRole::invalidate);
    connect(this, &MapRole::aliasNameChanged, this, &MapRole::invalidate);
    connect(this, &MapRole::defaultObjectChanged, this, &MapRole::invalidate);
}

QStringList MapRole::names()
{
    QStringList names;
    QStringList custom = m_customNames;
    if(custom.empty())
        custom = m_defaultObject.keys();

    names.reserve(custom.size());
    for (const QString& name : std::as_const(custom))
        names.append(m_aliasName.isEmpty() ? name : (m_aliasName+m_separator+name));

    return names;
}

void MapRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    int role = proxyModel.roleForName(m_roleName);
    setRole(role);

    ProxyRole::updateRoles(proxyModel);
}

QVariant MapRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel, const QString& roleName)
{
    const QVariant& source = proxyModel.sourceData(sourceIndex, m_role);
    if(source.metaType().id()!=QMetaType::QVariantMap)
        return QVariant();

    const QVariantMap& sourceMap = *reinterpret_cast<const QVariantMap*>(source.constData());
    const QString& name = m_aliasName.isEmpty() ? roleName : roleName.mid(m_aliasName.size()+m_separator.size());

    const QVariant& val = sourceMap.value(name);

    if(!val.isValid() && m_defaultObject.contains(name))
        return m_defaultObject.value(name);

    return val;
}

bool MapRole::setData(const QModelIndex& sourceIndex, const QVariant &value, const QQmlSortFilterProxyModel& proxyModel, const QString& roleName)
{
    const QVariant& source = proxyModel.sourceData(sourceIndex, m_role);
    if(source.metaType().id()!=QMetaType::QVariantMap)
        return false;

    const QVariantMap& sourceMap = *reinterpret_cast<const QVariantMap*>(source.constData());
    const QString& name = m_aliasName.isEmpty() ? roleName : roleName.mid(m_aliasName.size()+m_separator.size());

    QVariantMap map = m_defaultObject;
    map.insert(sourceMap);
    map.insert(name, value);

    return proxyModel.setSourceData(sourceIndex, map, m_role);
}

}
