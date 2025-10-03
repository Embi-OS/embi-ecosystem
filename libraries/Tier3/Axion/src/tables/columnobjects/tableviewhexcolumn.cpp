#include "tableviewhexcolumn.h"

TableViewHexColumn::TableViewHexColumn(QObject* parent):
    TableViewColumn(parent)
{

}

QVariant TableViewHexColumn::data(int row, const TableViewModel& viewModel)
{
    const QVariant srcVal = sourceData(row, viewModel);

    switch(srcVal.metaType().id()){
    case QMetaType::QByteArray:
        return hex(srcVal.toByteArray());
    }

    return hex(srcVal.toLongLong());
}
