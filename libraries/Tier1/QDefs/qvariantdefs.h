#ifndef QVARIANTDEFS_H
#define QVARIANTDEFS_H

#include <QString>
#include <QVariant>

//──────────────────────────────────────────────────────────────────────
// Read/Write methods
//──────────────────────────────────────────────────────────────────────

inline QVariant qVariantGetNestedValue(const QVariant& variant, const QStringList& keys, bool* ok=nullptr, int index=0)
{
    if(ok)
        *ok = false;

    if(index<0 || index>=keys.size())
        return QVariant();

    if (variant.metaType().id()!=QMetaType::QVariantMap)
        return QVariant();

    const QString& key = keys[index];
    const QVariantMap& nest = *reinterpret_cast<const QVariantMap*>(variant.constData());

    if(index==keys.size()-1) {
        if(ok && nest.contains(key))
            *ok = true;
        return nest.value(key);
    }

    const QVariant& nestedValue = nest[key];

    return qVariantGetNestedValue(nestedValue, keys, ok, index+1);
}

inline QVariant qVariantGetNestedValue(const QVariant& variant, const QString& keys, const QString& separator=".", bool* ok=nullptr)
{
    if(ok)
        *ok = false;
    if(separator.isEmpty())
        return QVariant();
    const QStringList keyList = keys.split(separator, Qt::SkipEmptyParts);
    return qVariantGetNestedValue(variant, keyList, ok);
}

inline bool qVariantSetNestedValue(QVariant& variant, const QStringList& keys, const QVariant& value, int index=0)
{
    if(index<0 || index>=keys.size())
        return false;

    if (!variant.isValid() || variant.metaType().id()!=QMetaType::QVariantMap)
        variant = QVariantMap();

    const QString& key = keys[index];
    QVariantMap& nest = *reinterpret_cast<QVariantMap*>(variant.data());
    QVariant& nestedValue = nest[key];

    if(index==keys.size()-1) {
        nestedValue = value;
        return true;
    }

    return qVariantSetNestedValue(nestedValue, keys, value, index+1);
}

inline bool qVariantSetNestedValue(QVariant& variant, const QString& keys, const QVariant& value, const QString& separator=".")
{
    if(separator.isEmpty())
        return false;
    const QStringList keyList = keys.split(separator, Qt::SkipEmptyParts);
    return qVariantSetNestedValue(variant, keyList, value);
}

inline bool qVariantRemoveNestedValue(QVariant& variant, const QStringList& keys, int index=0)
{
    if(index<0 || index>=keys.size())
        return false;

    if (variant.metaType().id()!=QMetaType::QVariantMap)
        return false;

    const QString& key = keys[index];
    QVariantMap& nest = *reinterpret_cast<QVariantMap*>(variant.data());

    if(index==keys.size()-1) {
        return nest.remove(key)>0;
    }

    QVariant& nestedValue = nest[key];

    return qVariantRemoveNestedValue(nestedValue, keys, index+1);
}

inline bool qVariantRemoveNestedValue(QVariant& variant, const QString& keys, const QString& separator=".")
{
    if(separator.isEmpty())
        return false;
    const QStringList keyList = keys.split(separator, Qt::SkipEmptyParts);
    return qVariantRemoveNestedValue(variant, keyList);
}

inline QVariant qVariantTakeNestedValue(QVariant& variant, const QStringList& keys, bool* ok=nullptr, int index=0)
{
    if(ok)
        *ok = false;

    if(index<0 || index>=keys.size())
        return QVariant();

    if (variant.metaType().id()!=QMetaType::QVariantMap)
        return QVariant();

    const QString& key = keys[index];
    QVariantMap& nest = *reinterpret_cast<QVariantMap*>(variant.data());

    if(index==keys.size()-1) {
        if(ok && nest.contains(key))
            *ok = true;
        return nest.take(key);
    }

    QVariant& nestedValue = nest[key];

    return qVariantTakeNestedValue(nestedValue, keys, ok, index+1);
}

inline QVariant qVariantTakeNestedValue(QVariant& variant, const QString& keys, const QString& separator=".", bool* ok=nullptr)
{
    if(ok)
        *ok = false;
    if(separator.isEmpty())
        return QVariant();
    const QStringList keyList = keys.split(separator, Qt::SkipEmptyParts);
    return qVariantTakeNestedValue(variant, keyList, ok);
}

inline bool qVariantInsertNestedValue(QVariant& variant, const QStringList& keys, const QVariant& value, int index=0)
{
    if(index<0 || index>=keys.size())
        return false;

    if (!variant.isValid() || variant.metaType().id()!=QMetaType::QVariantMap)
        variant = QVariantMap();

    const QString& key = keys[index];
    QVariantMap& nest = *reinterpret_cast<QVariantMap*>(variant.data());
    QVariant& nestedValue = nest[key];

    if(index==keys.size()-1) {
        if(nestedValue.metaType().id()==QMetaType::QVariantMap && value.metaType().id()==QMetaType::QVariantMap) {
            const QVariantMap& map = *reinterpret_cast<const QVariantMap*>(value.constData());
            QVariantMap& nestedMap = *reinterpret_cast<QVariantMap*>(nestedValue.data());
            nestedMap.insert(map);
        }
        else {
            nestedValue = value;
        }
        return true;
    }

    return qVariantInsertNestedValue(nestedValue, keys, value, index+1);
}

inline bool qVariantInsertNestedValue(QVariant& variant, const QString& keys, const QVariant& value, const QString& separator=".")
{
    if(separator.isEmpty())
        return false;
    const QStringList keyList = keys.split(separator, Qt::SkipEmptyParts);
    return qVariantInsertNestedValue(variant, keyList, value);
}

inline bool qVariantMergeNestedValue(QVariant& variant, const QStringList& keys, const QVariant& value, int index=0)
{
    if(index<0 || index>=keys.size())
        return false;

    if (!variant.isValid() || variant.metaType().id()!=QMetaType::QVariantMap)
        variant = QVariantMap();

    const QString& key = keys[index];
    QVariantMap& nest = *reinterpret_cast<QVariantMap*>(variant.data());
    QVariant& nestedValue = nest[key];

    if(index==keys.size()-1) {
        if(nestedValue.metaType().id()==QMetaType::QVariantMap && value.metaType().id()==QMetaType::QVariantMap) {
            QVariantMap map = value.toMap();
            QVariantMap& nestedMap = *reinterpret_cast<QVariantMap*>(nestedValue.data());
            map.insert(nestedMap);
            nestedValue = map;
        }
        else {
            nestedValue = value;
        }
        return true;
    }

    return qVariantMergeNestedValue(nestedValue, keys, value, index+1);
}

inline bool qVariantMergeNestedValue(QVariant& variant, const QString& keys, const QVariant& value, const QString& separator=".")
{
    if(separator.isEmpty())
        return false;
    const QStringList keyList = keys.split(separator, Qt::SkipEmptyParts);
    return qVariantMergeNestedValue(variant, keyList, value);
}

//──────────────────────────────────────────────────────────────────────
// Compare methods
//──────────────────────────────────────────────────────────────────────

inline bool qVariantEquals(const QVariant &left, const QVariant &right, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false)
{
    if(left.userType()==QMetaType::QString) {
        if (isLocaleAware)
            return left.toString().localeAwareCompare(right.toString())==0;
        else
            return left.toString().compare(right.toString(), cs)==0;
    }
    return left == right;
}

inline int qVariantOrdering(const QVariant &left, const QVariant &right)
{
    const QPartialOrdering ordering = QVariant::compare(left, right);
    if (ordering == QPartialOrdering::Equivalent)
        return 0;
    return (ordering == QPartialOrdering::Greater) ? 1 : -1;
}

inline int qVariantCompare(const QVariant &left, const QVariant &right, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false)
{
    if(cs == Qt::CaseSensitive && !isLocaleAware)
        return qVariantOrdering(left, right);

    switch (left.userType()) {
    case QMetaType::UnknownType:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::LongLong:
    case QMetaType::ULongLong:
    case QMetaType::Float:
    case QMetaType::Double:
    case QMetaType::QChar:
    case QMetaType::QDate:
    case QMetaType::QTime:
    case QMetaType::QDateTime:
    case QMetaType::Bool:
    case QMetaType::QUuid:
        return qVariantOrdering(left, right);
    case QMetaType::QByteArray:
        return left.toByteArray().compare(right.toByteArray());
    case QMetaType::QString:
    default:
        if (isLocaleAware)
            return left.toString().localeAwareCompare(right.toString());
        else
            return left.toString().compare(right.toString(), cs);
        break;
    }
}

inline bool qVariantLessThan(const QVariant &left, const QVariant &right, Qt::CaseSensitivity cs=Qt::CaseSensitive, bool isLocaleAware=false)
{
    return qVariantCompare(left, right, cs, isLocaleAware) < 0;
}

#endif // QVARIANTDEFS_H
