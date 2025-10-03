#ifndef RESTMODEL_H
#define RESTMODEL_H

#include <QDefs>
#include "rest_helpertypes.h"

#include "syncable/qspagination.h"
#include "qvariantlistmodel.h"

class QSWorker;
class RestModel: public QVariantListModel
{
    Q_OBJECT
    QML_ELEMENT

    // Specify basic fetch parameters
    // QVariantListModel::baseName
    // QVariantListModel::primaryField
    // QVariantListModel::roles
    // QVariantListModel::placeholder
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

    // Specify pagination
    Q_WRITABLE_PTR_PROPERTY(QSPagination, pagination, Pagination, nullptr)

    // Status and Errors
    Q_READONLY_VAR_PROPERTY(int, upload, Upload, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, download, Download, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, progress, Progress, 0) // 0 to 10000
    Q_READONLY_VAR_PROPERTY(int, code, Code, 0)
    Q_READONLY_REF_PROPERTY(QString, error, Error, {})
    Q_READONLY_REF_PROPERTY(QString, message, Message, {})

public:
    explicit RestModel(QObject* parent = nullptr);
    explicit RestModel(const QString& name, QObject *parent = nullptr);
    ~RestModel() override;

    Q_INVOKABLE QVariantMap selectParameters() const;
    Q_INVOKABLE QVariantMap submitParameters() const;

public slots:
    void resize();

signals:
    void error();

protected slots:
    void onPaginationAboutToChange(QSPagination* oldPagination, QSPagination* newPagination);
    void onPaginationChanged(QSPagination* pagination);

protected:
    bool doSelect() override final;
    bool doSubmit() override final;
    bool doCancel() override final;

    bool runSelectWorker(QSWorker* worker);
    bool runSubmitWorker(QSWorker* worker);

    QSWorker* createSelectWorker();
    QSWorker* createSubmitWorker();

    QPointer<QSWorker> m_selectWorker;
    QPointer<QSWorker> m_submitWorker;
};

#endif // RESTMODEL_H
