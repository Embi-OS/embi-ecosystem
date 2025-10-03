#ifndef SQLMAPPER_H
#define SQLMAPPER_H

#include <QDefs>
#include "sql_helpertypes.h"
#include "SqlCore/sqlquerybuilder.h"

#include "qvariantmapper.h"

class QSWorker;
class SqlMapper: public QVariantMapper
{
    Q_OBJECT
    QML_ELEMENT

    // Specify basic fetch parameters
    // QVariantMapper::baseName
    // QVariantMapper::primaryField
    // QVariantMapper::roles
    // QVariantMapper::placeholder
    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, SqlDefaultConnection)

    // Specify method
    Q_WRITABLE_REF_PROPERTY(QString, methodField, MethodField, {})
    Q_WRITABLE_REF_PROPERTY(QVariant, methodValue, MethodValue, {})

    // Specify parameters
    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, omit, Omit, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, expand, Expand, {})

    // Specify joins
    Q_WRITABLE_REF_PROPERTY(QList<SqlJoinQuery>, joins, Joins, {})

    // Status and Errors
    Q_READONLY_VAR_PROPERTY(double, upload, Upload, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(double, download, Download, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, progress, Progress, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, code, Code, 0)
    Q_READONLY_REF_PROPERTY(QString, error, Error, {})
    Q_READONLY_REF_PROPERTY(QString, message, Message, {})

    Q_READONLY_VAR_PROPERTY(bool, exists, Exists, false)

public:
    explicit SqlMapper(QObject *parent = nullptr);
    explicit SqlMapper(const QString& baseName, QObject *parent = nullptr, const QMetaObject* blacklistedMetaObject = &SqlMapper::staticMetaObject);
    ~SqlMapper() override;

    QSWorker* selectWorker() const;
    QSWorker* submitWorker() const;

signals:
    void error();

protected:
    bool doSelect() override final;
    bool doSubmit(const QStringList& dirtyKeys) override final;

    bool runSelectWorker(QSWorker* worker);
    bool runSubmitWorker(QSWorker* worker);

    QSWorker* createSelectWorker();
    QSWorker* createSubmitWorker();

    QPointer<QSWorker> m_selectWorker;
    QPointer<QSWorker> m_submitWorker;
};

#endif // SQLMAPPER_H
