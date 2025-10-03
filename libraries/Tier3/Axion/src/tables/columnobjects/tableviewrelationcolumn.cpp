#include "tableviewrelationcolumn.h"
#include <QModels>

TableViewRelationColumn::TableViewRelationColumn(QObject* parent):
    TableViewColumn(parent),
    m_proxyModel(new QProxyModel(this))
{
    m_proxyModel->setSortOrder(Qt::AscendingOrder);
    m_proxyModel->setSortCaseSensitivity(Qt::CaseSensitive);
    m_proxyModel->setSortLocaleAware(false);

    connect(this, &TableViewRelationColumn::keyRoleNameChanged, m_proxyModel, &QProxyModel::setSortRoleName);
    connect(this, &TableViewRelationColumn::keyRoleChanged, m_proxyModel, &QProxyModel::setSortRole);

    connect(this, &TableViewRelationColumn::modelAboutToChange, this, &TableViewRelationColumn::onModelAboutToChanged);
    connect(this, &TableViewRelationColumn::modelChanged, this, &TableViewRelationColumn::onModelChanged);
    connect(this, &TableViewRelationColumn::relationRoleNameChanged, this, &TableViewRelationColumn::updateModelRoles);
}

void TableViewRelationColumn::viewModelCompleted(const TableViewModel& viewModel)
{
    Q_UNUSED(viewModel)
    m_proxyModel->invalidate();
}

QVariant TableViewRelationColumn::data(int row, const TableViewModel& viewModel)
{
    const QVariant sourceVal = sourceData(row, viewModel);
    const int index = QQsfpm::indexOf(m_proxyModel, m_proxyModel->sortRole(), sourceVal);

    if(index<0)
        return "N/A";

    const QVariant& val = m_proxyModel->data(m_proxyModel->index(index,0), m_relationRole);

    return val;
}

void TableViewRelationColumn::onModelAboutToChanged(QAbstractItemModel* oldModel, QAbstractItemModel* newModel)
{
    Q_UNUSED(newModel)
    if(oldModel) {
        disconnect(oldModel, nullptr, this, nullptr);
    }
}

void TableViewRelationColumn::onModelChanged(QAbstractItemModel* model)
{
    m_proxyModel->setSourceModel(model);

    if (model != nullptr) {
        connect(model, &QAbstractItemModel::destroyed, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::dataChanged, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::headerDataChanged, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::rowsInserted, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::rowsMoved, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::columnsRemoved, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::columnsInserted, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::columnsMoved, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::modelReset, this, &TableViewRelationColumn::invalidate);
        connect(model, &QAbstractItemModel::layoutChanged, this, &TableViewRelationColumn::invalidate);
    }

    updateModelRoles();
    invalidate();
}

void TableViewRelationColumn::updateModelRoles()
{
    if(!m_model)
        return;

    disconnect(m_model, &QAbstractItemModel::rowsInserted, this, &TableViewRelationColumn::updateModelRoles);
    disconnect(m_model, &QAbstractItemModel::modelReset, this, &TableViewRelationColumn::updateModelRoles);

    if(!m_relationRoleName.isEmpty())
        m_relationRole = m_model->roleNames().key(m_relationRoleName.toUtf8(), -1);

    if(m_relationRole<0) {
        connect(m_model, &QAbstractItemModel::rowsInserted, this, &TableViewRelationColumn::updateModelRoles);
        connect(m_model, &QAbstractItemModel::modelReset, this, &TableViewRelationColumn::updateModelRoles);
    }
}
