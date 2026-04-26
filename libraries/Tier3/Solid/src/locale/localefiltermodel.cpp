#include "localefiltermodel.h"
#include "localemodel.h"

LocaleFilterModel::LocaleFilterModel(QObject* parent):
    QSortFilterProxyModel(parent)
{
    connect(this, &LocaleFilterModel::searchChanged, this, &LocaleFilterModel::invalidate);
    connect(this, &LocaleFilterModel::filtersChanged, this, &LocaleFilterModel::invalidate);
    LocaleModel *localeModel = new LocaleModel(this);
    setSourceModel(localeModel);
    m_localeRole=localeModel->roleForName("locale");
    m_nameRole=localeModel->roleForName("name");
    m_languageRole=localeModel->roleForName("language");
    m_territoryRole=localeModel->roleForName("territory");
}

bool LocaleFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    bool ret = false;
    if (this->sourceModel() && m_nameRole>=0 && m_territoryRole>=0)
    {
       const QModelIndex index = this->sourceModel()->index(source_row, 0, source_parent);
       if (index.isValid())
       {
           const QVariant nameRole = index.data(m_nameRole);
           const QString name = nameRole.toString();
           if (nameRole.isValid() && !m_filters.isEmpty())
           {
               if(!m_filters.contains(name)) {
                   return false;
               }
           }

           if (nameRole.isValid())
           {
               if (name.contains(m_search,  Qt::CaseInsensitive)) {
                   ret = true;
               }
           }
           const QVariant languageRole = index.data(m_languageRole);
           if (languageRole.isValid())
           {
               const QString language = languageRole.toString();
               if (language.contains(m_search,  Qt::CaseInsensitive)) {
                   ret = true;
               }
           }
           const QVariant territoryRole = index.data(m_territoryRole);
           if (territoryRole.isValid())
           {
               const QString territory = territoryRole.toString();
               if (territory.contains(m_search,  Qt::CaseInsensitive)) {
                   ret = true;
               }
           }
       }
    }

    return ret;
}

int LocaleFilterModel::indexForTerritory(const QString &territory) const
{
    QAbstractItemModel *model = this->sourceModel();
    LocaleModel *localModel = qobject_cast<LocaleModel*>(model);
    QModelIndex i = localModel->indexForTerritory(territory);
    QModelIndex ret = mapFromSource(i);
    return ret.row();
}
