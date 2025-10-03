#include "qmodelmapper.h"
#include "qutils_log.h"

QModelMapper::QModelMapper(QObject *parent) :
    QModelMapper("", parent)
{

}

QModelMapper::QModelMapper(const QString& baseName, QObject *parent) :
    QVariantMapper(baseName, parent, &QModelMapper::staticMetaObject)
{
    connect(this, &QModelMapper::parentIndexChanged, this, &QModelMapper::invalidateModelIndex);
    connect(this, &QModelMapper::rowChanged, this, &QModelMapper::invalidateModelIndex);
    connect(this, &QModelMapper::columnChanged, this, &QModelMapper::invalidateModelIndex);
    connect(this, &QModelMapper::roleChanged, this, &QModelMapper::invalidateModelIndex);
    connect(this, &QModelMapper::valueChanged, this, &QModelMapper::invalidateModelIndex);
    connect(this, &QModelMapper::flagsChanged, this, &QModelMapper::invalidateModelIndex);
    connect(this, &QModelMapper::roleNameChanged, this, &QModelMapper::updateRoles);

    connect(this, &QModelMapper::modelAboutToChange, this, &QModelMapper::onModelAboutToChange);
    connect(this, &QModelMapper::modelChanged, this, &QModelMapper::onModelChanged);
}

QModelMapper::~QModelMapper()
{
    if(m_submitPolicy<QVariantMapperPolicies::Manual && m_isDirty) {
        submit();
        waitForSubmit();
    }
}

const QModelIndex& QModelMapper::getModelIndex() const
{
    return m_modelIndex;
}

bool QModelMapper::doSelect()
{
    bool result = true;
    const QVariantMap storage = selectModel(&result);
    setStorage(storage);
    emitSelectDone(result);

    return true;
}

bool QModelMapper::doSubmit(const QStringList& dirtyKeys)
{
    bool result = submitModel(dirtyKeys);
    emitSubmitDone(result);

    return result;
}

void QModelMapper::onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel)
{
    if(oldModel!=nullptr)
    {
        disconnect(oldModel, nullptr,this, nullptr);
    }
}

void QModelMapper::onModelChanged(QAbstractItemModel* model)
{
    if (model != nullptr)
    {
        connect(model, &QAbstractItemModel::modelReset, this, &QModelMapper::invalidateModelIndex);
        connect(model, &QAbstractItemModel::layoutChanged, this, &QModelMapper::invalidateModelIndex);
        connect(model, &QAbstractItemModel::dataChanged, this, &QModelMapper::onModelDataChanged);
        connect(model, &QAbstractItemModel::rowsInserted, this, &QModelMapper::onModelRowsInserted);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &QModelMapper::onModelRowsRemoved);
        connect(model, &QAbstractItemModel::columnsInserted, this, &QModelMapper::onModelColumnsInserted);
        connect(model, &QAbstractItemModel::columnsRemoved, this, &QModelMapper::onModelColumnsRemoved);
    }

    updateRoles();
    invalidateModelIndex();
}

void QModelMapper::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if(m_submiting)
        return;

    if (m_parentIndex != topLeft.parent() ||
        m_parentIndex != bottomRight.parent())
    {
        return;
    }

    if (m_modelIndex.row() >= topLeft.row() &&
        m_modelIndex.column() >= topLeft.column() &&
        m_modelIndex.row() <= bottomRight.row() &&
        m_modelIndex.column() <= bottomRight.column())
    {
        invalidateModelIndex();
    }
}

void QModelMapper::onModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_modelIndex.row() >= first)
    {
        invalidateModelIndex();
    }
}

void QModelMapper::onModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_modelIndex.row() >= first)
    {
        invalidateModelIndex();
    }
}

void QModelMapper::onModelColumnsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_modelIndex.column() >= first)
    {
        invalidateModelIndex();
    }
}

void QModelMapper::onModelColumnsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_modelIndex.column() >= first)
    {
        invalidateModelIndex();
    }
}

void QModelMapper::invalidateModelIndex()
{
    m_modelIndex = QModelIndex();
    queueSelect();
}

void QModelMapper::updateModelIndex()
{
    if (m_selectPolicy==QVariantMapperPolicies::Disabled) {
        QUTILSLOG_WARNING()<<this<<"Select policy is disabled for"<<m_baseName;
        return;
    }

    QModelIndex modelIndex;
    if(!m_model)
    {
        // Is there so that modelIndex stays invalid
    }
    else if(m_role>=0)
    {
        const QModelIndexList indexes = m_model->match(m_model->index(0, 0, m_parentIndex),
                                                       m_role,
                                                       m_value,
                                                       1,
                                                       m_flags);
        if(!indexes.isEmpty())
            modelIndex = indexes.first();
    }
    else if(m_row>=0)
    {
        modelIndex = m_model->index(m_row, m_column, m_parentIndex);
    }

    if(m_modelIndex==modelIndex)
        return;
    m_modelIndex = modelIndex;
    emit this->modelIndexChanged(m_modelIndex);
}

void QModelMapper::updateRoles()
{
    if(!m_model)
        return;

    int role = m_model->roleNames().key(m_roleName.toUtf8(), -1);
    setRole(role);
}

QVariantMap QModelMapper::selectModel(bool* result)
{
    if(!m_modelIndex.isValid())
        updateModelIndex();

    if(!m_model)
    {
        if(result)
            *result=false;
        return QVariantMap();
    }

    return fromModel(result);
}

QVariantMap QModelMapper::fromModel(bool* result)
{
    if(!m_model)
    {
        QUTILSLOG_WARNING()<<"No model given";
        if(result)
            *result=false;
        return QVariantMap();
    }

    if(!m_modelIndex.isValid())
    {
        QUTILSLOG_WARNING()<<"Invalid model index";
        if(result)
            *result=false;
        return QVariantMap();
    }

    auto roleNames = m_model->roleNames();

    QVariantMap map;
    for(auto [key, value]: roleNames.asKeyValueRange())
    {
        if(getMappedKeys().isEmpty() || getMappedKeys().contains(value))
            map.insert(value, m_model->data(m_modelIndex, key));
    }

    if(result)
        *result=true;

    return map;
}

bool QModelMapper::submitModel(const QStringList& dirtyKeys)
{
    if(!m_model)
        return false;

    QVariantMap dirtyMap;
    for(const QString& dirtyKey: dirtyKeys)
        dirtyMap.insert(dirtyKey, getStorage().value(dirtyKey));
    return toModel(dirtyMap);
}

bool QModelMapper::toModel(const QVariantMap& map) const
{
    if(!m_model)
    {
        QUTILSLOG_WARNING()<<"No model given";
        return false;
    }

    if(!m_modelIndex.isValid())
    {
        QUTILSLOG_WARNING()<<"Invalid model index";
        return false;
    }

    auto roleNames = m_model->roleNames();

    for(auto [key, value]: map.asKeyValueRange())
    {
        const int role = roleNames.key(key.toUtf8(), -1);
        if (role<0)
            continue;

        m_model->setData(m_modelIndex, value, role);
    }

    return true;
}
