#include "sqldbpreparator.h"
#include "sql_log.h"

SqlDbPreparator::SqlDbPreparator(QObject* parent) :
    SqlSchemePreparator(parent)
{

}

bool SqlDbPreparator::execute()
{
    return SqlSchemePreparator::run(getConnectionName(), getCanDrop());
}

SqlDbProfile SqlDbPreparator::createConnectionProfile()
{
    SqlDbProfile profile;

    switch(getType()) {
    case SqlDatabaseTypes::SQLite:
        profile.type = "QSQLITE";
        break;
    case SqlDatabaseTypes::MySQL:
        profile.type = "QMYSQL";
        break;
    default:
        break;
    }

    profile.name = m_name;
    profile.connectionName = m_connectionName;
    profile.path = m_path;
    profile.hostName = m_server;
    profile.userName = m_userName;
    profile.password = m_password;
    profile.port = m_port;
    profile.timeout = m_timeout;
    profile.connectOptions = m_connectOptions;

    return profile;
}

QString SqlDbPreparator::toString()
{
    QString str;

    QString type;
    switch(getType()) {
    case SqlDatabaseTypes::SQLite:
        type = "QSQLITE";
        break;
    case SqlDatabaseTypes::MySQL:
        type = "QMYSQL";
        break;
    default:
        break;
    }

    str.append(QString("\n"));
    str.append(QString("connectionName  : %1\n").arg(getConnectionName()));
    str.append(QString("type            : %1\n").arg(type));
    str.append(QString("name            : %1\n").arg(getName()));
    str.append(QString("path            : %1\n").arg(getPath()));
    str.append(QString("server          : %1\n").arg(getServer()));
    str.append(QString("userName        : %1\n").arg(getUserName()));
    str.append(QString("port            : %1\n").arg(getPort()));

    return str;
}

bool SqlDbPreparator::isValid()
{
    if(getType()==SqlDatabaseTypes::None)
    {
        SQLLOG_CRITICAL("Unsuported type");
        return false;
    }

    if(getName().isEmpty())
    {
        SQLLOG_CRITICAL("name must be set");
        return false;
    }

    if(getType() == SqlDatabaseTypes::MySQL)
    {
        if(getServer().isEmpty())
        {
            SQLLOG_CRITICAL("server must be set");
            return false;
        }
    }
    else if(getType() == SqlDatabaseTypes::SQLite)
    {
        if(getPath().isEmpty() && getName()!=":memory:")
        {
            SQLLOG_CRITICAL("path must be set");
            return false;
        }
    }

    return SqlSchemePreparator::isValid();
}
