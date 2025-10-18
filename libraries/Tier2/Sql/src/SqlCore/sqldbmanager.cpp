#include "sqldbmanager.h"
#include "sql_log.h"
#include "sqldbpool.h"
#include "sqlquerybuilder.h"

#include <QtConcurrentRun>

SqlDbManager::SqlDbManager(QObject *parent) :
    QObject(parent)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());
}

QDeferred<QSqlError> SqlDbManager::testConnexion(const SqlDbProfile& profile)
{
    QDeferred<QSqlError> defer;

    QFuture<QSqlError> future = QtConcurrent::run([defer](SqlDbProfile profile) mutable {
        profile.connectionName = "sql_test_connexion";
        QSqlDatabase db = SqlDbPool::create(profile);
        if(db.open())
        {
            db.close();
            QSqlDatabase::removeDatabase(profile.connectionName);
            defer.resolve(QSqlError());
        }
        else
        {
            SQLLOG_CRITICAL()<<"Echec ouverture de la base de donnée:"<<db.connectionName();
            defer.reject(db.lastError());
        }
        return db.lastError();
    }, profile);

    return defer;
}

QDefer SqlDbManager::dropDatabase(const QString& connectionName)
{
    QDefer defer;

    QFuture<bool> future = QtConcurrent::run([defer, connectionName]() mutable {
        bool result = false;
        QSqlDriver::DbmsType dbType = SqlDbPool::driver(connectionName)->dbmsType();
        QString dbName = SqlDbPool::database(connectionName, true).databaseName();
        SqlDbPool::closeDatabase(connectionName);
        if(dbType==QSqlDriver::SQLite && QFileInfo::exists(dbName))
            result = QFile::remove(dbName);
        defer.end(result);
        return result;
    });

    return defer;
}

bool SqlDbManager::open(SqlDbPreparator* preparator)
{
    if(!preparator) {
        SQLLOG_CRITICAL()<<"No preparator has been given to the database";
        return false;
    }

    if(!preparator->isValid()) {
        SQLLOG_CRITICAL()<<"Preparator is invalid";
        return false;
    }

    if(!openConnection(preparator)) {
        SQLLOG_CRITICAL()<<"Failed to open database";
        return false;
    }

    if(!createDatabase(preparator)) {
        SQLLOG_CRITICAL()<<"Failed to create database";
        return false;
    }

    return true;
}

bool SqlDbManager::close(const QString& connectionName)
{
    if(!SqlDbPool::closeDatabase(connectionName)) {
        SQLLOG_CRITICAL()<<"Failed to close connection"<<connectionName;
        return false;
    }

    return true;
}

bool SqlDbManager::openConnection(const SqlDbProfile& profile)
{
    bool result = SqlDbPool::init(profile);

    if(!result)
    {
        SQLLOG_CRITICAL().noquote()<<"Failed to init async connection:"<<profile.toString();
    }
    else
    {
        QSqlQuery query = SqlBuilder::version().connection(profile.connectionName).exec();
        const QString version = SqlBuilder::value(query,0,"VERSION").toString();
        SQLLOG_INFO()<<"Database"<<profile.connectionName<<"Version:"<<version;
    }

    return result;
}

bool SqlDbManager::openConnection(SqlDbPreparator* preparator)
{
    const SqlDbProfile profile = preparator->createConnectionProfile();
    return openConnection(profile);
}

bool SqlDbManager::createDatabase(SqlDbPreparator* preparator)
{
    if(preparator==nullptr) {
        return false;
    }

    QDefer defer;
    QLambdaThreadWorker* worker = new QLambdaThreadWorker;
    defer.complete([worker](bool) { delete worker; });

    preparator->setParent(nullptr);
    worker->moveQObjectToThread(preparator);
    worker->execInThread([defer, preparator]() mutable
    {
        bool result = preparator->execute();
        SqlDbPool::closeDatabase(SqlDbPool::connectionName(preparator->getConnectionName()));
        preparator->moveToThread(qApp->thread());
        defer.end(result);
    });

    bool result = defer.await();
    preparator->setParent(nullptr);

    return result;
}
