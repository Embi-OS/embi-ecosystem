#include "modelfilter.h"
#include "qqmlsortfilterproxymodel.h"

#include "QDebug"
#include "QElapsedTimer"

namespace QQsfpm {

/*!
    \qmltype ModelFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief Filters rows matching exactly a value contained in a different model.

    A ModelFilter is a simple \l RoleFilter that accepts rows matching exactly the model

    In the following example, only rows with their \c favorite role set to \c true will be accepted when the checkbox is checked :
    \code
    CheckBox {
       id: showOnlyFavoriteCheckBox
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: ModelFilter {
           roleName: "ID"
           model: compareModel
           modelRoleName: "idcompare"
       }
    }
    \endcode

*/

ModelFilter::ModelFilter(QObject *parent) :
    RoleFilter(parent),
    m_proxyModel(new QProxyModel(this))
{
    m_proxyModel->setSortOrder(Qt::AscendingOrder);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseSensitive);
    m_proxyModel->setSortLocaleAware(false);

    connect(m_proxyModel, &QAbstractItemModel::dataChanged, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::headerDataChanged, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::rowsInserted, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::rowsRemoved, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::rowsMoved, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::columnsRemoved, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::columnsInserted, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::columnsMoved, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::modelReset, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QAbstractItemModel::layoutChanged, this, &ModelFilter::invalidate);
    connect(m_proxyModel, &QProxyModel::roleNamesChanged, this, &ModelFilter::roleNamesInvalidated);

    connect(this, &ModelFilter::delayedChanged, m_proxyModel, &QProxyModel::setDelayed);
    connect(this, &ModelFilter::modelRoleNameChanged, m_proxyModel, &QProxyModel::setSortRoleName);
    connect(this, &ModelFilter::modelRoleChanged, m_proxyModel, &QProxyModel::setSortRole);

    connect(this, &ModelFilter::modelRoleNameChanged, this, &ModelFilter::invalidate);
    connect(this, &ModelFilter::modelRoleChanged, this, &ModelFilter::invalidate);
    connect(this, &ModelFilter::modelAboutToChange, this, &ModelFilter::onModelAboutToChange);
    connect(this, &ModelFilter::modelChanged, this, &ModelFilter::onModelChanged);
}

void ModelFilter::onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel)
{
    if(oldModel!=nullptr) {
        disconnect(oldModel, nullptr,this, nullptr);
    }
}

void ModelFilter::onModelChanged(QAbstractItemModel* model)
{
    m_proxyModel->setSourceModel(model);

    invalidate();
}

void ModelFilter::proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel)
{
    Q_UNUSED(proxyModel)
    m_proxyModel->invalidate();
}

bool ModelFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    const QVariant& sourceVal = sourceData(sourceIndex, proxyModel);

    const int index = QQsfpm::indexOf(m_proxyModel, m_proxyModel->sortRole(), sourceVal);

    return index >= 0;
}

}
