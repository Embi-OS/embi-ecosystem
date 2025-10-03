#ifndef RESTARRAYMODEL_H
#define RESTARRAYMODEL_H

#include <QDefs>
#include "rest_helpertypes.h"

#include "qvariantreadermodel.h"

class QSWorker;
class RestArrayModel: public QVariantReaderModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_VAR_PROPERTY(bool, manual, Manual, false)
    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_READONLY_VAR_PROPERTY(bool, loading, Loading, false)

    // Specify basic fetch parameters
    // QVariantReaderModel::roles
    // QVariantReaderModel::placeholder
    Q_WRITABLE_REF_PROPERTY(QString, baseName, BaseName, {})
    Q_WRITABLE_REF_PROPERTY(QString, primaryField, PrimaryField, {})
    Q_WRITABLE_REF_PROPERTY(QString, connection, Connection, RestHelper::defaultConnection)

    // Specify filters parameters
    Q_WRITABLE_VAR_PROPERTY(bool, filterEnabled, FilterEnabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, filterInverted, FilterInverted, false)
    Q_WRITABLE_REF_PROPERTY(QString, filterRoleName, FilterRoleName, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, filterValue, FilterValue, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, filterMap, FilterMap, {})

    // Specify sorting fields
    Q_WRITABLE_VAR_PROPERTY(bool, sortEnabled, SortEnabled, true)
    Q_WRITABLE_REF_PROPERTY(QString, sortRoleName, SortRoleName, "")
    Q_WRITABLE_VAR_PROPERTY(Qt::SortOrder, sortOrder, SortOrder, Qt::AscendingOrder)
    Q_WRITABLE_REF_PROPERTY(QVariantMap, sortMap, SortMap, {})

    // Specify limit/offset
    Q_WRITABLE_VAR_PROPERTY(int, limit, Limit, 0)
    Q_WRITABLE_VAR_PROPERTY(int, offset, Offset, 0)

    // Specify parameters
    Q_WRITABLE_REF_PROPERTY(QVariantMap, headers, Headers, {})
    Q_WRITABLE_REF_PROPERTY(QVariantMap, parameters, Parameters, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, fields, Fields, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, omit, Omit, {})
    Q_WRITABLE_REF_PROPERTY(QStringList, expand, Expand, {})

    // Status and Errors
    Q_READONLY_VAR_PROPERTY(int, download, Download, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, progress, Progress, 0) // 0 to 10000

public:
    explicit RestArrayModel(QObject* parent = nullptr);
    explicit RestArrayModel(const QString& name, QObject *parent = nullptr);

    Q_INVOKABLE QVariantMap selectParameters() const;

public slots:
    void queueInvalidate();
    void invalidate();
    void cancel();

signals:
    void invalidated(bool result);

protected:
    bool runSelectWorker(QSWorker* worker);

    QSWorker* createSelectWorker();

    QPointer<QSWorker> m_selectWorker;

    bool m_invalidateQueued = false;
};

#endif // RESTARRAYMODEL_H
