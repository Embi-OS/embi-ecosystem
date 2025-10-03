#include "sqlprimarycolumnpreparator.h"

SqlPrimaryColumnPreparator::SqlPrimaryColumnPreparator(QObject* parent) :
    SqlColumnPreparator("ID", SqlColumnTypes::Integer, SqlColumnOptions::AutoIncrement | SqlColumnOptions::Primary, QVariant(), parent)
{

}

SqlPrimaryColumnPreparator::SqlPrimaryColumnPreparator(const QString& name, int startValue, QObject* parent) :
    SqlColumnPreparator(name, SqlColumnTypes::Integer, SqlColumnOptions::AutoIncrement | SqlColumnOptions::Primary, QVariant(), parent),
    m_startValue(startValue)
{

}
