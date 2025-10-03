#include "qmodelhelper.h"
#include "qmodels_log.h"

#include "qvariantlistmodel.h"

#include <QElapsedTimer>
#include <QDateTime>
#include <QHash>

//────────────────────────────────────────────────────────────────────────────────────────────────
// QModelHelper
//────────────────────────────────────────────────────────────────────────────────────────────────

// ──────── CONSTRUCTOR ──────────
QModelHelper::QModelHelper(QAbstractItemModel* object) :
    QObject(object),
    m_model(object)
{
    connect(m_model, &QAbstractItemModel::rowsInserted, this, &QModelHelper::countInvalidate);
    connect(m_model, &QAbstractItemModel::rowsRemoved, this, &QModelHelper::countInvalidate);
    connect(m_model, &QAbstractItemModel::modelReset, this, &QModelHelper::countInvalidate);
    connect(m_model, &QAbstractItemModel::layoutChanged, this, &QModelHelper::countInvalidate);
    connect(m_model, &QAbstractItemModel::dataChanged, this, &QModelHelper::contentChanged);
    m_count = m_model->rowCount();
}

QModelHelper* QModelHelper::wrap(const QObject* object)
{
    return qobject_cast<QModelHelper*>(qmlAttachedPropertiesObject<QModelHelper>(object, true));
}

QModelHelper* QModelHelper::qmlAttachedProperties(QObject* object)
{
    QAbstractItemModel* parent = qobject_cast<QAbstractItemModel*>(object);
    if (!parent)
    {
        QMODELSLOG_CRITICAL()<<object<<parent;
        QMODELSLOG_FATAL("QModelHelper must be attached to a QAbstractItemModel*");
        return nullptr;
    }

    QModelHelper* helper = new QModelHelper(parent);
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;
}

// ──────── ABSTRACT MODEL OVERRIDE ──────────
int QModelHelper::roleForName(const QByteArray& name) const
{
    return roleForName(m_model, name);
}
QByteArray QModelHelper::roleName(int role) const
{
    return roleName(m_model, role);
}

// ──────── ABSTRACT MODEL PRIVATE ──────────
void QModelHelper::countInvalidate()
{
    emit this->contentChanged();

    const int aCount = count();
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

// ──────── PUBLIC API ──────────
QQmlPropertyMap* QModelHelper::map(int row, int column, const QModelIndex& parent)
{
    QQmlPropertyMap* mapper = mapperForRow(row);
    if (!mapper) {
        mapper = new QModelHelperPropertyMap(row, column, parent, m_model, this);
        m_mappers.append({row, mapper});
        connect(mapper, &QObject::destroyed, this, &QModelHelper::removeMapper, Qt::DirectConnection);
    }
    return mapper;
}

// ──────── PUBLIC API ──────────
bool QModelHelper::set(int row, const QVariantMap& map)
{
    return set(m_model, row, map);
}

bool QModelHelper::setProperty(int row, const QString& property, const QVariant& value)
{
    return setProperty(m_model, row, property, value);
}

bool QModelHelper::setProperties(const QString& property, const QVariant& value)
{
    return setProperties(m_model, property, value);
}

bool QModelHelper::setProperties(const QList<int>& indexes, const QString& property, const QVariant& value)
{
    return setProperties(m_model, indexes, property, value);
}

QVariantMap QModelHelper::find(const QString &columnName, const QVariant& val, bool isSorted, Qt::SortOrder sortOrder) const
{
    return find(m_model, columnName, val, isSorted, sortOrder);
}

QVariantMap QModelHelper::get(int row, const QStringList& roles) const
{
    return get(m_model, row, roles);
}

QVariant QModelHelper::getProperty(int row, const QString& property) const
{
    return getProperty(m_model, row, property);
}

QVariantList QModelHelper::getProperties(const QString& property) const
{
    return getProperties(m_model, property);
}

QVariantList QModelHelper::getProperties(const QList<int>& indexes, const QString& property) const
{
    return getProperties(m_model, indexes, property);
}

QVariantList QModelHelper::getProperties(const QModelIndexList& indexes, const QString& property) const
{
    return getProperties(m_model, indexes, property);
}

bool QModelHelper::isSorted(int role, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return isSorted(m_model, role, order, cs, isLocaleAware);
}

bool QModelHelper::isSorted(const QString& roleName, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return isSorted(m_model, roleName, order, cs, isLocaleAware);
}

int QModelHelper::isSortedUntil(int role, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return isSortedUntil(m_model, role, order, cs, isLocaleAware);
}

int QModelHelper::isSortedUntil(const QString& roleName, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return isSortedUntil(m_model, roleName, order, cs, isLocaleAware);
}


int QModelHelper::lowerBound(const QString &roleName, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return lowerBound(m_model, roleName, value, order, cs, isLocaleAware);
}

int QModelHelper::lowerBound(int role, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return lowerBound(m_model, role, value, order, cs, isLocaleAware);
}

int QModelHelper::upperBound(const QString &roleName, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return upperBound(m_model, roleName, value, order, cs, isLocaleAware);
}

int QModelHelper::upperBound(int role, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return upperBound(m_model, role, value, order, cs, isLocaleAware);
}

int QModelHelper::indexOf(const QString &columnName, const QVariant &val, bool isSorted, Qt::SortOrder sortOrder, bool upperBound, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return indexOf(m_model, columnName, val, isSorted, sortOrder, upperBound, cs, isLocaleAware);
}

QList<int> QModelHelper::indexesOf(const QString &columnName, const QVariant &val, Qt::CaseSensitivity cs, bool isLocaleAware) const
{
    return indexesOf(m_model, columnName, val, cs, isLocaleAware);
}

int QModelHelper::count(const QString &columnName, const QVariant &val) const
{
    return count(m_model, columnName, val);
}

bool QModelHelper::contains(const QString &columnName, const QVariant &val, bool isSorted, Qt::SortOrder sortOrder) const
{
    return contains(m_model, columnName, val, isSorted, sortOrder);
}

bool QModelHelper::containsExclude(const QString &columnName, const QVariant &val, const QString& excludedColumnName, const QVariant& excludedVal) const
{
    return containsExclude(m_model, columnName, val, excludedColumnName, excludedVal);
}

bool QModelHelper::equals(const QAbstractItemModel* model) const
{
    return equals(m_model, model);
}

QVariantList QModelHelper::toVariantList() const
{
    return toVariantList(m_model);
}

QVariantList QModelHelper::toFilteredVariantList(const QString &columnName, const QVariantList& values) const
{
    return toFilteredVariantList(m_model, columnName, values);
}

bool QModelHelper::dataDiffersFromRowSibling(int row, int column, int role) const
{
    return dataDiffersFromRowSibling(m_model, row, column, role);
}

bool QModelHelper::dataDiffersFromColumnSibling(int row, int column, int role) const
{
    return dataDiffersFromColumnSibling(m_model, row, column, role);
}

// ──────── PUBLIC STATIC API ──────────
int QModelHelper::roleForName(const QAbstractItemModel* model, const QByteArray& name)
{
    if(!model)
        return -1;

    return model->roleNames().key(name, -1);
}

QByteArray QModelHelper::roleName(const QAbstractItemModel* model, int role)
{
    if(!model)
        return QByteArray();

    return model->roleNames().value(role,"");
}

bool QModelHelper::set(QAbstractItemModel* model, int row, const QVariantMap& map)
{
    if(!model)
        return false;

    bool aRet=false;
    if (row >= count(model) || row < 0)
        return aRet;
    for(auto [key, value]: map.asKeyValueRange())
    {
        aRet = setProperty(model, row, key, value);
    }

    return aRet;
}

bool QModelHelper::setProperty(QAbstractItemModel* model, int row, const QString& property, const QVariant& value)
{
    if(!model)
        return false;

    int roleIndex = roleForName(model, property.toUtf8());

    if(roleIndex<0)
        return false;

    return model->setData(model->index(row, 0), value, roleIndex);
}

bool QModelHelper::setProperties(QAbstractItemModel* model, const QString& property, const QVariant& value)
{
    if(!model)
        return false;

    bool aRet = false;

    int roleIndex = roleForName(model, property.toUtf8());
    for(int i=count(model)-1;i>=0;i--)
    {
        aRet = model->setData(model->index(i, 0), value, roleIndex);
    }

    return aRet;
}

bool QModelHelper::setProperties(QAbstractItemModel* model, const QList<int>& indexes, const QString& property, const QVariant& value)
{
    if(!model)
        return false;

    bool aRet = false;

    int roleIndex = roleForName(model, property.toUtf8());
    for(int index: indexes)
    {
        aRet = model->setData(model->index(index, 0), value, roleIndex);
    }

    return aRet;
}

QVariantMap QModelHelper::find(const QAbstractItemModel* model, const QString &columnName, const QVariant& val, bool isSorted, Qt::SortOrder sortOrder)
{
    int index = indexOf(model, columnName, val, isSorted, sortOrder);
    return get(model, index);
}

QVariantMap QModelHelper::get(const QAbstractItemModel* model, int row, const QStringList& roles)
{
    if(!model)
        return QVariantMap();

    QHash<int,QByteArray> names = model->roleNames();
    if(!roles.isEmpty())
    {
        QHash<int,QByteArray> tmpNames=names;
        names.clear();
        for(const QString& role: roles)
        {
            const QByteArray roleName = role.toUtf8();
            names[tmpNames.key(roleName)]=roleName;
        }
    }

    QVariantMap map;
    const QModelIndex modelIndex = model->index(row, 0);
    for(auto [key, value]: names.asKeyValueRange())
        map.insert(value, model->data(modelIndex, key));

    return map;
}

QVariant QModelHelper::getProperty(const QAbstractItemModel* model, int row, const QString& property)
{
    if(!model)
        return QVariant();

    int roleIndex = roleForName(model, property.toUtf8());

    if(roleIndex<0)
        return QVariant();

    return model->data(model->index(row, 0), roleIndex);
}

QVariantList QModelHelper::getProperties(const QAbstractItemModel* model, const QString& property)
{
    if(!model)
        return QVariantList();

    QVariantList ret;
    int roleIndex = roleForName(model, property.toUtf8());

    if(roleIndex<0)
        return ret;

    ret.reserve(model->rowCount());
    for(int i=0;i<count(model);++i)
    {
        ret+=model->data(model->index(i, 0), roleIndex);
    }

    return ret;
}

QVariantList QModelHelper::getProperties(const QAbstractItemModel* model, const QList<int>& indexes, const QString& property)
{
    if(!model)
        return QVariantList();

    int roleIndex = roleForName(model, property.toUtf8());
    if(roleIndex<0)
        return QVariantList();

    QVariantList ret;
    ret.reserve(indexes.size());
    for(int index: indexes)
    {
        ret+=model->data(model->index(index, 0), roleIndex);
    }

    return ret;
}

QVariantList QModelHelper::getProperties(const QAbstractItemModel* model, const QModelIndexList& indexes, const QString& property)
{
    if(!model)
        return QVariantList();

    int roleIndex = roleForName(model, property.toUtf8());
    if(roleIndex<0)
        return QVariantList();

    QVariantList ret;
    ret.reserve(indexes.size());
    for(const QModelIndex& index: indexes)
    {
        ret+=model->data(index, roleIndex);
    }

    return ret;
}

bool QModelHelper::isSorted(const QAbstractItemModel* model, int role, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if (!model || role<0)
        return false;

    return isSortedUntil(model, role, order, cs, isLocaleAware)==model->rowCount();
}

bool QModelHelper::isSorted(const QAbstractItemModel* model, const QString& roleName, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    int role = roleForName(model, roleName.toUtf8());
    return isSorted(model, role, order, cs, isLocaleAware);
}

int QModelHelper::isSortedUntil(const QAbstractItemModel* model, int role, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if (!model || role<0)
        return -1;

    int first = 0;
    int last = model->rowCount();
    if (first != last)
    {
        int next = first;
        while (++next != last)
        {
            const QVariant nextValue = model->data(model->index(next,0), role);
            const QVariant firstValue = model->data(model->index(first,0), role);
            if (order==Qt::AscendingOrder ? qVariantLessThan(nextValue, firstValue, cs, isLocaleAware) : qVariantLessThan(firstValue, nextValue, cs, isLocaleAware))
                return next;
            first = next;
        }
    }
    return last;
}

int QModelHelper::isSortedUntil(const QAbstractItemModel* model, const QString& roleName, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    int role = roleForName(model, roleName.toUtf8());
    return isSortedUntil(model, role, order, cs, isLocaleAware);
}


int QModelHelper::lowerBound(const QAbstractItemModel* model, const QString& roleName, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    int role = roleForName(model, roleName.toUtf8());
    return lowerBound(model, role, value, order, cs, isLocaleAware);
}

int QModelHelper::lowerBound(const QAbstractItemModel* model, int role, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if (!model || role<0)
        return -1;

    int left = 0;
    int right = model->rowCount();

    // Recherche binaire
    while (left < right) {
        int mid = left + (right - left) / 2;

        const QVariant& cellValue = model->data(model->index(mid,0), role);
        if (order==Qt::AscendingOrder ? qVariantLessThan(cellValue, value, cs, isLocaleAware) : qVariantLessThan(value, cellValue, cs, isLocaleAware)) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    // Vérification finale : s'assurer que left correspond bien à 'value'
    if (left < model->rowCount()) {
        return left;
    }

    return -1; // Non trouvé
}

int QModelHelper::upperBound(const QAbstractItemModel* model, const QString& roleName, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    int role = roleForName(model, roleName.toUtf8());
    return upperBound(model, role, value, order, cs, isLocaleAware);
}

int QModelHelper::upperBound(const QAbstractItemModel* model, int role, const QVariant& value, Qt::SortOrder order, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if (!model || role<0)
        return -1;

    int left = 0;
    int right = model->rowCount();

    // Recherche binaire
    while (left < right) {
        int mid = left + (right - left) / 2;

        const QVariant& cellValue = model->data(model->index(mid,0), role);
        if (order==Qt::AscendingOrder ? !qVariantLessThan(value, cellValue, cs, isLocaleAware) : !qVariantLessThan(cellValue, value, cs, isLocaleAware)) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }

    // Vérification finale : s'assurer que left correspond bien à un élément strictement supérieur
    if (left < model->rowCount()) {
        return left;
    }

    return -1; // Non trouvé
}

int QModelHelper::indexOf(const QAbstractItemModel* model, int role, const QVariant& val, bool isSorted, Qt::SortOrder sortOrder, bool upperBound, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if (!model || model->rowCount() <= 0)
        return -1;

    if(isSorted)
    {
        int index = upperBound ?
                  QModelHelper::upperBound(model, role, val, sortOrder, cs, isLocaleAware) :
                  QModelHelper::lowerBound(model, role, val, sortOrder, cs, isLocaleAware);

        if (index >= 0 && index < model->rowCount())
        {
            auto comp = [&](const QVariant& left, const QVariant& right) -> bool {
                return sortOrder==Qt::AscendingOrder ?
                           qVariantLessThan(left, right, cs, isLocaleAware) :
                           qVariantLessThan(right, left, cs, isLocaleAware);
            };

            const QVariant& cellValue = model->data(model->index(index, 0), role);

            if ((upperBound && comp(val, cellValue)) || // upperBound => strictement supérieur ou inférieur selon order
                (!upperBound && qVariantEquals(cellValue, val, cs, isLocaleAware))) // lowerBound => égalité requise
            {
                return index;
            }
        }
    }
    else
    {
        for(int row=0; row<model->rowCount(); ++row)
        {
            const QVariant& cellValue = model->data(model->index(row,0), role);
            if(qVariantEquals(cellValue, val, cs, isLocaleAware))
            {
                return row;
            }
        }
    }

    // Non trouvé
    return upperBound ? model->rowCount() : -1;
}
QList<int> QModelHelper::indexesOf(const QAbstractItemModel* model, int role, const QVariant& val, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    if(!model)
        return QList<int>();

    QList<int> ret;

    for(int i=0; i<model->rowCount(); ++i)
    {
        const QVariant& cellValue = model->data(model->index(i,0), role);
        if(qVariantEquals(cellValue, val, cs, isLocaleAware))
        {
            ret += i;
        }
    }

    return ret;
}
int QModelHelper::count(const QAbstractItemModel* model, int role, const QVariant& val)
{
    return indexesOf(model, role, val).size();
}
bool QModelHelper::contains(const QAbstractItemModel* model, int role, const QVariant& val, bool isSorted, Qt::SortOrder sortOrder)
{
    return indexOf(model, role, val, isSorted, sortOrder) >= 0;
}
bool QModelHelper::containsExclude(const QAbstractItemModel* model, int role, const QVariant& val, const QString& excludedColumnName, const QVariant& excludedVal)
{
    const QList<int> indexes = indexesOf(model, role, val);
    for(int index: indexes)
    {
        if (getProperty(model, index, excludedColumnName)!=excludedVal) {
            return true;
        }
    }
    return false;
}

int QModelHelper::indexOf(const QAbstractItemModel* model, const QString &columnName, const QVariant& val, bool isSorted, Qt::SortOrder sortOrder, bool upperBound, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    int roleIndex = roleForName(model, columnName.toUtf8());
    return indexOf(model, roleIndex, val, isSorted, sortOrder, upperBound, cs, isLocaleAware);
}
QList<int> QModelHelper::indexesOf(const QAbstractItemModel* model, const QString &columnName, const QVariant& val, Qt::CaseSensitivity cs, bool isLocaleAware)
{
    int roleIndex = roleForName(model, columnName.toUtf8());
    return indexesOf(model, roleIndex, val, cs, isLocaleAware);
}
int QModelHelper::count(const QAbstractItemModel* model, const QString &columnName, const QVariant& val)
{
    int roleIndex = roleForName(model, columnName.toUtf8());
    return count(model, roleIndex, val);
}
bool QModelHelper::contains(const QAbstractItemModel* model, const QString& columnName, const QVariant& val, bool isSorted, Qt::SortOrder sortOrder)
{
    int roleIndex = roleForName(model, columnName.toUtf8());
    return contains(model, roleIndex, val, isSorted, sortOrder);
}
bool QModelHelper::containsExclude(const QAbstractItemModel* model, const QString& columnName, const QVariant& val, const QString& excludedColumnName, const QVariant& excludedVal)
{
    int roleIndex = roleForName(model, columnName.toUtf8());
    return containsExclude(model, roleIndex, val, excludedColumnName, excludedVal);
}

bool QModelHelper::equals(const QAbstractItemModel* srcModel, const QAbstractItemModel* compModel)
{
    if(srcModel==nullptr || compModel==nullptr)
        return false;

    if(srcModel->rowCount() != compModel->rowCount())
        return false;

    if(srcModel->roleNames().count() != compModel->roleNames().count())
        return false;

    for(int i=0; i<srcModel->rowCount(); ++i)
    {
        const QVariantMap& a = QModelHelper::get(srcModel, i);
        const QVariantMap& b = QModelHelper::get(compModel, i);

        const QList<QString> aProps=a.keys();
        const QList<QString> bProps=b.keys();

        if (aProps.count() != bProps.count())
            return false;

        for(const QString& prop: aProps)
        {
            if (a.value(prop) != b.value(prop))
                return false;
        }
    }

    return true;
}

QVariantList QModelHelper::toVariantList(const QAbstractItemModel* model)
{
    if(const QVariantListModel* varModel = qobject_cast<const QVariantListModel*>(model))
    {
        return varModel->getStorage();
    }

    const QHash<int,QByteArray> names = model->roleNames();
    QVariantList variants;
    variants.reserve(model->rowCount());
    for(int i=0; i<model->rowCount(); ++i)
    {
        QVariantMap map;
        const QModelIndex modelIndex = model->index(i, 0);
        for(auto [key, value]: names.asKeyValueRange())
            map.insert(value, model->data(modelIndex, key));
        variants.append(map);
    }

    return variants;
}

QVariantList QModelHelper::toFilteredVariantList(const QAbstractItemModel* model, const QString &columnName, const QVariantList& values)
{
    QVariantList variants;
    variants.reserve(values.size());
    if(const QVariantListModel* varModel = qobject_cast<const QVariantListModel*>(model))
    {
        for(const QVariant& var: varModel->getStorage())
        {
            if(var.metaType().id()==QMetaType::QVariantMap)
            {
                const QVariantMap& original = *reinterpret_cast<const QVariantMap*>(var.constData());
                if(!values.contains(original.value(columnName)))
                    continue;
                variants.append(original);
            }
        }
    }
    else
    {
        const QHash<int,QByteArray> names = model->roleNames();
        int roleIndex = roleForName(model, columnName.toUtf8());
        for(int i=0; i<model->rowCount(); ++i)
        {
            const QModelIndex modelIndex = model->index(i, 0);
            if(!values.contains(model->data(modelIndex, roleIndex)))
                continue;
            QVariantMap map;
            for(auto [key, value]: names.asKeyValueRange())
                map.insert(value, model->data(modelIndex, key));
            variants.append(map);
        }
    }

    return variants;
}

bool QModelHelper::dataDiffersFromRowSibling(const QAbstractItemModel* model, int row, int column, int role)
{
    if(!model)
        return false;

    const QVariant dataBefore = model->index(row-1, column).data(role);
    const QVariant dataAfter = model->index(row+1, column).data(role);

    const QVariant data = model->index(row, column).data(role);

    return (row>0 && data!=dataBefore) || (row<model->rowCount()-1 && data!=dataAfter);
}

bool QModelHelper::dataDiffersFromColumnSibling(const QAbstractItemModel* model, int row, int column, int role)
{
    if(!model)
        return false;

    const QVariant dataBefore = model->index(row, column-1).data(role);
    const QVariant dataAfter = model->index(row, column+1).data(role);

    const QVariant data = model->index(row, column).data(role);

    return (column>0 && data!=dataBefore) || (column<model->columnCount()-1 && data!=dataAfter);
}

// ──────── HELPER PRIVATE ──────────
QQmlPropertyMap* QModelHelper::mapperForRow(int row) const
{
    auto it = std::find_if(
        m_mappers.cbegin(),
        m_mappers.cend(),
        [row] (const QPair<int, QQmlPropertyMap*> pair) {
            return pair.first == row;
        });

    if (it != m_mappers.cend())
        return it->second;
    else
        return nullptr;
}

void QModelHelper::removeMapper(QObject* mapper)
{
    if(!mapper)
        return;

    auto it = std::find_if(
        m_mappers.cbegin(),
        m_mappers.cend(),
        [mapper] (const QPair<int, QQmlPropertyMap*> pair) {
            return pair.second == mapper;
        });

    if (it != m_mappers.cend())
        m_mappers.erase(it);
}

//────────────────────────────────────────────────────────────────────────────────────────────────
// QModelHelperPropertyMap
//────────────────────────────────────────────────────────────────────────────────────────────────

QModelHelperPropertyMap::QModelHelperPropertyMap(int row, int column, const QModelIndex& parentIndex, QAbstractItemModel* model, QObject* parent) :
    QQmlPropertyMap(parent),
    m_row(row),
    m_column(column),
    m_parent(parentIndex),
    m_model(model)
{
    connect(model, &QAbstractItemModel::modelReset, this, &QModelHelperPropertyMap::update);
    connect(model, &QAbstractItemModel::layoutChanged, this, &QModelHelperPropertyMap::update);
    connect(model, &QAbstractItemModel::dataChanged, this, &QModelHelperPropertyMap::onDataChanged);
    connect(model, &QAbstractItemModel::rowsInserted, this, &QModelHelperPropertyMap::onRowsInserted);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &QModelHelperPropertyMap::onRowsRemoved);
    connect(model, &QAbstractItemModel::columnsInserted, this, &QModelHelperPropertyMap::onColumnsInserted);
    connect(model, &QAbstractItemModel::columnsRemoved, this, &QModelHelperPropertyMap::onColumnsRemoved);
    update();
}

QVariant QModelHelperPropertyMap::updateValue(const QString& key, const QVariant& input)
{
    int role = m_model->roleNames().key(key.toUtf8(), -1);
    if (role == -1)
        return input;

    QModelIndex index = modelIndex();
    m_model->setData(index, input, role);
    return m_model->data(index, role);
}

QModelIndex QModelHelperPropertyMap::modelIndex() const
{
    return m_model->index(m_row, m_column, m_parent);
}

void QModelHelperPropertyMap::update()
{
    const QHash<int, QByteArray> roles = m_model->roleNames();
    const QModelIndex index(modelIndex());

    for(auto [key, value]: roles.asKeyValueRange())
        insert(value, m_model->data(index, key));
}

void QModelHelperPropertyMap::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight, const QVector<int>& roles)
{
    QModelIndex index(modelIndex());
    if (m_parent != topLeft.parent() || m_parent != bottomRight.parent())
        return;

    if (m_row >= topLeft.row() && m_column >= topLeft.column() && m_row <= bottomRight.row() && m_column <= bottomRight.column()) {
        auto roleNames = m_model->roleNames();
        QVector<int> actualRoles;
        if(roles.isEmpty())
        {
            QList<int> keys = roleNames.keys();
            actualRoles = keys.toVector();
        }
        else
        {
            actualRoles = roles;
        }

        for (int role : std::as_const(actualRoles))
            insert(roleNames[role], m_model->data(index, role));
    }
}
void QModelHelperPropertyMap::onRowsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parent && m_row >= first)
        update();
}
void QModelHelperPropertyMap::onRowsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parent && m_row >= first)
        update();
}
void QModelHelperPropertyMap::onColumnsInserted(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parent && m_column >= first)
        update();
}
void QModelHelperPropertyMap::onColumnsRemoved(const QModelIndex& parent, int first, int last)
{
    Q_UNUSED(last)
    if (parent == m_parent && m_column >= first)
        update();
}
