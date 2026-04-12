#include "happymigration.h"
#include "happy_log.h"

#include "utils/datetimeutils.h"

HappyMigration::HappyMigration(QObject *parent) :
    QObject(parent)
{

}

bool HappyMigration::run(HappyServer* happyServer)
{
    QElapsedTimer timer;
    timer.start();

    m_happyServer = happyServer;

    QList<MigrationObject> migrations;
    const auto *metaObj = metaObject();
    for (int idx = metaObj->methodOffset(); idx < metaObj->methodCount(); idx++)
    {
        QMetaMethod method = metaObj->method(idx);
        if (method.methodType() == QMetaMethod::Slot && method.access() == QMetaMethod::Private)
        {
            if (method.parameterCount() > 0) {
                HAPPYLOG_WARNING()<<"HappyMigration private slots requires no parameter";
                continue;
            }

            if (method.returnMetaType().id()!=QMetaType::Bool) {
                HAPPYLOG_WARNING()<<"HappyMigration private slots return must be boolean";
                continue;
            }

            const QString methodName = method.name();
            QStringList list = methodName.split("_");

            MigrationObject migration;
            migration.app = list.takeFirst().toLower();
            migration.name = ::camelToSnake(list.join("_")).toLower();
            migration.method = method;

            QVariantMap map;
            map.insert("app", migration.app);
            map.insert("name", migration.name);

            QSqlQuery selectReply = SqlBuilder::select("*").from(HAPPY_MIGRATIONS_TABLE).where(map).forwardOnly().trust().exec();
            if(selectReply.seek(0))
                continue;

            migrations.append(migration);
        }
    }

    if(migrations.isEmpty())
        return true;

    bool result = true;
    for(const MigrationObject& migration: migrations)
    {
        if(runMigration(migration))
        {
            QVariantMap map;
            map.insert("app", migration.app);
            map.insert("name", migration.name);
            map.insert("applied", QDateTime::currentDateTime());
            SqlBuilder::insert(map).into(HAPPY_MIGRATIONS_TABLE).exec();
        }
        else
        {
            result = false;
        }
    }

    HAPPYLOG_INFO().noquote()<<"HappyMigration run succesfully in"<<DateTimeUtils::formatDuration(timer.elapsed(), DurationFormatOptions::ShowMilliseconds);

    return result;
}

bool HappyMigration::runMigration(const MigrationObject& migration)
{
    if (!migration.method.isValid())
        return false;

    QElapsedTimer timer;
    timer.start();

    bool ret;
    void *argv[] = {&ret};
    QMetaObject::metacall(const_cast<HappyMigration*>(this), QMetaObject::InvokeMetaMethod, migration.method.methodIndex(), argv);

    if(!ret)
    {
        HAPPYLOG_CRITICAL()<<"HappyMigration"<<migration.app<<migration.name<<"failed";
        return false;
    }

    HAPPYLOG_INFO().noquote()<<"HappyMigration"<<migration.app<<migration.name<<"applied succesfully in"<<DateTimeUtils::formatDuration(timer.elapsed(), DurationFormatOptions::ShowMilliseconds);

    return true;
}
