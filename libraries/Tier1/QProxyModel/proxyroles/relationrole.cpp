#include "relationrole.h"
#include "qqmlsortfilterproxymodel.h"

namespace QQsfpm {

RelationRole::RelationRole(QObject *parent) :
    SingleRole(parent),
    m_proxyModel(new QProxyModel(this))
{
    m_proxyModel->setSortOrder(Qt::AscendingOrder);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseSensitive);
    m_proxyModel->setSortLocaleAware(false);

    connect(m_proxyModel, &QAbstractItemModel::dataChanged, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::headerDataChanged, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::rowsInserted, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::rowsRemoved, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::rowsMoved, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::columnsRemoved, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::columnsInserted, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::columnsMoved, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::modelReset, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::layoutChanged, this, &RelationRole::invalidate);
    connect(m_proxyModel, &QProxyModel::roleNamesChanged, this, &RelationRole::roleNamesInvalidated);
    connect(this, &RelationRole::customRoleNamesChanged, this, &RelationRole::roleNamesInvalidated);

    connect(this, &RelationRole::delayedChanged, m_proxyModel, &QProxyModel::setDelayed);
    connect(this, &RelationRole::modelRoleNameChanged, m_proxyModel, &QProxyModel::setSortRoleName);
    connect(this, &RelationRole::modelRoleChanged, m_proxyModel, &QProxyModel::setSortRole);

    connect(this, &RelationRole::roleNameChanged, this, &RelationRole::roleNamesInvalidated);
    connect(this, &RelationRole::roleChanged, this, &RelationRole::invalidate);
    connect(this, &RelationRole::modelRoleNameChanged, this, &RelationRole::invalidate);
    connect(this, &RelationRole::modelRoleChanged, this, &RelationRole::invalidate);
    connect(this, &RelationRole::modelAboutToChange, this, &RelationRole::onModelAboutToChange);
    connect(this, &RelationRole::modelChanged, this, &RelationRole::onModelChanged);
    connect(this, &RelationRole::defaultObjectChanged, this, &RelationRole::invalidate);
}

void RelationRole::onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel)
{
    if(oldModel!=nullptr) {
        disconnect(oldModel, nullptr,this, nullptr);
    }
}

void RelationRole::onModelChanged(QAbstractItemModel* model)
{
    m_proxyModel->setSourceModel(model);

    invalidate();
}

void RelationRole::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    Q_UNUSED(proxyModel)
    m_proxyModel->invalidate();
}

void RelationRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    int role = proxyModel.roleForName(m_roleName);
    setRole(role);

    m_proxyRoleNames = m_proxyModel->roleNames();
    if(!m_customRoleNames.isEmpty())
    {
        const QHash<int,QByteArray> roleNames = m_proxyRoleNames;
        m_proxyRoleNames.clear();
        m_proxyRoleNames.reserve(m_customRoleNames.size());
        for(const QString& customRoleName: m_customRoleNames)
        {
            const QByteArray roleName = customRoleName.toUtf8();
            m_proxyRoleNames[roleNames.key(roleName)] = roleName;
        }
    }

    m_proxyDefaultObject.clear();
    for (auto [key, value] : m_defaultObject.asKeyValueRange()) {
        m_proxyDefaultObject.insert(m_proxyModel->roleForName(key), value);
    }

    ProxyRole::updateRoles(proxyModel);
}

QVariant RelationRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    const QVariant& sourceVal = proxyModel.sourceData(sourceIndex, m_role);

    const int index = QQsfpm::indexOf(m_proxyModel, m_proxyModel->sortRole(), sourceVal);

    QVariantMap map;
    const QModelIndex modelIndex = m_proxyModel->index(index, 0);
    for(auto [key, value]: m_proxyRoleNames.asKeyValueRange()) {
        const QVariant& val = m_proxyModel->data(modelIndex, key);
        map.insert(value, val.isValid() ? val : m_proxyDefaultObject.value(key));
    }

    return map;
}

}
