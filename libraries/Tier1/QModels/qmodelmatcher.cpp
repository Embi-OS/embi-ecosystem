#include "qmodelmatcher.h"
#include "qmodels_log.h"

QModelMatcher::QModelMatcher(QObject* parent):
    QObject(parent)
{
    connect(this, &QModelMatcher::enabledChanged, this, &QModelMatcher::queueInvalidate);
    connect(this, &QModelMatcher::startRowChanged, this, &QModelMatcher::queueInvalidate);
    connect(this, &QModelMatcher::startColumnChanged, this, &QModelMatcher::queueInvalidate);
    connect(this, &QModelMatcher::roleChanged, this, &QModelMatcher::queueInvalidate);
    connect(this, &QModelMatcher::valueChanged, this, &QModelMatcher::queueInvalidate);
    connect(this, &QModelMatcher::hitsChanged, this, &QModelMatcher::queueInvalidate);
    connect(this, &QModelMatcher::flagsChanged, this, &QModelMatcher::queueInvalidate);
    connect(this, &QModelMatcher::roleNameChanged, this, &QModelMatcher::updateRoles);

    connect(this, &QModelMatcher::sourceModelAboutToChange, this, &QModelMatcher::onModelAboutToChange);
    connect(this, &QModelMatcher::sourceModelChanged, this, &QModelMatcher::onModelChanged);

    QMetaObject::invokeMethod(this, &QModelMatcher::countInvalidate, Qt::QueuedConnection);
}

void QModelMatcher::classBegin()
{

}

void QModelMatcher::componentComplete()
{
    updateMethod();
    invalidate();
}

void QModelMatcher::countInvalidate()
{
    const int aCount = count();
    bool emptyChanged=false;

    if(m_count==aCount)
        return;

    if((m_count==0 && aCount!=0) || (m_count!=0 && aCount==0))
        emptyChanged=true;

    m_count=aCount;
    emit this->countChanged(count());

    if(emptyChanged)
        emit this->emptyChanged(isEmpty());
}

void QModelMatcher::queueInvalidate()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &QModelMatcher::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void QModelMatcher::invalidate()
{
    m_completed = true;

    m_invalidateQueued = false;

    if(!m_enabled)
        return;

    emit aboutToBeInvalidated();

    m_indexes.clear();
    m_indexes.reserve(m_hits);

    if(m_sourceModel && m_sourceModel->rowCount()>0)
    {
        if(m_method.isValid() && !m_methodParameterRoles.isEmpty())
        {
            m_indexes = methodMatch(m_sourceModel->index(m_startRow, m_startColumn, m_parentIndex),
                                    m_hits,
                                    m_flags);
        }
        else if(m_role>=0)
        {
            m_indexes = roleMatch(m_sourceModel->index(m_startRow, m_startColumn, m_parentIndex),
                                  m_hits,
                                  m_flags);
        }
    }

    emit indexesChanged(m_indexes);
    countInvalidate();

    emit invalidated();
}

const QModelIndexList& QModelMatcher::getIndexes() const
{
    return m_indexes;
}

void QModelMatcher::updateRoles()
{
    if(!m_sourceModel || m_sourceModel->roleNames().isEmpty())
        return;

    int role = m_sourceModel->roleNames().key(m_roleName.toUtf8(), -1);
    setRole(role);
    if(m_role<0 && !m_roleName.isEmpty())
    {
        QMODELSLOG_WARNING()<<m_roleName<<"is not a valid roleName for"<<m_sourceModel;
    }
    else if(m_role>=0)
    {
        return;
    }

    m_methodParameterRoles.clear();
    for(const QByteArray& parameterName: std::as_const(m_methodParameterNames))
    {
        int role = m_sourceModel->roleNames().key(parameterName, -1);
        if(role>=0)
            m_methodParameterRoles << role;
        else {
            QMODELSLOG_WARNING()<<parameterName<<"is not a valid roleName for"<<m_sourceModel;
        }
    }
}

void QModelMatcher::initRoles()
{
    if(m_sourceModel && !m_sourceModel->roleNames().isEmpty())
    {
        disconnect(m_sourceModel, &QAbstractItemModel::rowsInserted, this, &QModelMatcher::initRoles);
        disconnect(m_sourceModel, &QAbstractItemModel::modelReset, this, &QModelMatcher::initRoles);
        updateRoles();
    }
}

void QModelMatcher::onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel)
{
    if(oldModel!=nullptr)
    {
        disconnect(oldModel, nullptr,this, nullptr);
    }
}

void QModelMatcher::onModelChanged(QAbstractItemModel* model)
{
    if (model != nullptr)
    {
        connect(model, &QAbstractItemModel::modelReset, this, &QModelMatcher::queueInvalidate);
        connect(model, &QAbstractItemModel::layoutChanged, this, &QModelMatcher::queueInvalidate);
        connect(model, &QAbstractItemModel::dataChanged, this, &QModelMatcher::onModelDataChanged);
        connect(model, &QAbstractItemModel::rowsInserted, this, &QModelMatcher::onModelRowsInserted);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &QModelMatcher::onModelRowsRemoved);
        connect(model, &QAbstractItemModel::columnsInserted, this, &QModelMatcher::onModelColumnsInserted);
        connect(model, &QAbstractItemModel::columnsRemoved, this, &QModelMatcher::onModelColumnsRemoved);

        if (model->roleNames().isEmpty()) { // workaround for when a model has no roles and roles are added when the model is populated
            // QTBUG-57971
            connect(model, &QAbstractItemModel::rowsInserted, this, &QModelMatcher::initRoles);
            connect(model, &QAbstractItemModel::modelReset, this, &QModelMatcher::initRoles);
        }
    }

    updateRoles();
    queueInvalidate();
}

void QModelMatcher::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    if(!m_enabled || !m_completed)
    {
        return;
    }

    if(!roles.isEmpty() && m_method.isValid() && !m_methodParameterRoles.isEmpty())
    {
        bool roleMatch=false;
        for(int methodParameterRole: std::as_const(m_methodParameterRoles))
        {
            if(roles.contains(methodParameterRole)) {
                roleMatch = true;
                break;
            }
        }
        if(!roleMatch)
            return;
    }
    else if(m_role>=0 && !roles.isEmpty() && !roles.contains(m_role))
    {
        return;
    }

    if (m_parentIndex != topLeft.parent() ||
        m_parentIndex != bottomRight.parent())
    {
        return;
    }

    if (m_startRow <= bottomRight.row() &&
        m_startColumn <= bottomRight.column())
    {
        queueInvalidate();
    }
}

void QModelMatcher::onModelRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_startRow <= first)
    {
        queueInvalidate();
    }
}

void QModelMatcher::onModelRowsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_startRow <= first)
    {
        queueInvalidate();
    }
}

void QModelMatcher::onModelColumnsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_startColumn <= first)
    {
        queueInvalidate();
    }
}

void QModelMatcher::onModelColumnsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parentIndex &&
        m_startColumn <= first)
    {
        queueInvalidate();
    }
}

bool QModelMatcher::methodMatch(const QModelIndex &idx) const
{
    bool ret;
    std::vector<void*> argv;
    argv.push_back(&ret);

    QVariantList params;
    for(int i=0; i<m_methodParameterRoles.size(); i++)
    {
        const int parameterRole = m_methodParameterRoles.at(i);
        const QMetaType parameterType = m_methodParameterTypes.at(i);
        QVariant param = m_sourceModel->data(idx, parameterRole);
        if(parameterType.id()!=QMetaType::QVariant && !param.canConvert(parameterType)) {
            QMODELSLOG_WARNING()<<"Unable to convert param type"<<param.metaType()<<"to"<<parameterType;
            return false;
        }
        else if(parameterType.id()!=QMetaType::QVariant) {
            param.convert(parameterType);
        }

        params.append(param);
    }

    for(QVariant& param: params)
        argv.push_back(param.data());

    QMetaObject::metacall(const_cast<QModelMatcher*>(this), QMetaObject::InvokeMetaMethod, m_method.methodIndex(), argv.data());

    return ret;
}

QModelIndexList QModelMatcher::methodMatch(const QModelIndex &start, int hits, Qt::MatchFlags flags) const
{
    QModelIndexList result;
    bool recurse = flags.testAnyFlag(Qt::MatchRecursive);
    bool wrap = flags.testAnyFlag(Qt::MatchWrap);
    bool allHits = (hits == -1);
    const int column = start.column();
    QModelIndex p = m_sourceModel->parent(start);
    int from = start.row();
    int to = m_sourceModel->rowCount(p);
    // iterates twice if wrapping
    for (int i = 0; (wrap && i < 2) || (!wrap && i < 1); ++i) {
        for (int r = from; (r < to) && (allHits || result.size() < hits); ++r) {
            QModelIndex idx = m_sourceModel->index(r, column, p);
            if (!idx.isValid())
                continue;
            // QMetaMethod based matching
            if (methodMatch(idx))
                result.append(idx);
            if (recurse) {
                const auto parent = column != 0 ? idx.sibling(idx.row(), 0) : idx;
                if (m_sourceModel->hasChildren(parent)) { // search the hierarchy
                    result += methodMatch(m_sourceModel->index(0, column, parent),
                                          (allHits ? -1 : hits - result.size()), flags);
                }
            }
        }
        // prepare for the next iteration
        from = 0;
        to = start.row();
    }
    return result;
}

QModelIndexList QModelMatcher::roleMatch(const QModelIndex &start, int hits, Qt::MatchFlags flags) const
{
    return m_sourceModel->match(start,
                                m_role,
                                m_value,
                                hits,
                                flags);
}

void QModelMatcher::updateMethod()
{
    const auto *metaObj = metaObject();
    for (int idx = metaObj->methodOffset(); idx < metaObj->methodCount(); idx++) {
        // Once we find the method signature, break the loop
        QMetaMethod method = metaObj->method(idx);
        if (method.name() == "match") {
            m_method = method;
            break;
        }
    }

    if (!m_method.isValid()) {
        return;
    }

    if (m_method.parameterCount() < 1) {
        QMODELSLOG_WARNING()<<"ModelMatcher match method requires at least one parameter";
        return;
    }

    if (m_method.returnMetaType().id()!=QMetaType::Bool) {
        QMODELSLOG_WARNING()<<"ModelMatcher match method return must be boolean";
        return;
    }

    m_methodParameterNames = m_method.parameterNames();
    m_methodParameterTypes.clear();
    for(int i=0; i<m_method.parameterCount(); i++)
        m_methodParameterTypes << m_method.parameterMetaType(i);

    updateRoles();
}
