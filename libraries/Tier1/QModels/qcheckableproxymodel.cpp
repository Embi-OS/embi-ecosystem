#include "qcheckableproxymodel.h"

QCheckableProxyModel::QCheckableProxyModel(QObject *parent) :
    QIdentityProxyModel(parent),
    m_selection(new QItemSelectionModel(this))
{
    connect(m_selection, &QItemSelectionModel::selectionChanged, this, &QCheckableProxyModel::onSelectionChanged);
}

QVariant QCheckableProxyModel::data(const QModelIndex &index, int role) const
{
    if(!sourceModel())
        return QVariant();

    if (role == Qt::CheckStateRole)
    {
        QVariant ret = (m_inverted ^ m_selection->selection().contains(mapToSource(index))) ? Qt::Checked : Qt::Unchecked;
        return ret;
    }

    return QIdentityProxyModel::data(index, role);
}

bool QCheckableProxyModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!sourceModel())
        return false;

    if (role == Qt::CheckStateRole)
    {
        Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
        const QModelIndex srcIndex = mapToSource(index);
        m_selection->select(QItemSelection(srcIndex, srcIndex), (m_inverted ^ (state==Qt::Unchecked)) ? QItemSelectionModel::Deselect : QItemSelectionModel::Select);
        emit this->dataChanged(index, index, {Qt::CheckStateRole});
        return true;
    }

    return QIdentityProxyModel::setData(index, value, role);
}

Qt::ItemFlags QCheckableProxyModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QIdentityProxyModel::flags(index);
    }

    return QIdentityProxyModel::flags(index) | Qt::ItemIsUserCheckable;
}

QHash<int, QByteArray> QCheckableProxyModel::roleNames() const
{
    return m_roleNames;
}

QModelIndexList QCheckableProxyModel::match(const QModelIndex& start, int role, const QVariant& value, int hits, Qt::MatchFlags flags) const
{
    return QAbstractProxyModel::match(start, role, value, hits, flags);
}

void QCheckableProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (sourceModel && (sourceModel->roleNames().isEmpty())) { // workaround for when a model has no roles and roles are added when the model is populated
        // QTBUG-57971
        connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &QCheckableProxyModel::initRoles);
        connect(sourceModel, &QAbstractItemModel::modelReset, this, &QCheckableProxyModel::initRoles);
    }

    QIdentityProxyModel::setSourceModel(sourceModel);
    m_selection->setModel(sourceModel);

    if(!m_selectedRows.isEmpty())
        setSelectedRows(m_selectedRows);
    if(!m_selectedColumns.isEmpty())
        setSelectedColumns(m_selectedColumns);
}

void QCheckableProxyModel::resetInternalData()
{
    QIdentityProxyModel::resetInternalData();
    updateRoleNames();
}

int QCheckableProxyModel::selectionCount() const
{
    return m_selection->selectedIndexes().count();
}

bool QCheckableProxyModel::hasSelection() const
{
    return m_selection->hasSelection();
}

QModelIndexList QCheckableProxyModel::selectedIndexes() const
{
    return m_selection->selectedIndexes();
}

QList<int> QCheckableProxyModel::selectedRows() const
{
    const QModelIndexList indexes = m_selection->selectedIndexes();
    QList<int> ret;
    ret.reserve(indexes.size());
    for(const QModelIndex& index: indexes)
    {
        if(!ret.contains(index.row()))
            ret.append(index.row());
    }

    std::stable_sort(ret.begin(), ret.end());

    return ret;
}

void QCheckableProxyModel::setSelectedRows(QList<int> rows)
{
    if(!sourceModel())
    {
        m_selectedRows = rows;
        return;
    }

    std::stable_sort(rows.begin(), rows.end());
    if(rows==selectedRows())
        return;

    m_isUpdatingFromProperty=true;

    m_selection->clearSelection();
    for(int row: rows)
        selectRow(row);

    m_isUpdatingFromProperty=false;
    emit this->selectionChanged();
}

QList<int> QCheckableProxyModel::selectedColumns() const
{
    const QModelIndexList indexes = m_selection->selectedIndexes();
    QList<int> ret;
    ret.reserve(indexes.size());
    for(const QModelIndex& index: indexes)
    {
        if(!ret.contains(index.column()))
            ret.append(index.column());
    }

    std::stable_sort(ret.begin(), ret.end());

    return ret;
}

void QCheckableProxyModel::setSelectedColumns(QList<int> columns)
{
    if(!sourceModel())
    {
        m_selectedColumns = columns;
        return;
    }

    std::stable_sort(columns.begin(), columns.end());
    if(columns==selectedColumns())
        return;

    m_isUpdatingFromProperty=true;

    m_selection->clearSelection();
    for(int column: columns)
        selectColumn(column);

    m_isUpdatingFromProperty=false;
    emit this->selectionChanged();
}

bool QCheckableProxyModel::isChecked(int index) const
{
    return m_selection->isRowSelected(index);
}

void QCheckableProxyModel::clear()
{
    m_selection->clear();
}

void QCheckableProxyModel::selectAll()
{
    m_isUpdatingFromProperty=true;

    for(int i=0; i<rowCount(); i++)
        m_selection->select(this->index(i,0), QItemSelectionModel::Select | QItemSelectionModel::Rows);

    m_isUpdatingFromProperty=false;
    emit this->selectionChanged();
}

void QCheckableProxyModel::set(const QModelIndex &index, bool select)
{
    m_selection->select(index, select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
}

void QCheckableProxyModel::setRow(int row, bool select)
{
    m_selection->select(this->index(row,0), (select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect) | QItemSelectionModel::Rows);
}

void QCheckableProxyModel::setColumn(int column, bool select)
{
    m_selection->select(this->index(0,column), (select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect) | QItemSelectionModel::Columns);
}

void QCheckableProxyModel::select(const QModelIndex &index)
{
    m_selection->select(index, QItemSelectionModel::Select);
}

void QCheckableProxyModel::selectRow(int row)
{
    m_selection->select(this->index(row,0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void QCheckableProxyModel::selectColumn(int column)
{
    m_selection->select(this->index(0,column), QItemSelectionModel::Select | QItemSelectionModel::Columns);
}

void QCheckableProxyModel::deselect(const QModelIndex &index)
{
    m_selection->select(index, QItemSelectionModel::Deselect);
}

void QCheckableProxyModel::deselectRow(int row)
{
    m_selection->select(this->index(row,0), QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}

void QCheckableProxyModel::deselectColumn(int column)
{
    m_selection->select(this->index(0,column), QItemSelectionModel::Deselect | QItemSelectionModel::Columns);
}

void QCheckableProxyModel::toggle(const QModelIndex &index)
{
    m_selection->select(index, QItemSelectionModel::Toggle);
}

void QCheckableProxyModel::toggleRow(int row)
{
    m_selection->select(this->index(row,0), QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
}

void QCheckableProxyModel::toggleColumn(int column)
{
    m_selection->select(this->index(0,column), QItemSelectionModel::Toggle | QItemSelectionModel::Columns);
}

void QCheckableProxyModel::countInvalidate()
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

void QCheckableProxyModel::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    const auto lstSelected = mapSelectionFromSource(selected);
    for (const QItemSelectionRange &range : lstSelected)
    {
        emit this->dataChanged(range.topLeft(), range.bottomRight(), {Qt::CheckStateRole});
    }

    const auto lstDeselected = mapSelectionFromSource(deselected);
    for (const QItemSelectionRange &range : lstDeselected)
    {
        emit this->dataChanged(range.topLeft(), range.bottomRight(), {Qt::CheckStateRole});
    }

    if(!m_isUpdatingFromProperty)
        emit this->selectionChanged();
}

void QCheckableProxyModel::updateRoleNames()
{
    if(sourceModel() && !sourceModel()->roleNames().isEmpty())
    {
        QHash<int, QByteArray> roles = sourceModel()->roleNames();
        roles[Qt::CheckStateRole] = QByteArrayLiteral("checkState");
        m_roleNames = roles;
    }
}

void QCheckableProxyModel::initRoles()
{
    if(sourceModel() && !sourceModel()->roleNames().isEmpty())
    {
        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &QCheckableProxyModel::initRoles);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &QCheckableProxyModel::initRoles);
        resetInternalData();
    }
}

