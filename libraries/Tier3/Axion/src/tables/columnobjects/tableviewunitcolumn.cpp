#include "tableviewunitcolumn.h"
#include "tableviewmodel.h"
#include "axion_log.h"

TableViewUnitColumn::TableViewUnitColumn(QObject* parent):
    TableViewColumn(parent)
{
    connect(this, &TableViewUnitColumn::showUnitTypeChanged, this, &TableViewUnitColumn::invalidate);
}

void TableViewUnitColumn::updateRoles(const TableViewModel& viewModel)
{
    TableViewColumn::updateRoles(viewModel);

    m_unitRole = -1;
    if(!m_unitRoleName.isEmpty())
    {
        QStringList unitRoleNames = m_unitRoleName.split(m_roleNameSeparator, Qt::SkipEmptyParts);
        const QString unitRoleName = unitRoleNames.takeFirst();
        m_unitRole = viewModel.roleForName(unitRoleName);
        m_unitSubRoleNames = unitRoleNames;
    }
}

QVariant TableViewUnitColumn::data(int row, const TableViewModel& viewModel)
{
    const double rawValue = sourceData(row, viewModel).toDouble();

    if(!m_unitModel)
        return m_decimals<0 ? rawValue : qFuzzyRound(rawValue, m_decimals);

    const UnitType type = (UnitType)sourceUnitData(row, viewModel).toInt();

    if(m_showUnitType)
        return m_unitModel->display(rawValue, type, m_decimals);
    else
        return m_unitModel->formatted(rawValue, type, m_decimals);
}

QVariant TableViewUnitColumn::sourceUnitData(int row, const TableViewModel& viewModel)
{
    const QVariant variant = viewModel.sourceData(row, m_unitRole);

    if(m_unitSubRoleNames.isEmpty() || !variant.canConvert<QVariantMap>())
        return variant;

    bool ok = true;
    const QVariant& nestedVariant = qVariantGetNestedValue(variant, m_unitSubRoleNames);
    if(!ok) {
        AXIONLOG_DEBUG()<<getUnitRoleName()<<"does not exist";
    }

    return nestedVariant;
}
