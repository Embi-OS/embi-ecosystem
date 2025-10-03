#include "tableviewokkocolumn.h"
#include "tableviewmodel.h"
#include "axion_log.h"

#include "axion_helpertypes.h"

TableViewOkKoColumn::TableViewOkKoColumn(QObject* parent):
    TableViewColumn(parent)
{
    m_colorOk = Theme::Get()->getGreen();
    m_colorKo = Theme::Get()->getRed();
}

void TableViewOkKoColumn::updateRoles(const TableViewModel& viewModel)
{
    TableViewColumn::updateRoles(viewModel);

    m_okkoRole = -1;
    if(!m_okkoRoleName.isEmpty())
    {
        QStringList okkoRoleNames = m_okkoRoleName.split(m_roleNameSeparator, Qt::SkipEmptyParts);
        const QString okkoRoleName = okkoRoleNames.takeFirst();
        m_okkoRole = viewModel.roleForName(okkoRoleName);
        m_okkoSubRoleNames = okkoRoleNames;
    }
    if(m_okkoRole<0)
    {
        m_okkoRole = m_role;
        m_okkoSubRoleNames = m_subRoleNames;
    }
}

QColor TableViewOkKoColumn::color(int row, const TableViewModel& viewModel)
{
    const bool okko = sourceOKKOData(row, viewModel).toBool();
    return okko ? getColorKo() : getColorOk();
}

QVariant TableViewOkKoColumn::data(int row, const TableViewModel& viewModel)
{
    const QVariant srcVal = sourceData(row, viewModel);

    if(m_okkoRoleName.isEmpty())
        return srcVal.toBool() ? "KO" : "OK";

    return srcVal;
}

QVariant TableViewOkKoColumn::sourceOKKOData(int row, const TableViewModel& viewModel)
{
    const QVariant& variant = viewModel.sourceData(row, m_okkoRole);

    if(m_okkoSubRoleNames.isEmpty())
        return variant;

    bool ok = true;
    const QVariant& nestedVariant = qVariantGetNestedValue(variant, m_okkoSubRoleNames);
    if(!ok) {
        AXIONLOG_DEBUG()<<m_okkoRoleName<<"does not exist";
    }

    return nestedVariant;
}
