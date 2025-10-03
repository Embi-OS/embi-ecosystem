#include "qspatch.h"

QSPatch::QSPatch(QSPatch::Type type, int from, int to, int count):
    m_type(type),
    m_from(from),
    m_to(to),
    m_count(count)
{

}

QSPatch::QSPatch(QSPatch::Type type, int from, int to, int count, const QVariantMap& data):
    m_type(type),
    m_from(from),
    m_to(to),
    m_count(count)
{
    m_data.append(data);
}

QSPatch::QSPatch(Type type,int from, int to, int count, const QVariantList& data):
    m_type(type),
    m_from(from),
    m_to(to),
    m_count(count),
    m_data(data)
{

}


QSPatch::QSPatch(const QSPatch& other) :
    QSPatch(other.m_type, other.m_from, other.m_to, other.m_count, other.m_data)
{

}

QSPatch &QSPatch::operator=(const QSPatch& other)
{
    m_type = other.m_type;
    m_from = other.m_from;
    m_to = other.m_to;
    m_count = other.m_count;
    m_data = other.m_data;
    return *this;
}

QSPatch::Type QSPatch::type() const
{
    return m_type;
}

void QSPatch::setType(const QSPatch::Type &type)
{
    m_type = type;
}

const QVariantList& QSPatch::data() const
{
    return m_data;
}

void QSPatch::setData(const QVariantList& data)
{
    m_data = data;
}

void QSPatch::setData(const QVariantMap& data)
{
    m_data.clear();
    m_data.append(data);
}

bool QSPatch::operator==(const QSPatch& rhs) const
{
    if (m_type != rhs.m_type ||
        m_from != rhs.m_from ||
        m_to != rhs.m_to ||
        m_count != rhs.m_count ||
        m_data != rhs.m_type) {
        return false;
    }

    return true;
}

int QSPatch::from() const
{
    return m_from;
}

void QSPatch::setFrom(int from)
{
    m_from = from;
}

int QSPatch::to() const
{
    return m_to;
}

void QSPatch::setTo(int to)
{
    m_to = to;
}

bool QSPatch::isNull() const
{
    return m_type == QSPatch::Null;
}

int QSPatch::count() const
{
    return m_count;
}

void QSPatch::setCount(int count)
{
    m_count = count;
}

bool QSPatch::canMerge(const QSPatch &other) const
{
    bool res = false;

    if (m_type != other.type()) {
        return false;
    }

    if (m_type == QSPatch::Remove) {
        if (m_from == other.to() + 1 ||
            m_to == other.from() - 1) {
            res = true;
        }
    } else if (m_type == QSPatch::Move) {
        if (m_from + m_count == other.from()  &&
            m_to + m_count == other.to() ) {
            res = true;
        }
    } else if (m_type == QSPatch::Insert) {
        if (m_from == other.from() ||
            m_to == other.from() - 1) {
            res = true;
        }
    }

    return res;
}

QSPatch QSPatch::merged(const QSPatch &other) const
{
    if (!canMerge(other)) {
        return QSPatch();
    }

    QSPatch res = *this;

    res.merge(other);

    return res;
}

QSPatch &QSPatch::merge(const QSPatch &other)
{
    if (!canMerge(other))
        return (*this);

    if (m_type == QSPatch::Remove) {
        m_from = qMin(m_from, other.from());
        m_to = qMax(m_to, other.to());
        m_count = m_to - m_from + 1;
    } else if (m_type == QSPatch::Move) {
        m_count = m_count + other.count();
    } else if (m_type == QSPatch::Insert) {
        QVariantList list = m_data;
        list.append(other.data());
        m_data = list;
        m_to  = m_from + list.count() - 1;
        m_count = list.count();
    }

    return *this;
}

QSPatch QSPatch::createUpdate(int index, const QVariantMap &diff)
{
    if(diff.isEmpty())
        return QSPatch();
    return QSPatch(QSPatch::Update, index, index, 1, diff);
}

QSPatch QSPatch::createRemove(int from, int to)
{
    return QSPatch(QSPatch::Remove, from, to, to - from + 1);
}

QSPatch QSPatch::createInsert(int index, const QVariantMap& map)
{
    if(map.isEmpty())
        return QSPatch();
    return QSPatch(QSPatch::Insert, index, index, 1, map);
}

QSPatch QSPatch::createInsert(int index, const QVariantList& list)
{
    return QSPatch(QSPatch::Insert, index, index+list.size(), list.size(), list);
}

QDebug operator<<(QDebug dbg, const QSPatch& patch){
    switch (patch.type()) {
    case QSPatch::Remove:
        dbg << QString("QSPatch(Remove from %1 to %2 with %3)").arg(patch.from()).arg(patch.to()).arg(patch.count());
        break;
    case QSPatch::Move:
        dbg << QString("QSPatch(Move from %1 to %2 with %3)").arg(patch.from()).arg(patch.to()).arg(patch.count());
        break;
    case QSPatch::Insert:
        dbg << QString("QSPatch(Insert from %1 to %2 with %3)").arg(patch.from()).arg(patch.to()).arg(patch.count());
        dbg << patch.data();
        break;
    case QSPatch::Update:
        dbg << QString("QSPatch(Update %1)").arg(patch.from());
        dbg << patch.data();
        break;
    default:
        dbg << "Patch(Null)";
        break;
    }

    return dbg;
}


