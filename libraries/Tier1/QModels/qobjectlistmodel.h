#ifndef QOBJECTLISTMODEL_H
#define QOBJECTLISTMODEL_H

#include <QAbstractListModel>
#include <QQmlListProperty>
#include <QQmlParserStatus>
#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlListProperty>
#include <QtQml/QJSValue>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQmlComponent>

#include <QDefs>

#include "qobjectlistproperty.h"
#include "qmodels_log.h"

template <typename T>
class QObjectListModelIterator;
class QObjectListModelAttached;
class QObjectListModel : public QAbstractItemModel,
                         public QQmlParserStatus
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ObjectListModel)
    QML_ATTACHED(QObjectListModelAttached)
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool isCompleted READ isCompleted NOTIFY completedChanged FINAL)

    Q_PROPERTY(int count READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int length READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int size READ count NOTIFY countChanged FINAL)
    Q_PROPERTY(int isEmpty READ isEmpty NOTIFY emptyChanged FINAL)

    Q_CONSTANT_OLP_PROPERTY(QObject, content)
    Q_DEFAULT_PROPERTY(content)

    // ──────── CONSTRUCTOR ──────────
public:
    explicit QObjectListModel(QObject* parent = nullptr,
                              const QMetaObject* metaObject = nullptr,
                              const QByteArrayList& exposedRoles = {},
                              const QByteArrayList& hiddenRoles = {});

    static QObjectListModelAttached* qmlAttachedProperties(QObject* object);

    // ──────── QQmlParserStatus OVERRIDE ──────────
public:
    void classBegin() override;
    void componentComplete() override;

    bool isCompleted() const;

    // ──────── ABSTRACT MODEL OVERRIDE ──────────
public:
    bool setData(const QModelIndex& modelIndex, const QVariant& value, int role) override final;
    QVariant data(const QModelIndex& modelIndex, int role) const override final;

    QHash<int, QByteArray> roleNames() const override final;
    int rowCount(const QModelIndex& parent=QModelIndex()) const override;
    int columnCount(const QModelIndex& parent=QModelIndex()) const override;
    QModelIndex index(int row, int column=0, const QModelIndex &parent=QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index=QModelIndex()) const override;
    bool hasChildren(const QModelIndex& parent=QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void sort(int role, Qt::SortOrder order = Qt::AscendingOrder) override;
    void sort(QObjectList& storage, int role, Qt::SortOrder order = Qt::AscendingOrder) const;
    void sort(QObjectList& storage, const QString& roleName, Qt::SortOrder order = Qt::AscendingOrder) const;

    // ──────── PUBLIC C++ API ──────────
public:
    template <typename T>
    T* operator[] (const int index) const;
    template <typename T>
    T* at(int index) const;
    template <typename T>
    T* get(int index) const;

    bool contains(const QObject* object) const;
    int indexOf(const QObject* object) const;
    bool append(QObject* object);
    bool prepend(QObject* object);
    bool insert(int index, QObject* object);
    bool remove(const QObject* object);
    bool remove(int index, int count = 1);
    bool replace(int index, QObject* object);

    template <typename T>
    bool append(const QList<T*>& objectList);
    template <typename T>
    bool prepend(const QList<T*>& objectList);
    template <typename T>
    bool insert(int idx, const QList<T*>& itemList);
    template <typename T>
    bool remove(const QList<T*>& objects);
    template <typename T>
    T* takeAt(int index);

    template <typename T>
    T* first() const;
    template <typename T>
    T* last() const;

    Q_INVOKABLE bool isFirst(const QObject* object) const;
    Q_INVOKABLE bool isLast(const QObject* object) const;

    template <typename T>
    QObjectListModelIterator<T> modelIterator() const;

    template <typename T>
    bool setObjects(const QList<T*>& objectList);
    const QObjectList& objects() const;
    const QMetaObject* templateMetaObject() const;
    const QMultiHash<int, int>& signalIdxToRole() const;
    const QSet<int>& signalIdxs() const;

    int count() const { return m_objects.count(); };
    int size() const { return count(); };
    int length() const { return count(); };
    bool isEmpty() const { return count() == 0; };

    static const QMetaObject& emptyMetaObject();
    static const QString& emptyStr();
    static const QByteArray& emptyBA();
    static const QModelIndex& noParent();

    Q_INVOKABLE QString templateClassName() const;
    Q_INVOKABLE bool isMetaObjectCompatible(const QMetaObject* metaObject) const;
    Q_INVOKABLE bool isObjectCompatible(const QObject* object) const;
    Q_INVOKABLE int roleForName(const QString& name) const;
    Q_INVOKABLE QByteArray roleName(int role) const;

    // ──────── OVERRIDE OBSERVER API ──────────
protected:
    virtual void onObjectAboutToBeInserted(QObject* object, int index);
    virtual void onObjectInserted(QObject* object, int index);
    virtual void onObjectAboutToBeMoved(QObject* object, int src, int dest);
    virtual void onObjectMoved(QObject* object, int src, int dest);
    virtual void onObjectAboutToBeRemoved(QObject* object, int index);
    virtual void onObjectRemoved(QObject* object, int index);

    // ──────── PUBLIC API ──────────
protected:
    Q_INVOKABLE QObject* at(QJSValue index) const;
    Q_INVOKABLE QObject* get(QJSValue index) const;
    Q_INVOKABLE QObject* takeAt(QJSValue index) const;
    Q_INVOKABLE int indexOf(QJSValue object) const;
    Q_INVOKABLE bool contains(QJSValue object) const;

    Q_INVOKABLE bool append(QJSValue object);
    Q_INVOKABLE bool prepend(QJSValue item);
    Q_INVOKABLE bool insert(int index, QJSValue item);
    Q_INVOKABLE bool remove(QJSValue value);

public slots:
    virtual bool clear();

    bool move(int index, int to);
    bool moveDown(const int index);
    bool moveUp(const int index);

    bool moveNext(const int index) { return moveDown(index); };
    bool movePrevious(const int index) { return moveUp(index); };

signals:
    void completedChanged(bool completed);

    void countChanged(int count);
    void emptyChanged(bool empty);
    void objectsChanged(const QObjectList& objects);

    void objectInserted(QObject* object, int index);
    void objectRemoved(QObject* object, int index);
    void objectMoved(QObject* object, int from, int to);

    // ──────── ITERATOR ──────────
public:
    using iterator = typename QObjectList::const_iterator;
    using const_iterator = typename QObjectList::const_iterator;
    iterator begin() { return m_objects.begin(); }
    iterator end() { return m_objects.end(); }
    const_iterator begin() const { return m_objects.begin(); }
    const_iterator end() const { return m_objects.end(); }
    const_iterator cbegin() const { return m_objects.cbegin(); }
    const_iterator cend() const { return m_objects.cend(); }
    const_iterator constBegin() const { return m_objects.constBegin(); }
    const_iterator constEnd() const { return m_objects.constEnd(); }

    using reverse_iterator = typename QObjectList::const_reverse_iterator;
    using const_reverse_iterator = typename QObjectList::const_reverse_iterator;
    const_reverse_iterator rbegin() const { return m_objects.rbegin(); }
    const_reverse_iterator rend() const { return m_objects.rend(); }
    reverse_iterator rbegin() { return m_objects.rbegin(); }
    reverse_iterator rend() { return m_objects.rend(); }
    const_reverse_iterator crbegin() const { return m_objects.crbegin(); }
    const_reverse_iterator crend() const { return m_objects.crend(); }

    // ──────── PUBLIC OBSERVER API ──────────
public:
    template <typename T>
    QMetaObject::Connection onInserted(QObject* receiver, std::function<void(T* object, int index)> callback);
    template <typename T>
    QMetaObject::Connection onInserted(QObject* receiver, std::function<void(T* object)> callback);
    template <typename T>
    QMetaObject::Connection onInserted(std::function<void(T* object, int index)> callback);
    template <typename T>
    QMetaObject::Connection onInserted(std::function<void(T* object)> callback);

    template <typename T>
    QMetaObject::Connection onRemoved(QObject* receiver, std::function<void(T* object, int index)> callback);
    template <typename T>
    QMetaObject::Connection onRemoved(QObject* receiver, std::function<void(T* object)> callback);
    template <typename T>
    QMetaObject::Connection onRemoved(std::function<void(T* object, int index)> callback);
    template <typename T>
    QMetaObject::Connection onRemoved(std::function<void(T* object)> callback);

    template <typename T>
    QMetaObject::Connection onMoved(QObject* receiver, std::function<void(T* object, int from, int to)> callback);
    template <typename T>
    QMetaObject::Connection onMoved(std::function<void(T* object, int from, int to)> callback);

private slots:
    void onObjectDestroyed(QObject* item);
    void onObjectPropertyChanged();
    void countInvalidate();

protected:
    void dataChangedNotify(const QModelIndex& index, int signalIndex);

    void referenceObject(QObject* item);
    void dereferenceObject(QObject* item, bool deleteItem=true);

    QVariant readProperty(const QObject* object, int propertyIndex) const;
    bool writeProperty(QObject *object, int propertyIndex, const QVariant &value) const;

    void objectAboutToBeInsertedNotify(QObject* object, int index);
    void objectInsertedNotify(QObject* object, int index);
    void objectAboutToBeMovedNotify(QObject* object, int src, int dest);
    void objectMovedNotify(QObject* object, int src, int dest);
    void objectAboutToBeRemovedNotify(QObject* object, int index);
    void objectRemovedNotify(QObject* object, int index);

    bool m_completed=false;
    QObjectList m_objects;

private:
    // ──────── ATTRIBUTES ──────────
    int m_count=0;
    const QMetaObject* m_metaObj;
    QString m_templateClassName;
    QMetaMethod m_handler;
    QHash<int, QByteArray> m_roleNames;
    QHash<QString, int> m_roleIds;
    QMultiHash<int, int> m_signalIdxToRole;
    QSet<int> m_signalIdxs;
};

template <typename T>
QObjectListModelIterator<T> QObjectListModel::modelIterator() const
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::iterator",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());
    return QObjectListModelIterator<T>(this);
}

template <typename T>
T* QObjectListModel::operator[] (const int index) const
{
    return get<T>(index);
}

template <typename T>
T* QObjectListModel::at(int index) const
{
    return get<T>(index);
}
template <typename T>
T* QObjectListModel::get(int index) const
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::get",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());

    if(index < 0 || index >= m_objects.size())
    {
        QMODELSLOG_WARNING()<<templateClassName()<<"The index"<<index<<"is out of bound.";
        return nullptr;
    }

    return qobject_cast<T*>(m_objects.at(index));
}

template <typename T>
bool QObjectListModel::setObjects(const QList<T*>& objectList)
{
    if(m_objects.isEmpty() && objectList.isEmpty())
        return true;

    beginResetModel();

    for(int i = 0; i < m_objects.count(); ++i)
    {
        QObject* object = m_objects.at(i);
        objectAboutToBeRemovedNotify(object, i);
        dereferenceObject(object);
        objectRemovedNotify(object, i);
    }
    m_objects.clear();

    bool result = true;
    m_objects.reserve(objectList.count());
    for(int i = 0; i < objectList.count(); ++i)
    {
        QObject* object = objectList.at(i);
        if(!isObjectCompatible(object)) {
            QMODELSLOG_WARNING()<<templateClassName()<<": Fail to insert object"<<object<<"that isn't an appropriate derived class";
            result = false;
            continue;
        }
        objectAboutToBeInsertedNotify(object, m_objects.size());
        m_objects.append(object);
        referenceObject(object);
        objectInsertedNotify(object, m_objects.size()-1);
    }

    endResetModel();

    emit this->objectsChanged(m_objects);

    return result;
}

template <typename T>
bool QObjectListModel::append(const QList<T*>& objectList)
{
    return insert(count(), objectList);
}
template <typename T>
bool QObjectListModel::prepend(const QList<T*>& objectList)
{
    return insert(0, objectList);
}
template <typename T>
bool QObjectListModel::insert(int idx, const QList<T*>& itemList)
{
    if(itemList.isEmpty())
        return true;

    bool result = true;

    QObjectList objectList;
    objectList.reserve(itemList.size());
    for(int i = 0; i < itemList.count(); ++i)
    {
        QObject* object = itemList.at(i);
        if(!isObjectCompatible(object)) {
            QMODELSLOG_WARNING()<<templateClassName()<<": Fail to insert object"<<object<<"that isn't an appropriate derived class";
            result = false;
            continue;
        }
        objectList.append(object);
    }

    beginInsertRows(noParent(), idx, idx + objectList.count() - 1);

    m_objects.reserve(m_objects.count() + objectList.count());
    for(int i = 0; i < objectList.count(); ++i)
    {
        QObject* object = objectList.at(i);
        objectAboutToBeInsertedNotify(object, i + idx);
        m_objects.insert(idx + i, object);
        referenceObject(object);
        objectInsertedNotify(objectList.at(i), i + idx);
    }

    endInsertRows();

    return result;
}
template <typename T>
bool QObjectListModel::remove(const QList<T*>& objects)
{
    bool ret=true;
    for(const auto* object: objects)
    {
        if(!remove(object))
            ret = false;
    }
    return ret;
}
template <typename T>
T* QObjectListModel::takeAt(int index)
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::takeAt",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());

    if(index < 0 || index >= m_objects.size())
    {
        QMODELSLOG_WARNING()<<templateClassName()<<"Can't take an object whose index is out of bound";
        return nullptr;
    }

    beginRemoveRows(noParent(), index, index);

    QObject* item = m_objects.takeAt(index);
    objectAboutToBeRemovedNotify(item, index);
    dereferenceObject(item, false);
    objectRemovedNotify(item, index);

    endRemoveRows();

    return qobject_cast<T*>(item);
}

template <typename T>
T* QObjectListModel::first() const
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::first",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());

    if(m_objects.isEmpty())
    {
        QMODELSLOG_WARNING()<<templateClassName()<<"The first element of an empty list doesn't exist !";
        return nullptr;
    }

    return qobject_cast<T*>(m_objects.first());
}
template <typename T>
T* QObjectListModel::last() const
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::last",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());

    if(m_objects.isEmpty())
    {
        QMODELSLOG_WARNING()<<templateClassName()<<"The last element of an empty list doesn't exist !";
        return nullptr;
    }

    return qobject_cast<T*>(m_objects.last());
}

template <typename T>
QMetaObject::Connection QObjectListModel::onInserted(QObject* receiver, std::function<void(T* object, int index)> callback)
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::onInserted",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());

    if(!receiver)
        return {};

    if(!callback)
        return {};

    return connect(this, &QObjectListModel::objectInserted, receiver, [callback](QObject* qobject, int index)
                   {
                       T* object = qobject_cast<T*>(qobject);
                       if(object)
                           callback(object, index);
                   }, Qt::DirectConnection);
}
template <typename T>
QMetaObject::Connection QObjectListModel::onInserted(QObject* receiver, std::function<void(T* object)> callback)
{
    if(!callback)
        return {};

    return onInserted<T>(receiver, [callback](T* object, int) { callback(object); });
}
template <typename T>
QMetaObject::Connection QObjectListModel::onInserted(std::function<void(T* object, int index)> callback)
{
    return onInserted<T>(this, callback);
}
template <typename T>
QMetaObject::Connection QObjectListModel::onInserted(std::function<void(T* object)> callback)
{
    return onInserted<T>(this, callback);
}

template <typename T>
QMetaObject::Connection QObjectListModel::onRemoved(QObject* receiver, std::function<void(T* object, int index)> callback)
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::onRemoved",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());

    if(!receiver)
        return {};

    if(!callback)
        return {};

    return connect(this, &QObjectListModel::objectRemoved, receiver, [callback](QObject* qobject, int index)
                   {
                       T* object = qobject_cast<T*>(qobject);
                       if(object)
                           callback(object, index);
                   }, Qt::DirectConnection);
}
template <typename T>
QMetaObject::Connection QObjectListModel::onRemoved(QObject* receiver, std::function<void(T* object)> callback)
{
    if(!callback)
        return {};

    return onRemoved<T>(receiver, [callback](T* object, int) { callback(object); });
}
template <typename T>
QMetaObject::Connection QObjectListModel::onRemoved(std::function<void(T* object, int index)> callback)
{
    return onRemoved<T>(this, callback);
}
template <typename T>
QMetaObject::Connection QObjectListModel::onRemoved(std::function<void(T* object)> callback)
{
    return onRemoved<T>(this, callback);
}

template <typename T>
QMetaObject::Connection QObjectListModel::onMoved(QObject* receiver, std::function<void(T* object, int from, int to)> callback)
{
    Q_ASSERT_X(isMetaObjectCompatible(&T::staticMetaObject), "QObjectListModel::onMoved",
               QString("Invalid template type %1 instead of %2").arg(T::staticMetaObject.className()).arg(templateClassName()).toUtf8().constData());

    if(!receiver)
        return {};

    if(!callback)
        return {};

    return connect(this, &QObjectListModel::objectMoved, receiver, [callback](QObject* qobject, int from, int to)
                   {
                       T* object = qobject_cast<T*>(qobject);
                       if(object)
                           callback(object, from, to);
                   }, Qt::DirectConnection);
}
template <typename T>
QMetaObject::Connection QObjectListModel::onMoved(std::function<void(T* object, int from, int to)> callback)
{
    return onMoved<T>(this, callback);
}

class QObjectListModelAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObjectListModel *model READ getModel WRITE setModel NOTIFY modelChanged FINAL)

    Q_CONSTANT_PTR_PROPERTY(QObject, qtObject, QtObject, nullptr)
    Q_READONLY_VAR_PROPERTY(int, count, Count, -1)
    Q_READONLY_VAR_PROPERTY(int, index, Index, -1)
    Q_READONLY_VAR_PROPERTY(bool, isFirst, IsFirst, false)
    Q_READONLY_VAR_PROPERTY(bool, isLast, IsLast, false)

    friend QObjectListModel;

protected:
    explicit QObjectListModelAttached(QObject* object);

public:
    static QObjectListModelAttached* wrap(const QObject* object);

    QObjectListModel* getModel() const;
    void setModel(QObjectListModel* model);

signals:
    void modelChanged(QObjectListModel* model);

private slots:
    void onModelAboutToChange();
    void onModelChanged();
    void onModelCountInvalidate();

private:
    QObjectListModel* m_model;
};

#endif
