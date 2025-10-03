#include "qobjectlistmodel.h"
#include "qmodels_log.h"

QObjectListModel::QObjectListModel(QObject* parent,
                                   const QMetaObject* metaObject,
                                   const QByteArrayList& exposedRoles,
                                   const QByteArrayList& hiddenRoles) :
    QAbstractItemModel(parent),
    m_content(this),
    m_metaObj(metaObject)
{
    m_templateClassName = QStringLiteral("QObjectListModel<%1>").arg(m_metaObj?m_metaObj->className():"");

    m_content.onInserted([this](int, QObject* object) {
        if(!isObjectCompatible(object))
        {
            QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to append default child"<<object<<"that isn't an appropriate derived class";
            return;
        }

        int index = m_objects.count();
        objectAboutToBeInsertedNotify(object, index);
        m_objects.insert(index, object);
        referenceObject(object);
        objectInsertedNotify(object, index);
    });

    m_content.onRemoved([this](int, QObject* object) {
        int index = m_objects.indexOf(object);
        QObject* item = m_objects.takeAt(index);
        objectAboutToBeRemovedNotify(item, index);
        dereferenceObject(item);
        objectRemovedNotify(item, index);
    });

    connect(this, &QAbstractItemModel::rowsInserted, this, &QObjectListModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QObjectListModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &QObjectListModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QObjectListModel::countInvalidate);

    // Keep a track of black list rolename that are not compatible with Qml, they should never be used
    static QSet<QByteArray> roleNamesBlacklist;
    if(roleNamesBlacklist.isEmpty())
    {
        roleNamesBlacklist.reserve(5);
        roleNamesBlacklist<<QByteArrayLiteral("id")<<QByteArrayLiteral("index")<<QByteArrayLiteral("class")
                           <<QByteArrayLiteral("model")<<QByteArrayLiteral("modelData");
    }

    // Set handler that handle every property changed
    static const char* HANDLER = "onObjectPropertyChanged()";
    m_handler = QObjectListModel::metaObject()->method(QObjectListModel::metaObject()->indexOfMethod(HANDLER));

    // Return a pointer to the qtObject as the base Role. This point is essential
    m_roleNames.insert(Qt::UserRole, QByteArrayLiteral("qtObject"));

    if(m_metaObj)
    {
        // Number of attribute declare with the Q_PROPERTY flags
        const int len = m_metaObj->propertyCount();
        m_roleNames.reserve(len+m_roleNames.size());
        m_signalIdxToRole.reserve(len+m_signalIdxToRole.size());
        m_signalIdxs.reserve(len+m_signalIdxs.size());

        // For every property in the ItemType
        static const int offset = QObject::staticMetaObject.propertyCount();
        for(int propertyIdx = offset; propertyIdx < len; propertyIdx++)
        {
            const QMetaProperty metaProp = m_metaObj->property(propertyIdx);
            const QByteArray propName = QByteArray(metaProp.name());
            // Only expose the property as a role if:
            // - It isn't blacklisted(id, index, class, model, modelData)
            // - When exposedRoles is empty we expose every property
            // - When exposedRoles isn't empty we only expose the property asked by the user
            // - When hiddenRoles isn't empty we don't expose these properties
            if(!roleNamesBlacklist.contains(propName) && !hiddenRoles.contains(propName) && (exposedRoles.empty() || exposedRoles.contains(propName)))
            {
                const int role = propertyIdx + Qt::UserRole + 1;
                m_roleNames.insert(role, propName);
                // If there is a notify signal associated with the Q_PROPERTY we keep a track of it for fast lookup
                if(metaProp.hasNotifySignal())
                {
                    m_signalIdxToRole.insert(metaProp.notifySignalIndex(), role);
                    m_signalIdxs.insert(metaProp.notifySignalIndex());
                }
            }
            else if(roleNamesBlacklist.contains(propName))
            {
                QMODELSLOG_WARNING()<<"Can't have"<<propName<<"as a role name in"<<qPrintable(m_templateClassName)
                                     <<", because it's a blacklisted keywork in QML!. "
                                        "Please don't use any of the following words "
                                        "when declaring your Q_PROPERTY:(id, index, "
                                        "class, model, modelData)";
            }
        }
    }

    reverseStringIntHash(m_roleIds, m_roleNames);
}

void QObjectListModel::classBegin()
{

}

void QObjectListModel::componentComplete()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }
}

bool QObjectListModel::isCompleted() const
{
    return m_completed;
}

const QMetaObject& QObjectListModel::emptyMetaObject()
{
    static const QMetaObject ret = QMetaObject();
    return ret;
}
const QString& QObjectListModel::emptyStr()
{
    static const QString ret = "";
    return ret;
}
const QByteArray& QObjectListModel::emptyBA()
{
    static const QByteArray ret = QByteArrayLiteral("");
    return ret;
}
const QModelIndex& QObjectListModel::noParent()
{
    static const QModelIndex ret = QModelIndex();
    return ret;
}

bool QObjectListModel::setData(const QModelIndex& modelIndex, const QVariant& value, int role)
{
    bool ret = false;
    if(!modelIndex.isValid())
        return ret;
    QObject* item = static_cast<QObject*>(modelIndex.internalPointer());
    const int propertyIdx = role - Qt::UserRole - 1;
    if(item != nullptr && propertyIdx >= 0)
        ret = writeProperty(item, propertyIdx, value);
    return ret;
}
QVariant QObjectListModel::data(const QModelIndex& modelIndex, int role) const
{
    QVariant ret;
    if(!modelIndex.isValid())
        return ret;
    QObject* item = static_cast<QObject*>(modelIndex.internalPointer());
    const int propertyIdx = role - Qt::UserRole - 1;
    if(item != nullptr && propertyIdx >= -1)
        ret.setValue(propertyIdx>=0 ? readProperty(item, propertyIdx) : QVariant::fromValue(item));
    return ret;
}

QHash<int, QByteArray> QObjectListModel::roleNames() const
{
    return m_roleNames;
}
int QObjectListModel::rowCount(const QModelIndex& parent) const
{
    return !parent.isValid() ? count() : 0;
}
int QObjectListModel::columnCount(const QModelIndex& parent) const
{
    return parent.column()>0 ? 0 : 1;
}
QModelIndex QObjectListModel::index(int row, int column, const QModelIndex &parent) const
{
    if(row < 0 || row >= count() || column!=0)
        return QModelIndex();
    QObject* item = m_objects.at(row);
    return createIndex(row, column, item);
}
QModelIndex QObjectListModel::parent(const QModelIndex& index) const
{
    return QModelIndex();
}
bool QObjectListModel::hasChildren(const QModelIndex& parent) const
{
    return parent.isValid() ? false : !isEmpty();
}
Qt::ItemFlags QObjectListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    if (index.isValid())
        flags |= Qt::ItemNeverHasChildren;
    return flags;
}
QVariant QObjectListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return role==Qt::DisplayRole ? section+1 : QVariant();
}

void QObjectListModel::sort(int role, Qt::SortOrder order)
{
    beginResetModel();

    sort(m_objects, role, order);

    endResetModel();
}

void QObjectListModel::sort(QObjectList& objects, int role, Qt::SortOrder order) const
{
    const QString& roleName = m_roleNames[role];
    sort(objects, roleName, order);
}

void QObjectListModel::sort(QObjectList& objects, const QString& roleName, Qt::SortOrder order) const
{
    const QByteArray roleNameUtf8 = roleName.toUtf8();
    std::stable_sort(objects.begin(),objects.end(), [&](const QObject* left, const QObject* right) -> bool {
        const QVariant& leftValue = left->property(roleNameUtf8);
        const QVariant& rightValue = right->property(roleNameUtf8);

        const QPartialOrdering ordering = QVariant::compare(leftValue, rightValue);

        if (order == Qt::AscendingOrder) {
            return ordering == QPartialOrdering::Less;
        } else { // Qt::DescendingOrder
            return ordering == QPartialOrdering::Greater;
        }
    });
}

bool QObjectListModel::contains(const QObject* object) const
{
    if(!isObjectCompatible(object))
    {
        QMODELSLOG_WARNING()<<m_templateClassName<<"Can't contains"<<object;
        return false;
    }

    return m_objects.contains(object);
}
int QObjectListModel::indexOf(const QObject* object) const
{
    const int index = m_objects.indexOf(object);
    if(index < 0) {
        QMODELSLOG_DEBUG()<<m_templateClassName<<"The QObject"<<object<<"isn't in this QObjectListModel list.";
    }

    return index;
}
bool QObjectListModel::append(QObject* object)
{
    return insert(count(), object);
}
bool QObjectListModel::prepend(QObject* object)
{
    return insert(0, object);
}
bool QObjectListModel::insert(int index, QObject* object)
{
    if(!isObjectCompatible(object))
    {
        QMODELSLOG_WARNING()<<m_templateClassName<<"Can't insert"<<object;
        return false;
    }

    if(index > count())
    {
        QMODELSLOG_DEBUG()<<m_templateClassName<<"index"<<index<<"is greater than count"<<count()
                          <<"The item will be inserted at the end of the list";
        index = count();
    }
    else if(index < 0)
    {
        QMODELSLOG_DEBUG()<<m_templateClassName<<"index"<<index<<"is lower than 0"
                          <<"The item will be inserted at the beginning of the list";
        index = 0;
    }

    beginInsertRows(noParent(), index, index);

    objectAboutToBeInsertedNotify(object, index);
    m_objects.insert(index, object);
    referenceObject(object);
    objectInsertedNotify(object, index);

    endInsertRows();

    return true;
}
bool QObjectListModel::remove(const QObject* object)
{
    if(!isObjectCompatible(object))
    {
        QMODELSLOG_WARNING()<<m_templateClassName<<"Can't remove"<<object;
        return false;
    }

    return remove(indexOf(object));
}
bool QObjectListModel::remove(int index, int count)
{
    if(index < 0 || (index + count - 1) >= m_objects.size())
    {
        QMODELSLOG_WARNING()<<m_templateClassName<<"Can't remove an object whose index is out of bound";
        return false;
    }

    beginRemoveRows(noParent(), index, index + count - 1);

    for(int i = 0; i < count; ++i)
    {
        QObject* item = m_objects.takeAt(index);
        objectAboutToBeRemovedNotify(item, index + i);
        dereferenceObject(item);
        objectRemovedNotify(item, index + i);
    }

    endRemoveRows();

    return true;
}
bool QObjectListModel::replace(int index, QObject* object)
{
    if(!isObjectCompatible(object))
    {
        QMODELSLOG_WARNING()<<m_templateClassName<<"Can't replace"<<object;
        return false;
    }

    QObject* item = m_objects.takeAt(index);
    objectAboutToBeRemovedNotify(item, index);
    dereferenceObject(item);
    objectRemovedNotify(item, index);

    objectAboutToBeInsertedNotify(object, index);
    m_objects.insert(index, object);
    referenceObject(object);
    objectInsertedNotify(object, index);

    const QModelIndex modelIndex = this->index(index, 0, noParent());
    emit this->dataChanged(modelIndex, modelIndex);

    return true;
}

int QObjectListModel::roleForName(const QString& name) const
{
    return m_roleIds.value(name, -1);
}
QByteArray QObjectListModel::roleName(int role) const
{
    return m_roleNames.value(role, emptyBA());
}

void QObjectListModel::referenceObject(QObject* item)
{
    if(item != nullptr)
    {
        if(!item->parent())
        {
            item->setParent(this);
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }

        for(int key: std::as_const(m_signalIdxs)) {
            connect(item, item->metaObject()->method(key), this, m_handler, Qt::UniqueConnection);
        }

        connect(item, &QObject::destroyed, this, &QObjectListModel::onObjectDestroyed, Qt::DirectConnection);
    }
}
void QObjectListModel::dereferenceObject(QObject* item, bool deleteItem)
{
    if(item != nullptr)
    {
        disconnect(this, nullptr, item, nullptr);
        disconnect(item, nullptr, this, nullptr);

        if(item->parent() == this)
        {
            item->setParent(nullptr);
            if(deleteItem)
                QMetaObject::invokeMethod(item, &QObject::deleteLater, Qt::QueuedConnection);
        }
    }
}

QVariant QObjectListModel::readProperty(const QObject* object, int propertyIndex) const
{
    const QMetaProperty property = m_metaObj->property(propertyIndex);
    return property.read(object);
}
bool QObjectListModel::writeProperty(QObject *object, int propertyIndex, const QVariant &value) const
{
    const QMetaProperty property = m_metaObj->property(propertyIndex);
    return property.write(object, value);
}

void QObjectListModel::onObjectDestroyed(QObject* item)
{
    if(!item)
        return;

    const int index = m_objects.indexOf(item);

    if(index < 0 || index >= m_objects.size())
        return;

    remove(index);
}

void QObjectListModel::onObjectPropertyChanged()
{
    QObject* item = sender();
    const int row = m_objects.indexOf(item);
    const int sig = senderSignalIndex();
    if(row >= 0)
    {
        const QModelIndex index = this->index(row, 0, noParent());
        dataChangedNotify(index, sig);
    }
}
void QObjectListModel::countInvalidate()
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

void QObjectListModel::dataChangedNotify(const QModelIndex& index, int signalIndex)
{
    const QList<int> roles = m_signalIdxToRole.values(signalIndex);
    if(index.isValid() && !roles.isEmpty())
    {
        emit this->dataChanged(index, index, roles);
    }
}

bool QObjectListModel::clear()
{
    if(m_objects.isEmpty())
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

    endResetModel();

    return true;
}

bool QObjectListModel::move(int from, int to)
{
    if(from < 0 || from >= count())
    {
        QMODELSLOG_DEBUG()<<m_templateClassName<<"'From'"<<from<<"is out of bound";
        return false;
    }

    const auto clampedTo = std::clamp(to, 0, count() - 1);
    if(clampedTo != to)
    {
        QMODELSLOG_DEBUG()<<m_templateClassName<<"'to'"<<to<<"in move operation have been clamped to"<<clampedTo;
        to = clampedTo;
        if(from == to)
        {
            QMODELSLOG_DEBUG()<<m_templateClassName<<"Can't move object from"<<from<<"to"<<to<<"because from == to";
            return false;
        }
    }

    beginMoveRows(noParent(), from, from, noParent(), (from < to ? to + 1 : to));

    const auto object = m_objects.at(from);
    objectAboutToBeMovedNotify(object, from, to);
    m_objects.move(from, to);
    objectMovedNotify(object, from, to);

    endMoveRows();

    return true;
}
bool QObjectListModel::moveUp(const int index)
{
    // Move index to index-1
    if(index <= 0 || index >= count())
    {
        QMODELSLOG_DEBUG()<<m_templateClassName<<"The index is the first of the list or index is out of bound";
        return false;
    }

    return move(index, index - 1);
}
bool QObjectListModel::moveDown(const int index)
{
    // Move index to index+1
    if(!(count() &&  // There is a least one entry
          index >= 0 &&  // We can be from the first
          index < (count() - 1))  // To the last one minus 1
        )
    {
        QMODELSLOG_DEBUG()<<m_templateClassName<<"The index is the last of the list or index is out of bound";
        return false;
    }

    return move(index, index + 1);
}

QObject* QObjectListModel::at(QJSValue index) const
{
    const int i = index.toInt();
    if(i < 0 || i >= m_objects.size())
        return nullptr;
    return get<QObject>(i);
}
QObject* QObjectListModel::get(QJSValue index) const
{
    const int i = index.toInt();
    if(i < 0 || i >= m_objects.size())
        return nullptr;
    return get<QObject>(i);
}
QObject* QObjectListModel::takeAt(QJSValue index) const
{
    const int i = index.toInt();
    if(i < 0 || i >= m_objects.size())
        return nullptr;
    return takeAt(i);
}
bool QObjectListModel::append(QJSValue value)
{
    if(value.isQObject())
    {
        QObject* castObject = value.toQObject();
        if(castObject)
        {
            return append(castObject);
        }
        else
        {
            QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to append"<<value.toString()<<"item isn't an appropriate derived class";
            return false;
        }
    }
    else if(value.isArray())
    {
        QObjectList listToAppend;
        for(int i = 0; i < value.property("length").toInt(); ++i)
        {
            const QJSValue object = value.property(i);
            if(object.isQObject())
            {
                QObject* castObject = object.toQObject();
                if(castObject)
                {
                    listToAppend.append(castObject);
                }
                else
                {
                    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to append"<<object.toString()<<"item isn't an appropriate derived class";
                    return false;
                }
            }
            else
            {
                QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to append"<<object.toString()<<"item isn't QObject";
                return false;
            }
        }
        if(!listToAppend.isEmpty())
            return append(listToAppend);
    }

    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to append"<<value.toString()<<"item isn't a QObject, an array of QObject";
    return false;
}
bool QObjectListModel::prepend(QJSValue value)
{
    if(value.isQObject())
    {
        QObject* castObject = value.toQObject();
        if(castObject)
        {
            return prepend(castObject);
        }
        else
        {
            QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to prepend"<<value.toString()<<"item isn't an appropriate derived class";
            return false;
        }
    }
    else if(value.isArray())
    {
        QObjectList listToPrepend;
        for(int i = 0; i < value.property("length").toInt(); ++i)
        {
            const QJSValue object = value.property(i);
            if(object.isQObject())
            {
                QObject* castObject = object.toQObject();
                if(castObject)
                {
                    listToPrepend.append(castObject);
                }
                else
                {
                    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to prepend"<<object.toString()<<"item isn't an appropriate derived class";
                    return false;
                }
            }
            else
            {
                QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to prepend"<<object.toString()<<"item isn't QObject";
                return false;
            }
        }
        if(!listToPrepend.isEmpty())
            return prepend(listToPrepend);
    }

    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to prepend"<<value.toString()<<"item isn't a QObject, an array of QObject";
    return false;
}
bool QObjectListModel::insert(int idx, QJSValue value)
{
    if(value.isQObject())
    {
        QObject* castObject = value.toQObject();
        if(castObject)
        {
            return insert(idx, castObject);
        }
        else
        {
            QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to insert"<<value.toString()<<"item isn't an appropriate derived class";
            return false;
        }
    }
    else if(value.isArray())
    {
        QObjectList listToInsert;
        for(int i = 0; i < value.property("length").toInt(); ++i)
        {
            const QJSValue object = value.property(i);
            if(object.isQObject())
            {
                QObject* castObject = object.toQObject();
                if(castObject)
                {
                    listToInsert.append(castObject);
                }
                else
                {
                    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to insert "<<object.toString()<<"item isn't an appropriate derived class";
                    return false;
                }
            }
            else
            {
                QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to insert"<<object.toString()<<"item isn't QObject";
                return false;
            }
        }
        if(!listToInsert.isEmpty())
            return insert(idx, listToInsert);
    }

    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to insert"<<value.toString()<<"item isn't an array of QObject";
    return false;
}
bool QObjectListModel::remove(QJSValue value)
{
    if(value.isNumber())
    {
        const int index = int(value.toNumber());
        return remove(index);
    }
    else if(value.isQObject())
    {
        QObject* castObject = value.toQObject();
        if(castObject)
        {
            return remove(castObject);
        }
        else
        {
            QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to remove"<<value.toString()<<"item isn't an appropriate derived class";
            return false;
        }
    }
    else if(value.isArray())
    {
        QObjectList listToRemove;
        for(int i = 0; i < value.property("length").toInt(); ++i)
        {
            const QJSValue object = value.property(i);
            if(object.isQObject())
            {
                QObject* castObject = object.toQObject();
                if(castObject)
                {
                    listToRemove.append(castObject);
                }
                else
                {
                    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to remove"<<object.toString()<<"item isn't an appropriate derived class";
                    return false;
                }
            }
            else
            {
                QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to remove"<<object.toString()<<"item isn't QObject";
                return false;
            }
        }
        if(!listToRemove.isEmpty())
            return remove(listToRemove);
    }

    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to remove"<<value.toString()<<"item isn't an array of QObject or a QObject";
    return false;
}
bool QObjectListModel::contains(QJSValue value) const
{
    if(value.isQObject())
    {
        QObject* castObject = value.toQObject();
        if(castObject)
        {
            return contains(castObject);
        }
        else
        {
            QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to get indexOf"<<value.toString()<<"item isn't an appropriate derived class";
            return false;
        }
    }

    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to get indexOf"<<value.toString()<<"item isn't a QObject";
    return false;
}
int QObjectListModel::indexOf(QJSValue value) const
{
    if(value.isQObject())
    {
        QObject* castObject = value.toQObject();
        if(castObject)
        {
            return indexOf(castObject);
        }
        else
        {
            QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to get indexOf"<<value.toString()<<"item isn't an appropriate derived class";
            return -1;
        }
    }

    QMODELSLOG_WARNING()<<m_templateClassName<<": Fail to get indexOf"<<value.toString()<<"item isn't a QObject";
    return -1;
}

void QObjectListModel::objectAboutToBeInsertedNotify(QObject* object, int index)
{
    onObjectAboutToBeInserted(object, index);
}
void QObjectListModel::objectInsertedNotify(QObject* object, int index)
{
    onObjectInserted(object, index);
    emit this->objectInserted(object, index);
}
void QObjectListModel::objectAboutToBeMovedNotify(QObject* object, int src, int dest)
{
    onObjectAboutToBeMoved(object, src, dest);
}
void QObjectListModel::objectMovedNotify(QObject* object, int src, int dest)
{
    onObjectMoved(object, src, dest);
    emit this->objectMoved(object, src, dest);
}
void QObjectListModel::objectAboutToBeRemovedNotify(QObject* object, int index)
{
    onObjectAboutToBeRemoved(object, index);
}
void QObjectListModel::objectRemovedNotify(QObject* object, int index)
{
    onObjectRemoved(object, index);
    emit this->objectRemoved(object, index);
}

void QObjectListModel::onObjectAboutToBeInserted(QObject* object, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(index)
}
void QObjectListModel::onObjectInserted(QObject* object, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(index)
}
void QObjectListModel::onObjectAboutToBeMoved(QObject* object, int src, int dest)
{
    Q_UNUSED(object)
    Q_UNUSED(src)
    Q_UNUSED(dest)
}
void QObjectListModel::onObjectMoved(QObject* object, int src, int dest)
{
    Q_UNUSED(object)
    Q_UNUSED(src)
    Q_UNUSED(dest)
}
void QObjectListModel::onObjectAboutToBeRemoved(QObject* object, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(index)
}
void QObjectListModel::onObjectRemoved(QObject* object, int index)
{
    Q_UNUSED(object)
    Q_UNUSED(index)
}

bool QObjectListModel::isFirst(const QObject* object) const
{
    return object==m_objects.first();
}

bool QObjectListModel::isLast(const QObject* object) const
{
    return object==m_objects.last();
}

const QObjectList& QObjectListModel::objects() const
{
    return m_objects;
}
const QMetaObject* QObjectListModel::templateMetaObject() const
{
    return m_metaObj;
}
const QMultiHash<int, int>& QObjectListModel::signalIdxToRole() const
{
    return m_signalIdxToRole;
}
const QSet<int>& QObjectListModel::signalIdxs() const
{
    return m_signalIdxs;
}
QString QObjectListModel::templateClassName() const
{
    static const QString defaultClassName = "<>";
    return m_metaObj ? m_metaObj->className() : defaultClassName;
}
bool QObjectListModel::isMetaObjectCompatible(const QMetaObject* metaObject) const
{
    if(metaObject==&QObject::staticMetaObject)
        return true;
    if(!m_metaObj || !metaObject)
        return false;
    return metaObject->inherits(m_metaObj) || m_metaObj->inherits(metaObject);
}
bool QObjectListModel::isObjectCompatible(const QObject* object) const
{
    if(!object)
        return false;
    return m_metaObj ? object->metaObject()->inherits(m_metaObj) : false;
}

QObjectListModelAttached* QObjectListModel::qmlAttachedProperties(QObject* object)
{
    if (!object)
    {
        QMODELSLOG_FATAL("QObjectListModelAttached must be attached to a QObject*");
        return nullptr;
    }

    QObjectListModelAttached* helper = new QObjectListModelAttached(object);
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;
}

QObjectListModelAttached::QObjectListModelAttached(QObject* object) :
    QObject(object),
    m_qtObject(object),
    m_model(qobject_cast<QObjectListModel*>(object->parent()))
{
    QObject* parent = object->parent();
    while(parent) {
        if(QObjectListModel* model = QObjectListModelAttached::wrap(parent)->getModel()) {
            m_model = model;
            onModelChanged();
            break;
        }
        parent = parent->parent();
    }
}

QObjectListModelAttached* QObjectListModelAttached::wrap(const QObject* object)
{
    return qobject_cast<QObjectListModelAttached*>(qmlAttachedPropertiesObject<QObjectListModel>(object, true));
}

QObjectListModel* QObjectListModelAttached::getModel() const
{
    return m_model;
}

void QObjectListModelAttached::setModel(QObjectListModel* model)
{
    if(m_model==model)
        return;

    if(m_model)
        m_model->remove(m_qtObject);

    onModelAboutToChange();

    m_model = model;

    if(m_model)
        m_model->append(m_qtObject);

    onModelChanged();

    emit this->modelChanged(m_model);
}

void QObjectListModelAttached::onModelAboutToChange()
{
    if(m_model)
    {
        disconnect(m_model, nullptr, this, nullptr);
        disconnect(this, nullptr, m_model, nullptr);
    }
}

void QObjectListModelAttached::onModelChanged()
{
    if(m_model)
    {
        connect(m_model, &QAbstractItemModel::rowsInserted, this, &QObjectListModelAttached::onModelCountInvalidate);
        connect(m_model, &QAbstractItemModel::rowsRemoved, this, &QObjectListModelAttached::onModelCountInvalidate);
        connect(m_model, &QAbstractItemModel::modelReset, this, &QObjectListModelAttached::onModelCountInvalidate);
        connect(m_model, &QAbstractItemModel::layoutChanged, this, &QObjectListModelAttached::onModelCountInvalidate);
    }

    onModelCountInvalidate();
}

void QObjectListModelAttached::onModelCountInvalidate()
{
    int count = -1;
    int index = -1;

    if(m_model)
    {
        count = m_model->rowCount();
        index = m_model->indexOf(m_qtObject);
    }

    setCount(count);
    setIndex(index);
    setIsFirst(index==0);
    setIsLast(index==(m_count-1));
}
