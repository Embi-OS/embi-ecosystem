#include "tableviewmodel.h"
#include "table_log.h"

TableViewModel::TableViewModel(QObject* parent) :
    QIdentityProxyModel(parent)
{
    connect(this, &TableViewModel::horizontalHeaderDisplayRoleNameChanged, this, &TableViewModel::initHeadersRole);
    connect(this, &TableViewModel::verticalHeaderDisplayRoleNameChanged, this, &TableViewModel::initHeadersRole);
    connect(this, &TableViewModel::columnModelAboutToChange, this, &TableViewModel::onColumnModelAboutToChange);
    connect(this, &TableViewModel::columnModelChanged, this, &TableViewModel::onColumnModelChanged);

    connect(this, &QAbstractItemModel::modelReset, this, &TableViewModel::updateRoles);
    connect(this, &QAbstractItemModel::rowsInserted, this, &TableViewModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &TableViewModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &TableViewModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &TableViewModel::countInvalidate);

    connect(this, &QAbstractItemModel::dataChanged, this, &TableViewModel::onDataChanged);

}

void TableViewModel::classBegin()
{

}

void TableViewModel::componentComplete()
{
    invalidate();
}

void TableViewModel::invalidate()
{
    if(!m_isActive && m_columnModel)
    {
        for (TableViewColumn* column : m_columnModel->modelIterator<TableViewColumn>())
            column->viewModelCompleted(*this);
    }

    if(m_columnModel)
    {
        for (TableViewColumn* column : m_columnModel->modelIterator<TableViewColumn>())
            column->updateRoles(*this);
    }

    m_isActive = true;
}

QVariant TableViewModel::data(const QModelIndex &index, int role) const
{
    if(!sourceModel() || !m_columnModel || !index.isValid())
        return QVariant();

    if(role>ColumnObjectRole)
        return m_columnModel->data(m_columnModel->index(index.column(), 0), role-ColumnObjectRole);

    switch (role) {
    case Qt::EditRole:
        return QVariant();
    case Qt::DisplayRole:
    case ColumnFormattedRole:
        return formattedData(index);
    case ColumnColorRole:
        return formattedColor(index);
    case ColumnObjectRole:
        return QVariant::fromValue(m_columnModel->at<TableViewColumn>(index.column()));
    }

    return sourceData(index.row(), role);
}

bool TableViewModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!sourceModel() || !m_isActive || !index.isValid())
        return false;

    if(role==Qt::EditRole || role==ColumnFormattedRole)
        return setFormattedData(mapToSource(index), value);

    if(role<ColumnFormattedRole)
        return setSourceData(index.row(), value, role);

    return false;
}

QColor TableViewModel::formattedColor(const QModelIndex& index) const
{
    if (TableViewColumn* column = m_columnModel->at<TableViewColumn>(index.column())) {
        return column->color(index.row(), *this);
    }
    return QColor();
}

QVariant TableViewModel::formattedData(const QModelIndex &index) const
{
    if (TableViewColumn* column = m_columnModel->at<TableViewColumn>(index.column())) {
        return column->data(index.row(), *this);
    }
    return QVariant();
}

bool TableViewModel::setFormattedData(const QModelIndex& index, const QVariant &value) const
{
    if (TableViewColumn* column = m_columnModel->at<TableViewColumn>(index.column()))
        return column->setData(index.row(), value, *this);
    return false;
}

QVariantMap TableViewModel::sourceDatas(int row) const
{
    if(!sourceModel() || row<0)
        return QVariantMap();

    QVariantMap map;
    const QModelIndex modelIndex = sourceModel()->index(row, 0);
    QHash<int,QByteArray> names = sourceModel()->roleNames();
    for(auto [key, value]: names.asKeyValueRange())
        map.insert(value, sourceModel()->data(modelIndex, key));

    return map;
}

QVariant TableViewModel::sourceData(int row, int role) const
{
    if(!sourceModel() || row<0)
        return QVariant();
    return sourceModel()->data(sourceModel()->index(row,0), role);
}

bool TableViewModel::setSourceData(int row, const QVariant &value, int role) const
{
    if(!sourceModel() || row<0)
        return false;
    return sourceModel()->setData(sourceModel()->index(row,0), value, role);
}

QModelIndex TableViewModel::index(int row, int column, const QModelIndex& parent) const
{
    if (column >= 0)
        return createIndex(row, column, QIdentityProxyModel::index(row, 0, parent).internalPointer());
    return QIdentityProxyModel::index(row, column, parent);
}

QModelIndex TableViewModel::parent(const QModelIndex& child) const
{
    if (child.column() >= 0) {
        // Create an index for column 0 and use that to get the parent.
        const QModelIndex proxySibling = createIndex(child.row(), 0, child.internalPointer());
        return QIdentityProxyModel::parent(proxySibling);
    }
    return QIdentityProxyModel::parent(child);
}

QModelIndex TableViewModel::sibling(int row, int column, const QModelIndex &idx) const
{
    if (row == idx.row() && column == idx.column()) {
        return idx;
    }
    return index(row, column, parent(idx));
}

QHash<int, QByteArray> TableViewModel::roleNames() const
{
    return m_roleNames;
}

QModelIndexList TableViewModel::match(const QModelIndex& start, int role, const QVariant& value, int hits, Qt::MatchFlags flags) const
{
    return QAbstractProxyModel::match(start, role, value, hits, flags);
}

QVariant TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation==Qt::Vertical)
    {
        if(role==Qt::DisplayRole)
        {
            if(m_verticalHeaderDisplayRole>=0)
                return sourceData(section, m_verticalHeaderDisplayRole);
            return section;
        }
        return QVariant();
    }
    else if(orientation==Qt::Horizontal)
    {
        if(!m_columnModel)
            return QIdentityProxyModel::headerData(section, orientation, role);

        if(role==Qt::DisplayRole)
            return m_columnModel->data(m_columnModel->index(section, 0), m_horizontalHeaderDisplayRole);
        return m_columnModel->data(m_columnModel->index(section, 0), role-ColumnObjectRole);
    }

    return QVariant();
}

int TableViewModel::columnCount(const QModelIndex &parent) const
{
    if(!m_columnModel)
        return QIdentityProxyModel::columnCount(parent);
    return parent.isValid() ? 0 : m_columnModel->rowCount();
}

int TableViewModel::rowCount(const QModelIndex &parent) const
{
    if(!sourceModel())
        return QIdentityProxyModel::rowCount(parent);
    return parent.isValid() ? 0 : sourceModel()->rowCount();
}

int TableViewModel::roleForName(const QString& roleName) const
{
    int role = m_roleIds.value(roleName, -1);

    if(role<0 && !roleName.isEmpty() && !m_roleIds.isEmpty()) {
        TABLELOG_CRITICAL()<<"TableViewModel"<<this<<"does not contains role"<<roleName<<"for"<<sourceModel();
        qDebug()<<"roleNames are"<<roleNames();
    }

    return role;
}

QByteArray TableViewModel::roleName(int role) const
{
    return m_roleNames.value(role,"");
}

QModelIndex TableViewModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if(!sourceModel() || !proxyIndex.isValid())
        return QModelIndex();
    return QIdentityProxyModel::mapToSource(createIndex(proxyIndex.row(),0, proxyIndex.internalPointer()));
}

QModelIndex TableViewModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceModel() || !sourceIndex.isValid())
        return QModelIndex();
    return QIdentityProxyModel::mapFromSource(sourceIndex);
}

double TableViewModel::columnWidth(int column) const
{
    if(!isColumnVisible(column))
        return 0;
    return implicitColumnWidth(column);
}

double TableViewModel::implicitColumnWidth(int column) const
{
    if (TableViewColumn* tableColumn = m_columnModel->at<TableViewColumn>(column)) {
        return tableColumn->getWidth();
    }
    return -1;
}

bool TableViewModel::isColumnVisible(int column) const
{
    if (TableViewColumn* tableColumn = m_columnModel->at<TableViewColumn>(column)) {
        return tableColumn->getVisible();
    }
    return false;
}

bool TableViewModel::isColumnResizable(int column) const
{
    if (TableViewColumn* tableColumn = m_columnModel->at<TableViewColumn>(column)) {
        return tableColumn->getResizable();
    }
    return false;
}

void TableViewModel::resetInternalData()
{
    QIdentityProxyModel::resetInternalData();
}

void TableViewModel::countInvalidate()
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

void TableViewModel::updateRoleNames()
{
    if (!sourceModel())
        return;

    m_roleNames = sourceModel()->roleNames();
    m_roleNames[Qt::EditRole] = "edit";
    m_roleNames[Qt::DisplayRole] = "display";
    m_roleNames[ColumnFormattedRole] = "columnFormatted";
    m_roleNames[ColumnColorRole] = "columnColor";
    m_roleNames[ColumnObjectRole] = "columnObject";

    const QHash<int, QByteArray> columnRoleNames = m_columnModel->roleNames();

    m_roleNames.remove(columnRoleNames.size() + m_roleNames.size());
    for(auto [key, value]: columnRoleNames.asKeyValueRange())
        m_roleNames.insert(key + ColumnObjectRole, "column_"+value);

    reverseStringIntHash(m_roleIds, m_roleNames);
}

void TableViewModel::updateRoles()
{
    updateRoleNames();

    if(m_columnModel)
    {
        for (TableViewColumn* column : m_columnModel->modelIterator<TableViewColumn>())
            column->updateRoles(*this);
    }

    initHeadersRole();
}

void TableViewModel::initRoles()
{
    if(sourceModel() && !sourceModel()->roleNames().isEmpty())
    {
        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &TableViewModel::initRoles);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &TableViewModel::initRoles);
        resetInternalData();
        updateRoles();
    }
}

void TableViewModel::initHeadersRole()
{
    if(!m_verticalHeaderDisplayRoleName.isEmpty())
    {
        if(setVerticalHeaderDisplayRole(roleForName(m_verticalHeaderDisplayRoleName))) {
            // if(m_isActive)
            //     emit this->headerDataChanged(Qt::Vertical, 0, rowCount());
        }
    }

    if(!m_columnModel)
        return;

    if(!m_horizontalHeaderDisplayRoleName.isEmpty())
    {
        if(setHorizontalHeaderDisplayRole(m_columnModel->roleForName(m_horizontalHeaderDisplayRoleName.toUtf8()))) {
            if(m_isActive)
                emit this->headerDataChanged(Qt::Horizontal, 0, columnCount());
        }
    }
}

void TableViewModel::onColumnModelAboutToChange(TableViewColumnModel* oldColumnModel, TableViewColumnModel*)
{
    beginResetModel();

    if(oldColumnModel)
        disconnect(oldColumnModel, nullptr, this, nullptr);
}

void TableViewModel::onColumnModelChanged(TableViewColumnModel* columnModel)
{
    if (columnModel)
    {
        connect(columnModel, &QAbstractItemModel::dataChanged, this, &TableViewModel::onColumnDataChanged);
        connect(columnModel, &TableViewColumnModel::columnVisibleChanged, this, &TableViewModel::queueChangeLayout);
        connect(columnModel, &TableViewColumnModel::columnWidthChanged, this, &TableViewModel::queueChangeLayout);
        connect(columnModel, &TableViewColumnModel::countChanged, this, &TableViewModel::columnCountChanged);

        initHeadersRole();
    }

    endResetModel();
}

void TableViewModel::setSourceModel(QAbstractItemModel* model)
{
    if(sourceModel())
    {
        disconnect(sourceModel(), &QAbstractItemModel::dataChanged, this, &TableViewModel::onSourceDataChanged);

        disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &TableViewModel::initRoles);
        disconnect(sourceModel(), &QAbstractItemModel::modelReset, this, &TableViewModel::initRoles);
    }

    setHandleSourceDataChanges(false);

    QIdentityProxyModel::setSourceModel(model);

    if(sourceModel())
    {
        connect(sourceModel(), &QAbstractItemModel::dataChanged, this, &TableViewModel::onSourceDataChanged);

        if (sourceModel()->roleNames().isEmpty()) { // workaround for when a model has no roles and roles are added when the model is populated
            // QTBUG-57971
            connect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &TableViewModel::initRoles);
            connect(sourceModel(), &QAbstractItemModel::modelReset, this, &TableViewModel::initRoles);
        }
    }

    beginResetModel();

    endResetModel();
}

void TableViewModel::onColumnDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    if(!m_isActive)
        return;

    emit this->headerDataChanged(Qt::Horizontal, topLeft.row(), bottomRight.row());

    QVector<int> updatedRoles = roles;
    for (const int role: roles)
        updatedRoles<<(role + ColumnObjectRole);
    updatedRoles<<Qt::DisplayRole;
    updatedRoles<<ColumnFormattedRole;
    updatedRoles<<ColumnColorRole;
    updatedRoles<<ColumnObjectRole;

    emit this->dataChanged(index(0, topLeft.row()), index(rowCount()-1, bottomRight.row()), updatedRoles);
}

void TableViewModel::onSourceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    Q_ASSERT(topLeft.isValid() ? topLeft.model() == sourceModel() : true);
    Q_ASSERT(bottomRight.isValid() ? bottomRight.model() == sourceModel() : true);

    if(!m_isActive)
        return;

    //qDebug()<<"onSourceDataChanged"<<topLeft<<bottomRight<<roles;

    QVector<int> updatedRoles = roles;
    updatedRoles<<Qt::DisplayRole;
    updatedRoles<<ColumnFormattedRole;
    updatedRoles<<ColumnColorRole;

    const QModelIndex proxyTopLeft = createIndex(topLeft.row(), 0, topLeft.internalPointer());
    const QModelIndex proxyBottomRight = createIndex(bottomRight.row(), columnCount()-1, topLeft.internalPointer());

    emit this->dataChanged(proxyTopLeft, proxyBottomRight, updatedRoles);
}

void TableViewModel::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    //qDebug()<<"onDataChanged"<<topLeft<<bottomRight<<roles;
}

void TableViewModel::queueChangeLayout()
{
    if (m_delayed) {
        if (!m_layoutChangeQueued) {
            m_layoutChangeQueued = true;
            emit this->layoutAboutToBeChanged();
            QMetaObject::invokeMethod(this, &TableViewModel::changeLayout, Qt::QueuedConnection);
        }
    } else {
        changeLayout();
    }
}

void TableViewModel::changeLayout()
{
    if(!m_layoutChangeQueued)
        emit this->layoutAboutToBeChanged();

    m_layoutChangeQueued = false;
    emit this->layoutChanged();
}
