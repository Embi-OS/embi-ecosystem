#ifndef RESTMAPPER_H
#define RESTMAPPER_H

#include <QDefs>
#include "rest_helpertypes.h"

#include "qvariantmapper.h"

class QSWorker;
class RestMapper: public QVariantMapper
{
    Q_OBJECT
    QML_ELEMENT

    // Specify basic fetch parameters
    // QVariantMapper::baseName
    // QVariantMapper::roles
    // QVariantMapper::placeholder
    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, RestHelper::defaultConnection)

    // Specify method
    Q_WRITABLE_REF_PROPERTY(QString, method, Method, "")
    Q_WRITABLE_REF_PROPERTY(QVariantMap, body, Body, {})

    // Specify parameters
    Q_WRITABLE_REF_PROPERTY(QVariantMap, headers, Headers, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, parameters, Parameters, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, expand, Expand, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, omit, Omit, {})

    // Status and Errors
    Q_READONLY_VAR_PROPERTY(double, upload, Upload, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(double, download, Download, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, progress, Progress, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, code, Code, 0)
    Q_READONLY_REF_PROPERTY(QString, error, Error, {})
    Q_READONLY_REF_PROPERTY(QString, message, Message, {})

    Q_READONLY_VAR_PROPERTY(bool, exists, Exists, false)

public:
    explicit RestMapper(QObject *parent = nullptr);
    explicit RestMapper(const QString& baseName, QObject *parent = nullptr, const QMetaObject* blacklistedMetaObject = &RestMapper::staticMetaObject);
    ~RestMapper() override;

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

#endif // RESTMAPPER_H
