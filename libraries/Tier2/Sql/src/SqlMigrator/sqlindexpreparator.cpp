#include "sqlindexpreparator.h"

SqlIndexPreparator::SqlIndexPreparator(QObject* parent) :
    SqlIndexPreparator({}, parent)
{

}

SqlIndexPreparator::SqlIndexPreparator(const QStringList& fields, QObject* parent) :
    QObject(parent),
    m_fields(fields)
{

}

bool SqlIndexPreparator::isValid() const
{
    return !m_fields.isEmpty();
}
