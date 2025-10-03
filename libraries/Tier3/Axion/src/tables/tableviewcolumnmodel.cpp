#include "tableviewcolumnmodel.h"

TableViewColumnModel::TableViewColumnModel(QObject* parent) :
    QObjectListModel(parent, &TableViewColumn::staticMetaObject)
{
    onInserted<TableViewColumn>([this](TableViewColumn* object){
        connect(object, &TableViewColumn::essentialChanged, this, &TableViewColumnModel::onColumnEssentialChanged);
        connect(object, &TableViewColumn::visibleChanged, this, &TableViewColumnModel::onColumnVisibleChanged);
        connect(object, &TableViewColumn::widthChanged, this, &TableViewColumnModel::onColumnWidthChanged);

        connect(this, &TableViewColumnModel::roleNameSeparatorChanged, object, &TableViewColumn::setRoleNameSeparator);
        object->setRoleNameSeparator(m_roleNameSeparator);
    });
}

QStringList TableViewColumnModel::getActiveRoleNames() const
{
    QStringList roleNames;

    for(TableViewColumn* column: this->modelIterator<TableViewColumn>())
    {
        const QString roleName = column->getRoleName().split(column->getRoleNameSeparator(), Qt::SkipEmptyParts).takeFirst();
        if(roleName=="*")
            return QStringList();;
        if((column->getVisible() || column->getEssential()) && !roleName.isEmpty() && !roleNames.contains(roleName))
            roleNames.append(roleName);
    }

    return roleNames;
}

QString TableViewColumnModel::getColumnRoleName(int column, bool full) const
{
    if(column < 0 || column >= size())
        return QString();

    TableViewColumn* columnObject = at<TableViewColumn>(column);

    if(full)
        return columnObject->getRoleName();

    const QString roleName = columnObject->getRoleName().split(columnObject->getRoleNameSeparator(), Qt::SkipEmptyParts).takeFirst();

    return roleName;
}

bool TableViewColumnModel::isColumnSortable(int column) const
{
    if(column < 0 || column >= size())
        return false;
    TableViewColumn* columnObject = at<TableViewColumn>(column);
    return columnObject->getSortable();
}

bool TableViewColumnModel::isColumnFilterable(int column) const
{
    if(column < 0 || column >= size())
        return false;
    TableViewColumn* columnObject = at<TableViewColumn>(column);
    return columnObject->getFilterable();
}

void TableViewColumnModel::onColumnVisibleChanged(bool visible)
{
    TableViewColumn* column = qobject_cast<TableViewColumn*>(sender());
    if(!column)
        return;

    int index = indexOf(column);
    if(index<0)
        return;

    emit this->activeRoleNamesChanged();
    emit this->columnVisibleChanged(index, visible);
}

void TableViewColumnModel::onColumnEssentialChanged(bool visible)
{
    TableViewColumn* column = qobject_cast<TableViewColumn*>(sender());
    if(!column)
        return;

    int index = indexOf(column);
    if(index<0)
        return;

    emit this->activeRoleNamesChanged();
}

void TableViewColumnModel::onColumnWidthChanged(int width)
{
    TableViewColumn* column = qobject_cast<TableViewColumn*>(sender());
    if(!column)
        return;

    int index = indexOf(column);
    if(index<0)
        return;

    emit this->columnWidthChanged(index, width);
}
