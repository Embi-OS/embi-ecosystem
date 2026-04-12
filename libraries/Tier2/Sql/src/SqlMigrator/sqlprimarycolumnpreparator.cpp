#include "sqlprimarycolumnpreparator.h"

SqlPrimaryColumnPreparator::SqlPrimaryColumnPreparator(QObject* parent) :
    SqlColumnPreparator("ID", SqlColumnTypes::Integer, 0, SqlColumnOptions::AutoIncrement | SqlColumnOptions::Primary, QVariant(), parent)
{

}

SqlPrimaryColumnPreparator::SqlPrimaryColumnPreparator(const QString& name, int startValue, QObject* parent) :
    SqlColumnPreparator(name, SqlColumnTypes::Integer, 0, SqlColumnOptions::AutoIncrement | SqlColumnOptions::Primary, QVariant(), parent),
    m_startValue(startValue)
{

}
