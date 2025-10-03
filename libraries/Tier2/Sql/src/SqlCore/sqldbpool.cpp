#include "sqldbpool.h"
#include "sql_log.h"

#include <QThread>

class SqlDbConnection
{
    Q_DISABLE_COPY(SqlDbConnection)
public:
    SqlDbConnection(const QString& suffix) :
        m_suffix(suffix)
    {}

    ~SqlDbConnection() {
        SqlDbPool::closeThread(m_suffix);
    }

private:
    QString m_suffix;
};
static QThreadStorage<SqlDbConnection*> s_connections;

SqlDbPool::SqlDbPool()
{

}

bool SqlDbPool::init(const SqlDbProfile& profile, int retry)
{
    QSqlDatabase db = create(profile);
    int opennings = 0;

    QElapsedTimer timer;
    timer.start();

    do
    {
        opennings++;

        if(!db.open())
        {
            SQLLOG_WARNING()<<"Failed to open database:"<<db.connectionName()<<"timeout:"<<timer.nsecsElapsed()/1000000.0;
        }
    } while ([&]() -> bool {
        if(db.isOpen())
            return false;

        if(retry>0 && opennings>=retry)
            return false;

        int timeout = timer.nsecsElapsed()/1000000.0;
        if(timeout>=profile.timeout)
            return false;

        QUtils::await(1000);
        return true;
    }());

    if(!db.isOpen())
    {
        SQLLOG_CRITICAL()<<"Failed to open database:"<<db.connectionName();
        SQLLOG_CRITICAL()<<opennings<<"essais";
        SQLLOG_CRITICAL()<<db.lastError();
    }
    else
    {
        SQLLOG_INFO()<<"Database opening"<<profile.name<<"took"<<opennings<<"tries and"<<timer.elapsed()/1000.0<<"seconds";
    }

    return db.isOpen();
}

QSqlDatabase SqlDbPool::create(const SqlDbProfile& profile, bool force)
{
    QString threadConnection = profile.connectionName;
    const QString thread = pointerString(QThread::currentThread());

    if(QThread::currentThread()!=qApp->thread())
        threadConnection = QString("%1_%2").arg(profile.connectionName, thread);

    if(QSqlDatabase::contains(threadConnection))
    {
        SQLLOG_DEBUG()<<"Sql connection"<<threadConnection<<"already created";
        return QSqlDatabase::database(threadConnection, false);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(profile.type, threadConnection);

    if(profile.type=="QMYSQL")
    {
        db.setHostName(profile.hostName);
        db.setPort(profile.port);
        db.setUserName(profile.userName);
        db.setPassword(profile.password);
        db.setConnectOptions(profile.connectOptions);
        if (force && db.open())
        {
            const QString sql = QString("CREATE DATABASE IF NOT EXISTS %1").arg(profile.name);
            QSqlQuery query(db);
            bool result = query.exec(sql);
            if(!result)
            {
                const QSqlError error = query.lastError();
                SQLLOG_WARNING()<<"Error while creating database!";
                SQLLOG_WARNING()<<sql;
                SQLLOG_WARNING()<<error;
            }
            db.close();
        }
        else if (db.lastError().isValid())
        {
            SQLLOG_WARNING()<<db.lastError().text();
        }
        db.setDatabaseName(profile.name);
    }
    else if(profile.type=="QSQLITE")
    {
        // :memory:
        if(profile.name==":memory:")
        {
            db.setDatabaseName(profile.name);
        }
        else
        {
            const QString path = profile.path;
            const QString file = QString("%1.sqlite").arg(profile.name);
            const QString filePath = QDir(path).filePath(file);
            db.setDatabaseName(filePath);
        }
    }
    else
    {
        SQLLOG_CRITICAL()<<"Unsupported db type:"<<profile.type;
        QSqlDatabase::removeDatabase(threadConnection);
        return QSqlDatabase();
    }

    return db;
}

bool SqlDbPool::closeDatabase(const QString& connection)
{
    const QStringList conectionNames = QSqlDatabase::connectionNames();
    for(const QString& conectionName: conectionNames)
    {
        if(conectionName.startsWith(connection))
        {
            QSqlDatabase::database(conectionName).close();
            QSqlDatabase::removeDatabase(conectionName);
        }
    }
    if(QSqlDatabase::contains(connection))
    {
        QSqlDatabase::database(connection).close();
        QSqlDatabase::removeDatabase(connection);
    }

    return true;
}

bool SqlDbPool::closeThread(const QString& suffix)
{
    if(!qApp)
        return true;

    const QStringList conectionNames = QSqlDatabase::connectionNames();
    for(const QString& conectionName: conectionNames)
    {
        if(conectionName.endsWith(suffix))
        {
            QSqlDatabase::database(conectionName).close();
            QSqlDatabase::removeDatabase(conectionName);
        }
    }

    return true;
}

QString SqlDbPool::connectionName(const QString& connection)
{
    if(QThread::currentThread()==qApp->thread())
    {
        return connection;
    }

    const QString thread = pointerString(QThread::currentThread());
    const QString threadConnection = QString("%1_%2").arg(connection, thread);

    return threadConnection;
}

QSqlDatabase SqlDbPool::database(const QString& connection, bool open)
{
    if(QThread::currentThread()==qApp->thread())
    {
        return QSqlDatabase::database(connection, open);
    }

    const QString thread = pointerString(QThread::currentThread());
    const QString threadConnection = QString("%1_%2").arg(connection, thread);

    if(QSqlDatabase::contains(threadConnection))
        return QSqlDatabase::database(threadConnection);

    if (!s_connections.hasLocalData()) {
        s_connections.setLocalData(new SqlDbConnection(thread));
    }

    QSqlDatabase clone = QSqlDatabase::cloneDatabase(connection, threadConnection);

    if (open && !clone.open())
    {
        SQLLOG_WARNING()<<clone.lastError().text();
    }
    else
    {
        SQLLOG_DEBUG()<<"Open asynchronous database connection:"<<threadConnection;
    }

    return clone;
}

QSqlDatabase SqlDbPool::defaultDatabase(bool open)
{
    return database(SqlDefaultConnection, open);
}

QSqlDriver* SqlDbPool::driver(const QString& connection)
{
    return SqlDbPool::database(connection).driver();
}

QSqlDriver* SqlDbPool::defaultDriver()
{
    return SqlDbPool::driver(SqlDefaultConnection);
}

QStringList SqlDbPool::drivers()
{
    return QSqlDatabase::drivers();
}
