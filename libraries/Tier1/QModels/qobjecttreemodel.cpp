#include "qobjecttreemodel.h"
#include "qobjectlistmodeliterator.h"
#include "qmodels_log.h"

QTreeObject::QTreeObject(QObject* parent) :
    QObject(parent),
    m_content(this),
    m_parentObject(nullptr),
    m_parentModel(nullptr)
{
    m_content.onInserted([this](int index, QTreeObject* object) {
        object->m_parentObject = this;
        object->setParentModel(m_parentModel);
        connectObjectProperty(object);
        objectInsertedNotify(object, index);
        countInvalidate();
    });
    m_content.onRemoved([this](int index, QTreeObject* object) {
        disconnect(object, nullptr, this, nullptr);
        disconnect(this, nullptr, object, nullptr);
        objectRemovedNotify(object, index);
        countInvalidate();
    });

    // Set handler that handle every property changed
    static const char* HANDLER = "onObjectPropertyChanged()";
    m_handler = QTreeObject::metaObject()->method(QTreeObject::metaObject()->indexOfMethod(HANDLER));
}

void QTreeObject::classBegin()
{

}

void QTreeObject::componentComplete()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }
}

bool QTreeObject::isCompleted() const
{
    return m_completed;
}

void QTreeObject::onObjectPropertyChanged()
{
    QTreeObject* object = qobject_cast<QTreeObject*>(sender());
    const int signalIndex = senderSignalIndex();
    emit this->childrenDataChanged(object, signalIndex);
}

void QTreeObject::countInvalidate()
{
    int aCount = count();
    bool emptyChanged=false;

    if(m_count==aCount)
        return;

    if((m_count==0 && aCount!=0) || (m_count!=0 && aCount==0))
        emptyChanged=true;

    m_count=aCount;
    emit this->countChanged(count());

    if(emptyChanged)
        emit this->emptyChanged(isEmpty());
}

int QTreeObject::position() const
{
    return m_parentObject ? m_parentObject->content().indexOf(this) : m_parentModel->indexOf(this);
}

QModelIndex QTreeObject::index() const
{
    if(!m_parentModel)
        return QModelIndex();
    return m_parentModel->createIndex(position(), 0, this);
}

QTreeObject* QTreeObject::parentObject() const
{
    return m_parentObject;
}

QObjectTreeModel* QTreeObject::parentModel() const
{
    return m_parentModel;
}

void QTreeObject::setParentModel(QObjectTreeModel* parentModel)
{
    if(m_parentModel==parentModel)
        return;
    m_parentModel = parentModel;

    for(QTreeObject* object: m_content) {
        object->setParentModel(m_parentModel);
        connectObjectProperty(object);
    }
}

bool QTreeObject::forEach(std::function<bool(QTreeObject* object)> callback, Qt::FindChildOption options) const
{
    if(!callback)
        return false;
    for(QTreeObject* object: m_content) {
        if(!callback(object))
            return false;
        if(options & Qt::FindChildrenRecursively) {
            if(!object->forEach(callback, options))
                return false;
        }
    }
    return true;
}

void QTreeObject::objectInsertedNotify(QTreeObject* object, int index)
{
    onObjectInserted(object, index);
    emit this->objectInserted(object, index);
    emit this->treeObjectInserted(object, this, index);
}
void QTreeObject::objectRemovedNotify(QTreeObject* object, int index)
{
    onObjectRemoved(object, index);
    emit this->objectRemoved(object, index);
    emit this->treeObjectRemoved(object, this, index);
}
void QTreeObject::onObjectInserted(QTreeObject* object, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(index)
}
void QTreeObject::onObjectRemoved(QTreeObject* object, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(index)
}

void QTreeObject::connectObjectProperty(QTreeObject* object)
{
    disconnect(object, nullptr, this, nullptr);

    if(!m_parentModel)
        return;

    connect(object, &QTreeObject::childrenDataChanged, this, &QTreeObject::childrenDataChanged, Qt::UniqueConnection);
    connect(object, &QTreeObject::treeObjectInserted, this, &QTreeObject::treeObjectInserted, Qt::UniqueConnection);
    connect(object, &QTreeObject::treeObjectRemoved, this, &QTreeObject::treeObjectRemoved, Qt::UniqueConnection);
    for(int key: m_parentModel->signalIdxs())
        connect(object, object->metaObject()->method(key), this, m_handler, Qt::UniqueConnection);
}

QObjectTreeModel::QObjectTreeModel(QObject* parent,
                                   const QMetaObject* metaObject,
                                   const QByteArrayList& exposedRoles,
                                   const QByteArrayList& hiddenRoles) :
    QObjectListModel(parent, metaObject, exposedRoles, hiddenRoles)
{
    Q_ASSERT_X(metaObject && metaObject->inherits(&QTreeObject::staticMetaObject), "QObjectTreeModel",
               QString("QObjectTreeModel metaObject must at least inherits from %1").arg(QTreeObject::staticMetaObject.className()).toUtf8());
}

int QObjectTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return count();
    if (parent.column() > 0)
        return 0;
    QTreeObject* parentItem = static_cast<QTreeObject*>(parent.internalPointer());
    return parentItem->content().count();
}
int QObjectTreeModel::columnCount(const QModelIndex& parent) const
{
    return QObjectListModel::columnCount(parent);
}
QModelIndex QObjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return QObjectListModel::index(row, column, parent);
    QTreeObject* parentItem = static_cast<QTreeObject*>(parent.internalPointer());
    if(row < 0 || row >= parentItem->count() || column!=0)
        return QModelIndex();
    return createIndex(row, column, parentItem->content().at(row));
}
QModelIndex QObjectTreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();
    QTreeObject* childItem = static_cast<QTreeObject*>(index.internalPointer());
    QTreeObject* parentObject = childItem->parentObject();
    if (!parentObject)
        return QModelIndex();
    return createIndex(parentObject->position(), 0, parentObject);
}
bool QObjectTreeModel::hasChildren(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return !isEmpty();
    if (parent.column() > 0)
        return false;
    QTreeObject* childItem = static_cast<QTreeObject*>(parent.internalPointer());
    return !childItem->isEmpty();
}
Qt::ItemFlags QObjectTreeModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

bool QObjectTreeModel::clearObject(QTreeObject* parent)
{
    if(!parent)
        return false;
    const QModelIndex index = parent->index();
    beginRemoveRows(index, 0, parent->count()-1);
    parent->content().remove(0, parent->count());
    endRemoveRows();
    return true;
}
bool QObjectTreeModel::removeObject(QTreeObject* object, QTreeObject* parent)
{
    if(!object || !parent)
        return false;
    int pos = object->position();
    const QModelIndex index = parent->index();
    beginRemoveRows(index, pos, pos);
    parent->content().remove(object);
    endRemoveRows();
    return true;
}
bool QObjectTreeModel::insertObject(QTreeObject* object, QTreeObject* parent, int pos)
{
    if(!object || !parent)
        return false;
    if(pos >= parent->count())
        return false;
    if(pos < 0)
        pos = parent->count();
    const QModelIndex index = parent->index();
    beginInsertRows(index, pos, pos);
    parent->content().insert(pos, object);
    endInsertRows();
    return true;
}
bool QObjectTreeModel::insertObjects(QList<QTreeObject*> objects, QTreeObject* parent, int pos)
{
    if(objects.isEmpty() || !parent)
        return false;
    if(pos >= parent->count())
        return false;
    if(pos < 0)
        pos = parent->count();
    const QModelIndex index = parent->index();
    beginInsertRows(index, pos, pos+objects.count()-1);
    parent->content().insert(pos, objects);
    endInsertRows();
    return true;
}

bool QObjectTreeModel::forEach(std::function<bool(QTreeObject* object)> callback, Qt::FindChildOption options) const
{
    if(!callback)
        return false;
    for(QTreeObject* object: modelIterator<QTreeObject>()) {
        if(!callback(object))
            return false;
        if(options & Qt::FindChildrenRecursively) {
            if(!object->forEach(callback, options))
                return false;
        }
    }
    return true;
}
QTreeObject* QObjectTreeModel::findChildFromProperty(const QString& property, const QVariant& value, Qt::FindChildOption options) const
{
    QTreeObject* ret=nullptr;

    forEach([&](QTreeObject* object) {
        if(object->property(property.toUtf8().constData())==value) {
            ret = object;
            return false;
        }
        return true;
    }, options);

    return ret;
}

void QObjectTreeModel::onChildrenDataChanged(QTreeObject* object, int signalIndex)
{
    if(!object)
        return;
    const QModelIndex index = object->index();
    dataChangedNotify(index, signalIndex);
}

void QObjectTreeModel::onObjectInserted(QObject* object, int index)
{
    if(QTreeObject* treeObject = qobject_cast<QTreeObject*>(object))
    {
        connect(treeObject, &QTreeObject::childrenDataChanged, this, &QObjectTreeModel::onChildrenDataChanged);
        connect(treeObject, &QTreeObject::treeObjectInserted, this, &QObjectTreeModel::treeObjectInsertedNotify);
        connect(treeObject, &QTreeObject::treeObjectRemoved, this, &QObjectTreeModel::treeObjectRemovedNotify);

        if(treeObject->parentModel()) {
            QMODELSLOG_WARNING()<<"Object"<<object<<"already has a parentModel"<<treeObject->parentModel();
        }
        treeObject->setParentModel(this);

        treeObjectInsertedNotify(treeObject, this, index);
    }
}
void QObjectTreeModel::onObjectRemoved(QObject* object, int index)
{
    if(QTreeObject* treeObject = qobject_cast<QTreeObject*>(object)) {
        treeObject->setParentModel(nullptr);

        treeObjectRemovedNotify(treeObject, this, index);
    }
}

void QObjectTreeModel::treeObjectInsertedNotify(QTreeObject* object, QObject* parent, int index)
{
    onTreeObjectInserted(object, parent, index);
    emit this->treeObjectInserted(object, parent, index);
}
void QObjectTreeModel::treeObjectRemovedNotify(QTreeObject* object, QObject* parent, int index)
{
    onTreeObjectRemoved(object, parent, index);
    emit this->treeObjectRemoved(object, parent, index);
}

void QObjectTreeModel::onTreeObjectInserted(QTreeObject* object, QObject* parent, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(parent)
    Q_UNUSED(index)
}
void QObjectTreeModel::onTreeObjectRemoved(QTreeObject* object, QObject* parent, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(parent)
    Q_UNUSED(index)
}
