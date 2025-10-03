#include "sqlschemepreparator.h"
#include "sqlquerybuilder.h"
#include "sqldbpool.h"
#include "sql_log.h"

SqlSchemePreparator::SqlSchemePreparator(QObject* parent) :
    QObject(parent),
    m_tables(this)
{

}

bool SqlSchemePreparator::isValid()
{
    for(SqlTablePreparator* table: m_tables)
    {
        if(!table->isValid())
        {
            SQLLOG_CRITICAL("table is not valid");
            return false;
        }
    }

    return true;
}

bool SqlSchemePreparator::run(const QString& dbConnectionName, bool canDrop)
{
    bool result = doRun(dbConnectionName, false);

    return result;
}

bool SqlSchemePreparator::doRun(const QString& dbConnectionName, bool canDrop)
{
    if(!SqlDbPool::database(dbConnectionName, false).isValid())
    {
        SQLLOG_CRITICAL()<<"Invalid connection name given to SqlSchemePreparator::execute:"<<dbConnectionName;
        return false;
    }

    if(!SqlDbPool::database(dbConnectionName, false).isOpen())
    {
        SQLLOG_CRITICAL()<<"Connection name"<<dbConnectionName<<"is closed";
        return false;
    }

    m_dbConnectionName = dbConnectionName;

    if(m_tables.isEmpty())
    {
        SQLLOG_DEBUG()<<"No table given to the scheme preparator"<<m_dbConnectionName;
        return true;
    }

    if(!initTables())
        return false;

    if(!createTables())
        return false;

    if(canDrop)
    {
        if(!dropTables())
            return false;
    }

    if(!updatePrimaries())
        return false;

    if(!fillDefaultEntriesTables())
        return false;

    if(!fillBasicEntriesTables())
        return false;

    return true;
}

bool SqlSchemePreparator::initTables()
{
    for(SqlTablePreparator* table: m_tables)
    {
        if(!table->init(m_dbConnectionName))
        {
            SQLLOG_CRITICAL()<<"Error while initializing table:"<<table->getName();
            return false;
        }
    }

    return true;
}

bool SqlSchemePreparator::createTables()
{
    QStringList existingTables = SqlDbPool::driver(m_dbConnectionName)->tables(QSql::Tables);

    for(SqlTablePreparator* table: m_tables)
    {
        bool result = true;

        if(existingTables.contains(table->getName()+"_new"))
            SqlBuilder::drop().table(table->getName()+"_new").ifExists().connection(m_dbConnectionName).exec(&result);

        SQLLOG_NOTICE()<<"Working on table:"<<table->getName();

        if(existingTables.contains(table->getName()))
        {
            result = table->update();
        }
        else
        {
            result = table->create();
        }

        if(!result)
        {
            SQLLOG_CRITICAL()<<"Error while working on table:"<<table->getName();
        }
    }

    return true;
}

bool SqlSchemePreparator::dropTables()
{
    const QStringList existingTables = SqlDbPool::driver(m_dbConnectionName)->tables(QSql::Tables);
    for(const QString& tableName: existingTables)
    {
        bool mustDrop=true;
        if(tableName.contains("sqlite_master") ||
           tableName.contains("sqlite_sequence"))
            mustDrop=false;

        for(const SqlTablePreparator* table: m_tables)
        {
            if(table->getName()==tableName)
                mustDrop=false;
        }

        if(mustDrop)
        {
            SQLLOG_INFO()<<"Droping table:"<<tableName;

            bool result = false;
            SqlBuilder::drop().table(tableName).connection(m_dbConnectionName).exec(&result);

            if(!result)
            {
                SQLLOG_CRITICAL()<<"Error while droppings table:"<<tableName;
            }
        }
    }

    return true;
}

bool SqlSchemePreparator::updatePrimaries()
{
    SqlDbPool::database(m_dbConnectionName).transaction();

    for(SqlTablePreparator* table: m_tables)
    {
        if(!table->updatePrimary())
        {
            SQLLOG_CRITICAL()<<"Error while updating primary on table:"<<table->getName();
        }
    }

    SqlDbPool::database(m_dbConnectionName).commit();

    return true;
}

bool SqlSchemePreparator::fillDefaultEntriesTables()
{
    SqlDbPool::database(m_dbConnectionName).transaction();

    for(SqlTablePreparator* table: m_tables)
    {
        if(!table->fillDefaultEntries())
        {
            SQLLOG_CRITICAL()<<"Error while filling defaults entries on table:"<<table->getName();
        }
    }

    SqlDbPool::database(m_dbConnectionName).commit();

    return true;
}

bool SqlSchemePreparator::fillBasicEntriesTables()
{
    SqlDbPool::database(m_dbConnectionName).transaction();

    for(SqlTablePreparator* table: m_tables)
    {
        if(!table->fillBasicEntries())
        {
            SQLLOG_CRITICAL()<<"Error while filling basics entries on table:"<<table->getName();
        }
    }

    SqlDbPool::database(m_dbConnectionName).commit();

    return true;
}
