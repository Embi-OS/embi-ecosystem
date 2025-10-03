#include "tableviewlistcolumn.h"

TableViewListColumn::TableViewListColumn(QObject* parent):
    TableViewColumn(parent)
{
    connect(this, &TableViewListColumn::separatorChanged, this, &TableViewListColumn::invalidate);
}

QVariant TableViewListColumn::data(int row, const TableViewModel& viewModel)
{
    const QVariant srcVal = sourceData(row, viewModel);
    const QVariantList srcList = srcVal.toList();

    QStringList ret;
    ret.reserve(srcList.size());
    for(const QVariant& val: srcList)
        ret.append(val.toString());

    return ret.join(m_separator);
}
