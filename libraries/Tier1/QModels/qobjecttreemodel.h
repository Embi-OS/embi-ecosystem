#ifndef QOBJECTTREEMODEL_H
#define QOBJECTTREEMODEL_H

#include "qobjectlistmodel.h"
#include <QDefs>

class QObjectTreeModel;
class QTreeObject: public QObject,
                   public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(TreeObject)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY completedChanged FINAL)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_CONSTANT_OLP_PROPERTY(QTreeObject, content)
    Q_DEFAULT_PROPERTY(content)

public:
    explicit QTreeObject(QObject* parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

    bool isCompleted() const;

    int count() const { return m_content.count(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

    int position() const;
    QModelIndex index() const;
    QTreeObject* parentObject() const;
    QObjectTreeModel* parentModel() const;
    void setParentModel(QObjectTreeModel* parentModel);

    bool forEach(std::function<bool(QTreeObject* object)> callback, Qt::FindChildOption options=Qt::FindChildrenRecursively) const;

signals:
    void completedChanged(bool completed);

    void countChanged(int count);
    void emptyChanged(bool empty);
    void childrenDataChanged(QTreeObject* object, int signalIndex);

    void objectInserted(QTreeObject* object, int index);
    void objectRemoved(QTreeObject* object, int index);

    void treeObjectInserted(QTreeObject* object, QObject* parent, int index);
    void treeObjectRemoved(QTreeObject* object, QObject* parent, int index);

protected slots:
    void onObjectPropertyChanged();
    void countInvalidate();

protected:
    virtual void onObjectInserted(QTreeObject* object, int index);
    virtual void onObjectRemoved(QTreeObject* object, int index);

    bool m_completed=false;

private:
    void connectObjectProperty(QTreeObject* object);

    void objectInsertedNotify(QTreeObject* object, int index);
    void objectRemovedNotify(QTreeObject* object, int index);

    int m_count=0;
    QTreeObject* m_parentObject;
    QObjectTreeModel* m_parentModel;
    QMetaMethod m_handler;
};

class QObjectTreeModel: public QObjectListModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ObjectTreeModel)

    friend QTreeObject;
public:
    explicit QObjectTreeModel(QObject* parent = nullptr,
                              const QMetaObject* metaObject = &QTreeObject::staticMetaObject,
                              const QByteArrayList& exposedRoles = {},
                              const QByteArrayList& hiddenRoles = {});

    int rowCount(const QModelIndex& parent=QModelIndex()) const override;
    int columnCount(const QModelIndex& parent=QModelIndex()) const override;
    QModelIndex index(int row, int column=0, const QModelIndex &parent=QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index=QModelIndex()) const override;
    bool hasChildren(const QModelIndex& parent=QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_INVOKABLE bool clearObject(QTreeObject* parent);
    Q_INVOKABLE bool removeObject(QTreeObject* object, QTreeObject* parent);
    Q_INVOKABLE bool insertObject(QTreeObject* object, QTreeObject* parent, int pos=-1);
    Q_INVOKABLE bool insertObjects(QList<QTreeObject*> objects, QTreeObject* parent, int pos=-1);

    bool forEach(std::function<bool(QTreeObject* object)> callback, Qt::FindChildOption options=Qt::FindChildrenRecursively) const;
    Q_INVOKABLE QTreeObject* findChildFromProperty(const QString& property, const QVariant& value, Qt::FindChildOption options=Qt::FindChildrenRecursively) const;

signals:
    void treeObjectInserted(QTreeObject* object, QObject* parent, int index);
    void treeObjectRemoved(QTreeObject* object, QObject* parent, int index);

protected slots:
    void onChildrenDataChanged(QTreeObject* object, int signalIndex);

protected:
    void onObjectInserted(QObject* object, int index) override;
    void onObjectRemoved(QObject* object, int index) override;

    virtual void onTreeObjectInserted(QTreeObject* object, QObject* parent, int index);
    virtual void onTreeObjectRemoved(QTreeObject* object, QObject* parent, int index);

private:
    void treeObjectInsertedNotify(QTreeObject* object, QObject* parent, int index);
    void treeObjectRemovedNotify(QTreeObject* object, QObject* parent, int index);
};

#endif // QOBJECTTREEMODEL_H
