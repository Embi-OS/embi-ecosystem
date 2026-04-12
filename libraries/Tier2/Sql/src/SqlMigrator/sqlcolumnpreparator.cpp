#include "sqlcolumnpreparator.h"
#include "sql_log.h"

SqlColumnPreparator::SqlColumnPreparator(QObject* parent) :
    QObject(parent)
{

}

SqlColumnPreparator::SqlColumnPreparator(const QString& name, SqlColumnTypes::Enum type, int precision, SqlColumnOption options, const QVariant& defaultValue, QObject* parent) :
    QObject(parent),
    m_name(name),
    m_type(type),
    m_precision(precision),
    m_options(options),
    m_defaultValue(defaultValue)
{
    if(name.isEmpty()) {
        SQLLOG_WARNING()<<"SqlColumnPreparator with empty name!";
    }
}

bool SqlColumnPreparator::isValid() const
{
    return (!m_name.isEmpty()) && m_type != SqlColumnTypes::Invalid;
}

bool SqlColumnPreparator::hasDefaultValue() const
{
    return (m_defaultValue.isValid() && !m_defaultValue.isNull());
}

bool SqlColumnPreparator::isNullable() const
{
    return !m_options.testFlag(SqlColumnOptions::NotNullable);
}

bool SqlColumnPreparator::isPrimary() const
{
    return m_options.testFlag(SqlColumnOptions::Primary);
}

bool SqlColumnPreparator::isUnique() const
{
    return m_options.testFlag(SqlColumnOptions::Unique);
}

bool SqlColumnPreparator::isAutoIncremented() const
{
    return m_options.testFlag(SqlColumnOptions::AutoIncrement);
}

QString SqlColumnPreparator::generateDefinition(const QSqlDriver *driver) const
{
    if(!driver)
        return QString();

    QStringList columnDefinition;

    QString nameDefinition = driver->escapeIdentifier(getName(), QSqlDriver::FieldName);
    if(!nameDefinition.isEmpty())
        columnDefinition.append(nameDefinition);

    QString typeDefinition = generateType(driver);
    if(!typeDefinition.isEmpty())
        columnDefinition.append(typeDefinition);

    QString optionsDefinition = generateOptions(driver);
    if(!optionsDefinition.isEmpty())
        columnDefinition.append(optionsDefinition);

    return columnDefinition.join(" ");
}

QString SqlColumnPreparator::generateType(const QSqlDriver *driver) const
{
    if(!driver)
        return QString();

    return SqlHelper::sqlTypeGenerated(getType(), getPrecision());
}

QString SqlColumnPreparator::generateOptions(const QSqlDriver *driver) const
{
    if(!driver)
        return QString();

    QStringList options;
    if(isPrimary())
    {
        options.append("PRIMARY KEY");
        if (isAutoIncremented())
        {
            if(driver->dbmsType()==QSqlDriver::SQLite)
                options.append("AUTOINCREMENT");
            else if(driver->dbmsType()==QSqlDriver::MySqlServer)
                options.append("AUTO_INCREMENT");
        }
    }
    else
    {
        if(!isNullable())
        {
            options.append("NOT NULL");
        }

        if(isUnique())
        {
            options.append("UNIQUE");
        }
    }

    if(hasDefaultValue() && !isPrimary())
    {
        options.append(QString("DEFAULT %1").arg(Sql::formatValue(getDefaultValue(), driver)));
    }

    return options.join(" ");
}
