#include "sqlcolumnpreparator.h"
#include "sql_log.h"

SqlColumnPreparator::SqlColumnPreparator(QObject* parent) :
    QObject(parent)
{

}

SqlColumnPreparator::SqlColumnPreparator(const QString& name, SqlColumnTypes::Enum type, SqlColumnOption options, const QVariant& defaultValue, QObject* parent) :
    QObject(parent),
    m_name(name),
    m_type(type),
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

    QHash<SqlColumnTypes::Enum, QString> typeMap;

    typeMap.insert(SqlColumnTypes::Char,             QString("CHAR(%1)").arg(getPrecision()));
    typeMap.insert(SqlColumnTypes::VarChar,          QString("VARCHAR(%1)").arg(getPrecision()));
    typeMap.insert(SqlColumnTypes::Binary,           QString("BINARY(%1)").arg(getPrecision()));
    typeMap.insert(SqlColumnTypes::VarBinary,        QString("VARBINARY(%1)").arg(getPrecision()));
    typeMap.insert(SqlColumnTypes::TinyText,                ("TINYTEXT"));
    typeMap.insert(SqlColumnTypes::Text,                    ("TEXT"));
    typeMap.insert(SqlColumnTypes::MediumText,              ("MEDIUMTEXT"));
    typeMap.insert(SqlColumnTypes::LongText,                ("LONGTEXT"));
    typeMap.insert(SqlColumnTypes::Json,                    ("JSON"));
    typeMap.insert(SqlColumnTypes::Uuid,                    ("CHAR(36)"));
    typeMap.insert(SqlColumnTypes::Blob,                    ("BLOB"));
    typeMap.insert(SqlColumnTypes::Boolean,                 ("BOOLEAN"));
    typeMap.insert(SqlColumnTypes::SmallInt,                ("SMALLINT"));
    typeMap.insert(SqlColumnTypes::Integer,                 ("INTEGER"));
    typeMap.insert(SqlColumnTypes::BigInt,                  ("BIGINT"));
    typeMap.insert(SqlColumnTypes::Float,                   ("FLOAT"));
    typeMap.insert(SqlColumnTypes::DoublePrecision,         ("DOUBLE PRECISION"));
    typeMap.insert(SqlColumnTypes::Date,                    ("DATE"));
    typeMap.insert(SqlColumnTypes::Time,                    ("TIME"));
    typeMap.insert(SqlColumnTypes::DateTime,                ("DATETIME"));
    typeMap.insert(SqlColumnTypes::Timestamp,               ("TIMESTAMP"));

    return typeMap.value(getType());
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
