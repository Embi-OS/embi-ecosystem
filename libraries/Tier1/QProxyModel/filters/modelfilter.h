#ifndef MODELFILTER_H
#define MODELFILTER_H

#include "rolefilter.h"
#include <QVariant>
#include <QAbstractItemModel>
#include "qproxymodel.h"

namespace QQsfpm {

class ModelFilter : public QQsfpm::RoleFilter
{
    Q_OBJECT

    Q_WRITABLE_VAR_PROPERTY(bool, delayed, Delayed, false)
    Q_WRITABLE_REF_PROPERTY(QString, modelRoleName, ModelRoleName, {})
    Q_WRITABLE_VAR_PROPERTY(int, modelRole, ModelRole, -1)
    Q_WRITABLE_PTR_PROPERTY(QAbstractItemModel, model, Model, nullptr)

public:
    explicit ModelFilter(QObject *parent = nullptr);

    void proxyModelCompleted(const QQmlSortFilterProxyModel& proxyModel) override;

protected:
    bool filterRow(const QModelIndex &sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const override;

private slots:
    void onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel);
    void onModelChanged(QAbstractItemModel* model);

private:
    QProxyModel* m_proxyModel = nullptr;
};

}

#endif // MODELFILTER_H
