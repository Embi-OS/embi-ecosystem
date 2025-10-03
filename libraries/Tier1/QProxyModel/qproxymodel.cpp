#include "qproxymodel.h"
#include "qproxymodel_log.h"

QProxyModel::QProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    connect(this, &QAbstractProxyModel::sourceModelChanged, this, &QProxyModel::updateRoles);
    connect(this, &QAbstractItemModel::modelReset, this, &QProxyModel::updateRoles);
    connect(this, &QAbstractItemModel::rowsInserted, this, &QProxyModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QProxyModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &QProxyModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QProxyModel::countInvalidate);
    connect(this, &QAbstractItemModel::dataChanged, this, &QProxyModel::onDataChanged);

    connect(this, &QProxyModel::enabledChanged, this, &QProxyModel::queueInvalidate);

    connect(this, &QProxyModel::filterEnabledChanged, this, &QProxyModel::queueInvalidateFilter);
    connect(this, &QProxyModel::filterColumnChanged, this, &QProxyModel::queueInvalidateFilter);
    connect(this, &QProxyModel::filterRoleNameChanged, this, &QProxyModel::updateFilterRole);
    connect(this, &QProxyModel::filterValueChanged, this, &QProxyModel::queueInvalidateFilter);
    connect(this, &QProxyModel::filterInvertedChanged, this, &QProxyModel::queueInvalidate);
    connect(this, &QProxyModel::filterPatternChanged, this, &QProxyModel::updateFilterPattern);

    connect(this, &QProxyModel::sortEnabledChanged, this, &QProxyModel::queueInvalidateSorter);
    connect(this, &QProxyModel::sortColumnChanged, this, &QProxyModel::queueInvalidateSorter);
    connect(this, &QProxyModel::sortRoleNameChanged, this, &QProxyModel::updateSortRole);
    connect(this, &QProxyModel::sortOrderChanged, this, &QProxyModel::queueInvalidate);

    connect(this, &QProxyModel::invalidateWhenChanged, this, &QProxyModel::queueInvalidate);
    connect(this, &QProxyModel::invalidateFilterWhenChanged, this, &QProxyModel::queueInvalidateFilter);
    connect(this, &QProxyModel::invalidateSorterWhenChanged, this, &QProxyModel::queueInvalidateSorter);

    setDynamicSortFilter(true);
    setSortRole(-1);
    setSortCaseSensitivity(Qt::CaseInsensitive);
}

void QProxyModel::classBegin()
{

}

void QProxyModel::componentComplete()
{
    invalidate();
}

QVariant QProxyModel::data(const QModelIndex &index, int role) const
{
    if(!sourceModel())
        return QVariant();
    return sourceData(mapToSource(index), role);
}

bool QProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!sourceModel())
        return false;
    return setSourceData(mapToSource(index), value, role);
}

QHash<int, QByteArray> QProxyModel::roleNames() const
{
    return m_roleNames.isEmpty() && sourceModel() ? sourceModel()->roleNames() : m_roleNames;
}

QVariantMap QProxyModel::sourceDatas(const QModelIndex& sourceIndex, const QStringList& roles) const
{
    QHash<int,QByteArray> names = roleNames();
    if(!roles.isEmpty())
    {
        QHash<int,QByteArray> tmpNames=roleNames();
        names.clear();
        for(const QString& role: roles)
        {
            const QByteArray roleName = role.toUtf8();
            names[tmpNames.key(roleName)]=roleName;
        }
    }

    QVariantMap map;
    for(auto [key, value]: names.asKeyValueRange())
        map.insert(value, data(sourceIndex, key));

    return map;
}

QVariant QProxyModel::sourceData(const QModelIndex& sourceIndex, const QString& roleName) const
{
    int role = roleForName(roleName.toUtf8());
    return sourceData(sourceIndex, role);
}

QVariant QProxyModel::sourceData(const QModelIndex &sourceIndex, int role) const
{
    if(!sourceModel())
        return QVariant();
    return sourceModel()->data(sourceIndex, role);
}

bool QProxyModel::setSourceData(const QModelIndex& sourceIndex, const QVariant &value, const QString& roleName) const
{
    int role = roleForName(roleName.toUtf8());
    return setSourceData(sourceIndex, value, role);
}

bool QProxyModel::setSourceData(const QModelIndex& sourceIndex, const QVariant &value, int role) const
{
    if(!sourceModel())
        return false;
    return sourceModel()->setData(sourceIndex, value, role);
}

int QProxyModel::roleForName(const QString& roleName) const
{
    int role = m_roleIds.value(roleName, -1);

    if(role<0 && !roleName.isEmpty() && !m_roleIds.isEmpty()) {
        QPROXYMODELLOG_CRITICAL()<<"QProxyModel"<<this<<"does not contains role"<<roleName<<"for"<<sourceModel();
    }

    return role;
}

QByteArray QProxyModel::roleName(int role) const
{
    return m_roleNames.value(role, "");
}

QModelIndexList QProxyModel::sourceIndexes() const
{
    QModelIndexList indexes;
    indexes.reserve(rowCount());

    for(int i=0; i<rowCount(); i++)
    {
        indexes.append(mapToSource(index(i,0)));
    }

    return indexes;
}

QList<int> QProxyModel::sourceRows() const
{
    QList<int> rows;
    rows.reserve(rowCount());

    for(int i=0; i<rowCount(); i++)
    {
        rows.append(mapToSource(i));
    }

    return rows;
}

QModelIndex QProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return QSortFilterProxyModel::mapToSource(proxyIndex);
}

QModelIndex QProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

int QProxyModel::mapToSource(int proxyRow) const
{
    const QModelIndex proxyIndex = index(proxyRow, 0);
    const QModelIndex sourceIndex = mapToSource(proxyIndex);
    return sourceIndex.isValid() ? sourceIndex.row() : -1;
}

int QProxyModel::mapFromSource(int sourceRow) const
{
    QModelIndex proxyIndex;
    if (QAbstractItemModel* source = sourceModel()) {
        const QModelIndex sourceIndex = source->index(sourceRow, 0);
        proxyIndex = mapFromSource(sourceIndex);
    }
    return proxyIndex.isValid() ? proxyIndex.row() : -1;
}

QList<int> QProxyModel::mapToSource(const QList<int>& proxyRows) const
{
    QList<int> sourceRows;
    sourceRows.reserve(proxyRows.size());
    for(int proxyRow: proxyRows)
        sourceRows.append(mapToSource(proxyRow));
    return sourceRows;
}

QList<int> QProxyModel::mapFromSource(const QList<int>& sourceRows) const
{
    QList<int> proxyRows;
    proxyRows.reserve(sourceRows.size());
    for(int sourceRow: sourceRows)
        proxyRows.append(mapToSource(sourceRow));
    return sourceRows;
}

QItemSelection QProxyModel::mapSelectionToSource(const QItemSelection &proxySelection) const
{
    return QSortFilterProxyModel::mapSelectionToSource(proxySelection);
}

QItemSelection QProxyModel::mapSelectionFromSource(const QItemSelection &sourceSelection) const
{
    return QSortFilterProxyModel::mapSelectionFromSource(sourceSelection);
}

bool QProxyModel::isActive() const
{
    return m_isActive;
}

bool QProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (!m_isActive || !m_enabled || !sourceModel())
        return false;

    if(!m_filterEnabled)
        return true;

    QModelIndex sourceIndex = sourceModel()->index(source_row, m_filterColumn, source_parent);
    const QVariant& rowValue = sourceData(sourceIndex, filterRole());
    bool valueAccepted = !m_filterValue.isValid() || (m_filterValue==rowValue);
    bool baseAcceptsRow = valueAccepted && QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);

    return baseAcceptsRow ^ m_filterInverted;
}

bool QProxyModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    if (m_isActive && m_enabled && m_sortEnabled)
    {
        int comparison = compare(source_left, source_right);
        if (comparison != 0)
            return comparison < 0;
    }
    return source_left.row() < source_right.row();
}

int QProxyModel::compare(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    int role = sortRole();

    if (role < 0 || m_sortColumn < 0)
        return 0;

    const QVariant& leftValue = sourceData(source_left, role);
    const QVariant& rightValue = sourceData(source_right, role);

    int comparison = QQsfpm::compare(leftValue, rightValue, sortCaseSensitivity(), isSortLocaleAware());

    return (m_sortOrder == Qt::AscendingOrder) ? comparison : -comparison;
}

void QProxyModel::setSourceModel(QAbstractItemModel *model)
{
    if (model && (model->roleNames().isEmpty())) { // workaround for when a model has no roles and roles are added when the model is populated
        // QTBUG-57971
        connect(model, &QAbstractItemModel::rowsInserted, this, &QProxyModel::initRoles);
        connect(model, &QAbstractItemModel::modelReset, this, &QProxyModel::initRoles);
    }
    QSortFilterProxyModel::setSourceModel(model);
}

void QProxyModel::resetInternalData()
{
    QSortFilterProxyModel::resetInternalData();
    updateRoleNames();
}

void QProxyModel::countInvalidate()
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

void QProxyModel::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    Q_UNUSED(roles)
}

void QProxyModel::queueInvalidate()
{
    if(!isActive())
        return;

    if(!m_invalidateWhen)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &QProxyModel::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void QProxyModel::invalidate()
{
    m_isActive = true;
    m_invalidateQueued = false;

    QSortFilterProxyModel::invalidate();
    QProxyModel::invalidateSorter();
}

void QProxyModel::queueInvalidateFilter()
{
    if(!m_invalidateFilterWhen || !m_invalidateWhen)
        return;

    if (m_delayed) {
        if (!m_invalidateFilterQueued && !m_invalidateQueued) {
            m_invalidateFilterQueued = true;
            QMetaObject::invokeMethod(this, &QProxyModel::invalidateFilter, Qt::QueuedConnection);
        }
    } else {
        invalidateFilter();
    }
}

void QProxyModel::invalidateFilter()
{
    m_invalidateFilterQueued = false;
    if (m_isActive && !m_invalidateQueued)
        QSortFilterProxyModel::invalidateFilter();
}

void QProxyModel::queueInvalidateSorter()
{
    if(!m_invalidateSorterWhen || !m_invalidateWhen)
        return;

    if (m_delayed) {
        if (!m_invalidateSorterQueued && !m_invalidateQueued) {
            m_invalidateSorterQueued = true;
            QMetaObject::invokeMethod(this, &QProxyModel::invalidateSorter, Qt::QueuedConnection);
        }
    } else {
        invalidateSorter();
    }
}

void QProxyModel::invalidateSorter()
{
    m_invalidateSorterQueued = false;
    if (m_isActive && !m_invalidateQueued)
        QSortFilterProxyModel::sort(m_sortColumn);
}

void QProxyModel::updateFilterPattern()
{
    QRegularExpression regExp = filterRegularExpression();
    regExp.setPattern(m_filterPattern);
    QSortFilterProxyModel::setFilterRegularExpression(regExp);
}

void QProxyModel::updateRoleNames()
{
    if (!sourceModel())
        return;
    m_roleNames = sourceModel()->roleNames();
    reverseStringIntHash(m_roleIds, m_roleNames);

    emit this->roleNamesChanged();
}

void QProxyModel::updateFilterRole()
{
    int role = roleForName(m_filterRoleName.toUtf8());
    setFilterRole(role);
}

void QProxyModel::updateSortRole()
{
    int role = roleForName(m_sortRoleName.toUtf8());
    setSortRole(role);
}

void QProxyModel::updateRoles()
{
    updateFilterRole();
    updateSortRole();
}

void QProxyModel::initRoles()
{
    if(sourceModel() && !sourceModel()->roleNames().isEmpty())
    {
        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &QProxyModel::initRoles);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &QProxyModel::initRoles);
        resetInternalData();
        updateRoles();
    }
}
