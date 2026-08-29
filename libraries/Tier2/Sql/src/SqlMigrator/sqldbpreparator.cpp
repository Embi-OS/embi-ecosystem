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

SqlDbProfile SqlDbPreparator::createConnectionProfile(const QVariantMap& settings)
{
    SqlDbProfile profile;

    switch(settings.value("dbType").toInt()) {
    case SqlDatabaseTypes::SQLite:
        profile.type = "QSQLITE";
        break;
    case SqlDatabaseTypes::MySQL:
        profile.type = "QMYSQL";
        break;
    case SqlDatabaseTypes::MariaDB:
        profile.type = "QMARIADB";
        break;
    default:
        break;
    }

    profile.name = settings.value("dbName").toString();
    profile.connectionName = settings.value("dbConnectionName", SqlDefaultConnection).toString();
    profile.path = settings.value("dbPath").toString();
    profile.hostName = settings.value("dbServer").toString();
    profile.userName = settings.value("dbUserName").toString();
    profile.password = settings.value("dbPassword").toString();
    profile.port = settings.value("dbPort", 3306).toInt();
    profile.timeout = settings.value("dbTimeout", 10000).toInt();
    profile.connectOptions = settings.value("dbConnectOptions", "MYSQL_OPT_RECONNECT=1; MYSQL_OPT_CONNECT_TIMEOUT=3").toString();

    return profile;
}

QString SqlDbPreparator::connectionProfileError(const SqlDbProfile& profile)
{
    if(profile.type.isEmpty())
        return tr("Type de base de données non supporté.");

    if(profile.name.isEmpty() && (profile.type != "QSQLITE" || profile.path.isEmpty()))
        return tr("Le nom de la base de données est requis.");

    if(profile.type == "QSQLITE" && profile.path.isEmpty() && profile.name != ":memory:")
        return tr("Le chemin de la base SQLite est requis.");

    if((profile.type == "QMYSQL" || profile.type == "QMARIADB") && profile.hostName.isEmpty())
        return tr("Le serveur de base de données est requis.");

    return {};
}

SqlDbProfile SqlDbPreparator::createConnectionProfile()
{
    QVariantMap settings;
    settings.insert("dbType", getType());
    settings.insert("dbName", getName());
    settings.insert("dbConnectionName", getConnectionName());
    settings.insert("dbPath", getPath());
    settings.insert("dbServer", getServer());
    settings.insert("dbUserName", getUserName());
    settings.insert("dbPassword", getPassword());
    settings.insert("dbPort", getPort());
    settings.insert("dbTimeout", getTimeout());
    settings.insert("dbConnectOptions", getConnectOptions());

    return createConnectionProfile(settings);
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
    case SqlDatabaseTypes::MariaDB:
        type = "QMARIADB";
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

    if(getType() == SqlDatabaseTypes::MySQL || getType() == SqlDatabaseTypes::MariaDB)
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
