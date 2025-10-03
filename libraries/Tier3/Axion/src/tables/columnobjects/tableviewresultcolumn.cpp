#include "tableviewresultcolumn.h"
#include "tableviewmodel.h"
#include "axion_log.h"

#include "axion_helpertypes.h"

TableViewResultColumn::TableViewResultColumn(QObject* parent):
    TableViewColumn(parent)
{
    m_colorDefault = Theme::Get()->getYellow();
    m_colorSuccess= Theme::Get()->getGreen();
    m_colorFailure = Theme::Get()->getRed();
}

void TableViewResultColumn::updateRoles(const TableViewModel& viewModel)
{
    TableViewColumn::updateRoles(viewModel);

    m_resultRole = -1;
    if(!m_resultRoleName.isEmpty())
    {
        QStringList resultRoleNames = m_resultRoleName.split(m_roleNameSeparator, Qt::SkipEmptyParts);
        const QString resultRoleName = resultRoleNames.takeFirst();
        m_resultRole = viewModel.roleForName(resultRoleName);
        m_resultSubRoleNames = resultRoleNames;
    }
    if(m_resultRole<0)
    {
        m_resultRole = m_role;
        m_resultSubRoleNames = m_subRoleNames;
    }
}

QColor TableViewResultColumn::color(int row, const TableViewModel& viewModel)
{
    const int result = sourceResultData(row, viewModel).toInt();
    switch(result) {
    case 1:
        return m_colorSuccess;
    case 2:
        return m_colorFailure;
    }
    return m_colorDefault;
}

QVariant TableViewResultColumn::data(int row, const TableViewModel& viewModel)
{
    const QVariant srcVal = sourceData(row, viewModel);

    if(m_resultRole==m_role) {
        const int result = sourceResultData(row, viewModel).toInt();
        switch(result) {
        case 1:
            return "OK";
        case 2:
            return "KO";
        }
        return "N/A";
    }

    return srcVal;
}

QVariant TableViewResultColumn::sourceResultData(int row, const TableViewModel& viewModel)
{
    const QVariant variant = viewModel.sourceData(row, m_resultRole);

    if(m_resultSubRoleNames.isEmpty())
        return variant;

    bool ok = true;
    const QVariant& nestedVariant = qVariantGetNestedValue(variant, m_resultSubRoleNames);
    if(!ok) {
        AXIONLOG_DEBUG()<<getResultRoleName()<<"does not exist";
    }

    return nestedVariant;
}
