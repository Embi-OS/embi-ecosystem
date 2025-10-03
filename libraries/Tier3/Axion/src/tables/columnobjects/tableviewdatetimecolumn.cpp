#include "tableviewdatetimecolumn.h"

TableViewDateTimeColumn::TableViewDateTimeColumn(QObject* parent):
    TableViewColumn(parent)
{
    connect(this, &TableViewDateTimeColumn::formatChanged, this, &TableViewDateTimeColumn::invalidate);
    connect(this, &TableViewDateTimeColumn::formatTypeChanged, this, &TableViewDateTimeColumn::invalidate);
}

QVariant TableViewDateTimeColumn::data(int row, const TableViewModel& viewModel)
{
    const QVariant srcVal = sourceData(row, viewModel);

    if(!m_format.isEmpty())
        return QLocale().toString(srcVal.toDateTime(), m_format);

    switch(srcVal.metaType().id()){
    case QMetaType::QDateTime:
        return QLocale().toString(srcVal.toDateTime(), m_formatType);
    case QMetaType::QDate:
        return QLocale().toString(srcVal.toDate(), m_formatType);
    case QMetaType::QTime:
        return QLocale().toString(srcVal.toTime(), m_formatType);
    case QMetaType::QString:
        return srcVal;
    }

    return "N/A";
}
