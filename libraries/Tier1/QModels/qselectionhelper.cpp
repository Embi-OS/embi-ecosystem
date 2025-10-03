#include "qselectionhelper.h"
#include "qmodels_log.h"

QSelectionModel::QSelectionModel(QAbstractItemModel* parent):
    QItemSelectionModel(parent)
{
    connect(this, &QSelectionModel::currentIndexEnabledChanged, this, &QSelectionModel::clearCurrentIndex);
}

void QSelectionModel::setCurrentIndex(const QModelIndex &index, QItemSelectionModel::SelectionFlags command)
{
    if(!m_currentIndexEnabled)
    {
        clearCurrentIndex();
        return;
    }

    if((index==currentIndex() && m_currentIndexPolicy.testFlag(CurrentIndexPolicyToggleCell)) ||
       (index.row()==currentIndex().row() && m_currentIndexPolicy.testFlag(CurrentIndexPolicyToggleRow)) ||
       (index.column()==currentIndex().column() && m_currentIndexPolicy.testFlag(CurrentIndexPolicyToggleColumn)))
    {
        clearCurrentIndex();
        return;
    }

    QItemSelectionModel::setCurrentIndex(index, command);
}
void QSelectionModel::select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command)
{
    if(m_currentIndexPolicy.testFlag(CurrentIndexPolicyClear) && !command.testFlag(QItemSelectionModel::Current))
        clearCurrentIndex();

    QItemSelectionModel::select(index, command);
}
void QSelectionModel::select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command)
{
    if(m_currentIndexPolicy.testFlag(CurrentIndexPolicyClear) && !command.testFlag(QItemSelectionModel::Current))
        clearCurrentIndex();

    QItemSelectionModel::select(selection, command);
}
void QSelectionModel::clear()
{
    QItemSelectionModel::clear();
}
void QSelectionModel::reset()
{
    QItemSelectionModel::reset();
}

void QSelectionModel::clearCurrentIndex()
{
    QItemSelectionModel::clearCurrentIndex();
}

QSelectionHelper::QSelectionHelper(QItemSelectionModel* object) :
    QObject(object),
    m_selection(object)
{
    connect(this, &QSelectionHelper::selectionChanged, this, &QSelectionHelper::countInvalidate);

    connect(m_selection, &QItemSelectionModel::selectionChanged, this, &QSelectionHelper::onSelectionChanged);
}

QSelectionHelper* QSelectionHelper::wrap(const QObject* object)
{
    return qobject_cast<QSelectionHelper*>(qmlAttachedPropertiesObject<QSelectionHelper>(object, true));
}

QSelectionHelper* QSelectionHelper::qmlAttachedProperties(QObject* object)
{
    QItemSelectionModel* selection = qobject_cast<QItemSelectionModel*>(object);
    if (!selection)
    {
        QMODELSLOG_CRITICAL()<<object<<selection;
        QMODELSLOG_FATAL("QModelHelper must be attached to a QAbstractItemModel*");
        return nullptr;
    }

    QSelectionHelper* helper = new QSelectionHelper(selection);
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;
}

void QSelectionHelper::countInvalidate()
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

void QSelectionHelper::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if(!m_isUpdatingFromProperty)
        emit this->selectionChanged();
}

// ──────── PUBLIC API ──────────
QModelIndex QSelectionHelper::index(int row, int column, const QModelIndex &parent)
{
    return index(m_selection, row, column, parent);
}

bool QSelectionHelper::hasSelection() const
{
    return m_selection->hasSelection();
}

QModelIndexList QSelectionHelper::selectedIndexes() const
{
    return m_selection->selectedIndexes();
}

QList<int> QSelectionHelper::selectedRows(int column) const
{
    return selectedRows(m_selection, column);
}

void QSelectionHelper::setSelectedRows(QList<int> rows)
{
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

QList<int> QSelectionHelper::selectedColumns(int row) const
{
    return selectedColumns(m_selection, row);
}

void QSelectionHelper::setSelectedColumns(QList<int> columns)
{
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

void QSelectionHelper::selectAll()
{
    m_isUpdatingFromProperty=true;

    selectAll(m_selection);

    m_isUpdatingFromProperty=false;
    emit this->selectionChanged();
}

void QSelectionHelper::deselectAll()
{
    m_isUpdatingFromProperty=true;

    deselectAll(m_selection);

    m_isUpdatingFromProperty=false;
    emit this->selectionChanged();
}

void QSelectionHelper::toggleAll()
{
    m_isUpdatingFromProperty=true;

    toggleAll(m_selection);

    m_isUpdatingFromProperty=false;
    emit this->selectionChanged();
}

void QSelectionHelper::set(const QModelIndex &index, bool select)
{
    return set(m_selection, index, select);
}
void QSelectionHelper::setRow(int row, bool select)
{
    return setRow(m_selection, row, select);
}
void QSelectionHelper::setColumn(int column, bool select)
{
    return setColumn(m_selection, column, select);
}

void QSelectionHelper::select(const QModelIndex &index)
{
    return select(m_selection, index);
}

void QSelectionHelper::selectRow(int row)
{
    return selectRow(m_selection, row);
}

void QSelectionHelper::selectColumn(int column)
{
    return selectColumn(m_selection, column);
}

void QSelectionHelper::deselect(const QModelIndex &index)
{
    return deselect(m_selection, index);
}

void QSelectionHelper::deselectRow(int row)
{
    return deselectRow(m_selection, row);
}

void QSelectionHelper::deselectColumn(int column)
{
    return deselectColumn(m_selection, column);
}

void QSelectionHelper::toggle(const QModelIndex &index)
{
    return toggle(m_selection, index);
}

void QSelectionHelper::toggleRow(int row)
{
    return toggleRow(m_selection, row);
}

void QSelectionHelper::toggleColumn(int column)
{
    return toggleColumn(m_selection, column);
}

void QSelectionHelper::setCurrentIndex(const QModelIndex &index)
{
    return setCurrentIndex(m_selection, index);
}

void QSelectionHelper::setCurrentRow(int row)
{
    return setCurrentRow(m_selection, row);
}

void QSelectionHelper::setCurrentColumn(int column)
{
    return setCurrentColumn(m_selection, column);
}


// ──────── PUBLIC STATIC API ──────────
QModelIndex QSelectionHelper::index(QItemSelectionModel* selection, int row, int column, const QModelIndex &parent)
{
    return selection->model()->index(row, column, parent);
}

QList<int> QSelectionHelper::selectedRows(QItemSelectionModel* selection, int column)
{
    QModelIndexList indexes = selection->selectedRows(column);
    QList<int> ret;
    ret.reserve(indexes.size());
    for(const QModelIndex& index: indexes)
        ret.append(index.row());

    std::stable_sort(ret.begin(), ret.end());

    return ret;
}

QList<int> QSelectionHelper::selectedColumns(QItemSelectionModel* selection, int row)
{
    QModelIndexList indexes = selection->selectedColumns(row);
    QList<int> ret;
    ret.reserve(indexes.size());
    for(const QModelIndex& index: indexes)
        ret.append(index.column());

    std::stable_sort(ret.begin(), ret.end());

    return ret;
}

void QSelectionHelper::selectAll(QItemSelectionModel* selection)
{
    for(int i=0; i<selection->model()->rowCount(); i++)
        selection->select(index(selection, i,0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void QSelectionHelper::deselectAll(QItemSelectionModel* selection)
{
    for(int i=0; i<selection->model()->rowCount(); i++)
        selection->select(index(selection, i,0), QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}

void QSelectionHelper::toggleAll(QItemSelectionModel* selection)
{
    for(int i=0; i<selection->model()->rowCount(); i++)
        selection->select(index(selection, i,0), QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
}

void QSelectionHelper::set(QItemSelectionModel* selection, const QModelIndex &index, bool select)
{
    selection->select(index, select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
}

void QSelectionHelper::setRow(QItemSelectionModel* selection, int row, bool select)
{
    selection->select(index(selection, row,0), (select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect) | QItemSelectionModel::Rows);
}

void QSelectionHelper::setColumn(QItemSelectionModel* selection, int column, bool select)
{
    selection->select(index(selection, 0,column), (select ? QItemSelectionModel::Select : QItemSelectionModel::Deselect) | QItemSelectionModel::Columns);
}

void QSelectionHelper::select(QItemSelectionModel* selection, const QModelIndex &index)
{
    selection->select(index, QItemSelectionModel::Select);
}

void QSelectionHelper::selectRow(QItemSelectionModel* selection, int row)
{
    selection->select(index(selection, row,0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
}

void QSelectionHelper::selectColumn(QItemSelectionModel* selection, int column)
{
    selection->select(index(selection, 0,column), QItemSelectionModel::Select | QItemSelectionModel::Columns);
}

void QSelectionHelper::deselect(QItemSelectionModel* selection, const QModelIndex &index)
{
    selection->select(index, QItemSelectionModel::Deselect);
}

void QSelectionHelper::deselectRow(QItemSelectionModel* selection, int row)
{
    selection->select(index(selection, row,0), QItemSelectionModel::Deselect | QItemSelectionModel::Rows);
}

void QSelectionHelper::deselectColumn(QItemSelectionModel* selection, int column)
{
    selection->select(index(selection, 0,column), QItemSelectionModel::Deselect | QItemSelectionModel::Columns);
}

void QSelectionHelper::toggle(QItemSelectionModel* selection, const QModelIndex &index)
{
    selection->select(index, QItemSelectionModel::Toggle);
}

void QSelectionHelper::toggleRow(QItemSelectionModel* selection, int row)
{
    selection->select(index(selection, row,0), QItemSelectionModel::Toggle | QItemSelectionModel::Rows);
}

void QSelectionHelper::toggleColumn(QItemSelectionModel* selection, int column)
{
    selection->select(index(selection, 0,column), QItemSelectionModel::Toggle | QItemSelectionModel::Columns);
}

void QSelectionHelper::setCurrentIndex(QItemSelectionModel* selection, const QModelIndex &index)
{
    selection->setCurrentIndex(index, QItemSelectionModel::Current);
}

void QSelectionHelper::setCurrentRow(QItemSelectionModel* selection, int row)
{
    selection->setCurrentIndex(index(selection, row,0), QItemSelectionModel::Current | QItemSelectionModel::Rows);
}

void QSelectionHelper::setCurrentColumn(QItemSelectionModel* selection, int column)
{
    selection->setCurrentIndex(index(selection, 0,column), QItemSelectionModel::Current | QItemSelectionModel::Columns);
}

