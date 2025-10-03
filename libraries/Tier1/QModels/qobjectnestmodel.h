#ifndef QOBJECTNESTMODEL_H
#define QOBJECTNESTMODEL_H

#include "qobjectlistmodel.h"
#include <QDefs>

class QObjectNestModel: public QObjectListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ObjectNestModel)

    Q_WRITABLE_REF_PROPERTY(QString, text, Text, "")
    Q_WRITABLE_REF_PROPERTY(QVariant, value, Value, {})

public:
    explicit QObjectNestModel(QObject* parent = nullptr,
                              const QMetaObject* metaObject = &QObjectNestModel::staticMetaObject,
                              const QByteArrayList& exposedRoles = {},
                              const QByteArrayList& hiddenRoles = {});

    int rowCount(const QModelIndex& parent=QModelIndex()) const override;
    int columnCount(const QModelIndex& parent=QModelIndex()) const override;
    QModelIndex index(int row, int column=0, const QModelIndex &parent=QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index=QModelIndex()) const override;
    bool hasChildren(const QModelIndex& parent=QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QModelIndex child(const QModelIndex& index) const;

    int position() const;
    QObjectNestModel* parentObject() const;
    QObjectNestModel* parentModel() const;
    void setParentModel(QObjectNestModel* parentModel);

    bool forEach(std::function<bool(QObjectNestModel* object)> callback, Qt::FindChildOption options=Qt::FindChildrenRecursively) const;
    Q_INVOKABLE QObjectNestModel* findChildFromProperty(const QString& property, const QVariant& value, Qt::FindChildOption options=Qt::FindChildrenRecursively) const;

protected slots:
    void onChildrenModelAboutToBeReset();
    void onChildrenModelReset();
    void onChildrenDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles = QList<int>());
    void onChildrenRowsAboutToBeInserted(const QModelIndex &parent, int first, int last);
    void onChildrenRowsInserted(const QModelIndex &parent, int first, int last);
    void onChildrenRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void onChildrenRowsRemoved(const QModelIndex &parent, int first, int last);
    void onChildrenRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);
    void onChildrenRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow);

    // ──────── OVERRIDE OBSERVER API ──────────
protected:
    void onObjectInserted(QObject* object, int index) override;
    void onObjectRemoved(QObject* object, int index) override;

    // ──────── ATTRIBUTES ──────────
private:
    QObjectNestModel* m_parentObject;
    QObjectNestModel* m_parentModel;
};

#endif // QOBJECTNESTMODEL_H
