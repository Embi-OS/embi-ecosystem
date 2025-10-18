#ifndef SQLSUBMITWORKER_H
#define SQLSUBMITWORKER_H

#include "sql_helpertypes.h"
#include "SqlCore/sqlquerybuilder.h"
#include "syncable/qsworker.h"
class SqlSubmitWorker : public QSWorker
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, asynchronous, Asynchronous, true)
    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)
    Q_WRITABLE_REF_PROPERTY(QString, tableName, TableName, "")
    Q_WRITABLE_REF_PROPERTY(QString, primaryField, PrimaryField, "")

    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, omit, Omit, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, expand, Expand, {})

    Q_WRITABLE_REF_PROPERTY(QList<SqlJoinQuery>, joins, Joins, {})

    Q_WRITABLE_REF_PROPERTY(QVariantList, source, Source, {})
    Q_WRITABLE_REF_PROPERTY(QVariantList, destination, Destination, {})

public:
    explicit SqlSubmitWorker(QObject *parent = nullptr);

public slots:
    bool doRun() final override;
    bool abort() final override;
    bool waitForFinished(int timeout = -1, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents) final override;
};

#endif // SQLSUBMITWORKER_H
