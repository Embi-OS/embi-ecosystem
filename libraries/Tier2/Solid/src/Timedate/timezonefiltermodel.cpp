#include "timezonefiltermodel.h"
#include "timezonemodel.h"

TimezoneFilterModel::TimezoneFilterModel(QObject* parent):
    QSortFilterProxyModel(parent)
{
    connect(this, &TimezoneFilterModel::searchChanged, this, &TimezoneFilterModel::invalidate);
    TimezoneModel* timezoneModel = new TimezoneModel(this);
    setSourceModel(timezoneModel);
    m_nameRole=timezoneModel->roleForName("name");
    m_territoryRole=timezoneModel->roleForName("territory");
}

bool TimezoneFilterModel::filterAcceptsRow( int source_row, const QModelIndex& source_parent ) const
{
    bool ret = false;
    if (this->sourceModel())
    {
       QModelIndex index = this->sourceModel()->index( source_row, 0, source_parent );
       if (index.isValid())
       {
           QVariant nameRole = index.data(m_nameRole);
           if (nameRole.isValid())
           {
               QString name = nameRole.toString();
               if (name.contains(m_search,  Qt::CaseInsensitive)) {
                   ret = true;
               }
           }
           QVariant territoryRole = index.data(m_territoryRole);
           if (ret == false && territoryRole.isValid())
           {
               QString territory = territoryRole.toString();
               if (territory.contains(m_search,  Qt::CaseInsensitive)) {
                   ret = true;
               }
           }
       }
    }
    return ret;
}

int TimezoneFilterModel::indexForTimezone(const QString &timezone) const
{
    QAbstractItemModel *model = this->sourceModel();
    TimezoneModel* timezoneModel = qobject_cast<TimezoneModel*>(model);
    QModelIndex i = timezoneModel->indexForTimezone(timezone);
    QModelIndex ret = mapFromSource(i);
    return ret.row();
}
