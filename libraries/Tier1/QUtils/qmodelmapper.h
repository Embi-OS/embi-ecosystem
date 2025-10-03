#ifndef QMODELMAPPER_H
#define QMODELMAPPER_H

#include "qvariantmapper.h"
#include <QAbstractItemModel>

class QModelMapper: public QVariantMapper
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ModelMapper)

    Q_WRITABLE_PTR_PROPERTY(QAbstractItemModel, model, Model, nullptr)
    Q_WRITABLE_REF_PROPERTY(QModelIndex, parentIndex, ParentIndex, QModelIndex())
    Q_WRITABLE_VAR_PROPERTY(int, row, Row, -1)
    Q_WRITABLE_VAR_PROPERTY(int, column, Column, 0)
    Q_WRITABLE_VAR_PROPERTY(int, role, Role, -1)
    Q_WRITABLE_REF_PROPERTY(QString, roleName, RoleName, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, value, Value, {})
    Q_WRITABLE_VAR_PROPERTY(Qt::MatchFlags, flags, Flags, Qt::MatchExactly)

    Q_PROPERTY(QModelIndex modelIndex READ getModelIndex NOTIFY modelIndexChanged FINAL)

public:
    explicit QModelMapper(QObject *parent = nullptr);
    explicit QModelMapper(const QString& baseName, QObject *parent = nullptr);
    ~QModelMapper() override;

    const QModelIndex& getModelIndex() const;

protected:
    bool doSelect() final override;
    bool doSubmit(const QStringList& dirtyKeys) final override;

signals:
    void modelIndexChanged(const QModelIndex& modelIndex);

private slots:
    void onModelAboutToChange(QAbstractItemModel* oldModel, QAbstractItemModel* newModel);
    void onModelChanged(QAbstractItemModel* model);

    void onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles);
    void onModelRowsInserted(const QModelIndex& parent, int first, int last);
    void onModelRowsRemoved(const QModelIndex& parent, int first, int last);
    void onModelColumnsInserted(const QModelIndex& parent, int first, int last);
    void onModelColumnsRemoved(const QModelIndex& parent, int first, int last);

    void invalidateModelIndex();
    void updateModelIndex();
    void updateRoles();

private:

    QVariantMap selectModel(bool* result=nullptr);
    QVariantMap fromModel(bool* result=nullptr);

    bool submitModel(const QStringList& dirtyKeys);
    bool toModel(const QVariantMap& map) const;

    QModelIndex m_modelIndex;
};

#endif // QMODELMAPPER_H
