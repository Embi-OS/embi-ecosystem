#include "qmodelmatcher.h"

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
        m_indexes = m_sourceModel->match(m_sourceModel->index(m_startRow, m_startColumn, m_parentIndex),
                                         m_role,
                                         m_value,
                                         m_hits,
                                         m_flags);
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
    if(!m_sourceModel)
        return;

    int role = m_sourceModel->roleNames().key(m_roleName.toUtf8(), -1);
    setRole(role);
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

    if(!roles.isEmpty() && !roles.contains(m_role))
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
