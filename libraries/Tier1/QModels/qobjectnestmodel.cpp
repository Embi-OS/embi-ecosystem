#include "qobjectnestmodel.h"
#include "qobjectlistmodeliterator.h"

#include <QQmlComponent>

QObjectNestModel::QObjectNestModel(QObject* parent,
                                   const QMetaObject* metaObject,
                                   const QByteArrayList& exposedRoles,
                                   const QByteArrayList& hiddenRoles) :
    QObjectListModel(parent, metaObject, exposedRoles, hiddenRoles),
    m_parentObject(nullptr),
    m_parentModel(nullptr)
{
    Q_ASSERT_X(metaObject && metaObject->inherits(&QObjectNestModel::staticMetaObject), "QObjectNestModel",
               QString("QObjectNestModel metaObject at must least inherits from %1").arg(QObjectNestModel::staticMetaObject.className()).toUtf8());
}

int QObjectNestModel::position() const
{
    return m_parentObject ? m_parentObject->indexOf(this) : 0;
}
QObjectNestModel* QObjectNestModel::parentObject() const
{
    return m_parentObject;
}
QObjectNestModel* QObjectNestModel::parentModel() const
{
    return m_parentModel;
}
void QObjectNestModel::setParentModel(QObjectNestModel* parentModel)
{
    if(m_parentModel==parentModel)
        return;
    m_parentModel = parentModel;

    for(QObject* object: std::as_const(m_objects)) {
        if(QObjectNestModel* model = qobject_cast<QObjectNestModel*>(object))
            model->setParentModel(m_parentModel);
    }
}

bool QObjectNestModel::forEach(std::function<bool(QObjectNestModel* object)> callback, Qt::FindChildOption options) const
{
    if(!callback)
        return false;
    for(QObjectNestModel* object: modelIterator<QObjectNestModel>()) {
        if(!callback(object))
            return false;
        if(options & Qt::FindChildrenRecursively) {
            if(!object->forEach(callback, options))
                return false;
        }
    }
    return true;
}
QObjectNestModel* QObjectNestModel::findChildFromProperty(const QString& property, const QVariant& value, Qt::FindChildOption options) const
{
    QObjectNestModel* ret=nullptr;

    forEach([&](QObjectNestModel* object) {
        if(object->property(property.toUtf8().constData())==value) {
            ret = object;
            return false;
        }
        return true;
    }, options);

    return ret;
}

int QObjectNestModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return count();
    if (parent.column() > 0)
        return 0;
    QObjectNestModel* parentItem = static_cast<QObjectNestModel*>(parent.internalPointer());
    return parentItem->count();
}
int QObjectNestModel::columnCount(const QModelIndex& parent) const
{
    return QObjectListModel::columnCount(parent);
}
QModelIndex QObjectNestModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
        return QObjectListModel::index(row, column, parent);
    QObjectNestModel* parentItem = static_cast<QObjectNestModel*>(parent.internalPointer());
    if(row < 0 || row >= parentItem->count() || column!=0)
        return QModelIndex();
    return createIndex(row, column, parentItem->at(row));
}
QModelIndex QObjectNestModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();
    QObjectNestModel* childItem = static_cast<QObjectNestModel*>(index.internalPointer());
    QObjectNestModel* parentObject = childItem->parentObject();
    QObjectNestModel* parentModel = childItem->parentModel();
    if (!parentObject || parentObject==parentModel)
        return QModelIndex();
    return createIndex(parentObject->position(), 0, parentObject);
}
bool QObjectNestModel::hasChildren(const QModelIndex& parent) const
{
    if(!parent.isValid())
        return !isEmpty();
    if (parent.column() > 0)
        return false;
    QObjectNestModel* childItem = static_cast<QObjectNestModel*>(parent.internalPointer());
    return !childItem->isEmpty();
}
Qt::ItemFlags QObjectNestModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}
QModelIndex QObjectNestModel::child(const QModelIndex& index) const
{
    if(!index.isValid())
        return QModelIndex();
    return createIndex(index.row(), index.column(), index.internalPointer());
}

void QObjectNestModel::onObjectInserted(QObject* object, int index)
{
    if(QObjectNestModel* model = qobject_cast<QObjectNestModel*>(object))
    {
        connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &QObjectNestModel::onChildrenModelAboutToBeReset);
        connect(model, &QAbstractItemModel::modelReset, this, &QObjectNestModel::onChildrenModelReset);
        connect(model, &QAbstractItemModel::dataChanged, this, &QObjectNestModel::onChildrenDataChanged);
        connect(model, &QAbstractItemModel::rowsAboutToBeInserted, this, &QObjectNestModel::onChildrenRowsAboutToBeInserted);
        connect(model, &QAbstractItemModel::rowsInserted, this, &QObjectNestModel::onChildrenRowsInserted);
        connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &QObjectNestModel::onChildrenRowsAboutToBeRemoved);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &QObjectNestModel::onChildrenRowsRemoved);
        connect(model, &QAbstractItemModel::rowsAboutToBeMoved, this, &QObjectNestModel::onChildrenRowsAboutToBeMoved);
        connect(model, &QAbstractItemModel::rowsMoved, this, &QObjectNestModel::onChildrenRowsMoved);

        if(model->m_parentObject)
            QMODELSLOG_WARNING()<<"Object"<<object<<"already has a parentObject"<<model->m_parentObject;
        else
            model->m_parentObject = this;

        if(model->parentModel()) {
            QMODELSLOG_WARNING()<<"Object"<<object<<"already has a parentModel"<<model->parentModel();
        }
        model->setParentModel(this);
    }
}
void QObjectNestModel::onObjectRemoved(QObject* object, int index)
{
    if(QObjectNestModel* model = qobject_cast<QObjectNestModel*>(object))
        model->m_parentObject = nullptr;
}

void QObjectNestModel::onChildrenModelAboutToBeReset()
{
    beginResetModel();
}
void QObjectNestModel::onChildrenModelReset()
{
    endResetModel();
}
void QObjectNestModel::onChildrenDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles)
{
    const QModelIndex thisTopLeft = child(topLeft);
    const QModelIndex thisBottomRight = child(bottomRight);
    emit this->dataChanged(thisTopLeft, thisBottomRight, roles);
}
void QObjectNestModel::onChildrenRowsAboutToBeInserted(const QModelIndex &parent, int first, int last)
{
    const QModelIndex thisParent = parent.isValid() ? child(parent) : index(indexOf(sender()), 0);
    beginInsertRows(thisParent, first, last);
}
void QObjectNestModel::onChildrenRowsInserted(const QModelIndex &parent, int first, int last)
{
    endInsertRows();
}
void QObjectNestModel::onChildrenRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    const QModelIndex thisParent = parent.isValid() ? child(parent) : index(indexOf(sender()), 0);
    beginRemoveRows(thisParent, first, last);
}
void QObjectNestModel::onChildrenRowsRemoved(const QModelIndex &parent, int first, int last)
{
    endInsertRows();
}
void QObjectNestModel::onChildrenRowsAboutToBeMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    const QModelIndex thisSource = sourceParent.isValid() ? child(sourceParent) : index(indexOf(sender()), 0);
    const QModelIndex thisDestination = destinationParent.isValid() ? child(destinationParent) : index(indexOf(sender()), 0);
    beginMoveRows(thisSource, sourceStart, sourceEnd, thisDestination, destinationRow);
}
void QObjectNestModel::onChildrenRowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    endMoveRows();
}
