#include "qvariantreadermodel.h"
#include <QJsonVariant>

QVariantReaderModel::QVariantReaderModel(QObject* parent):
    QAbstractListModel(parent)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &QVariantReaderModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QVariantReaderModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &QVariantReaderModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QVariantReaderModel::countInvalidate);

    connect(this, &QVariantReaderModel::rolesChanged, this, [this](const QStringList& roles){
        QVariantMap map;
        for(const QString& field: roles)
            map.insert(field, QVariant());
        updateRoleNames(map);
    });
    connect(this, &QVariantReaderModel::placeholderChanged, this, [this](const QVariantMap& placeholder){
        updateRoleNames(placeholder);
    });
}

void QVariantReaderModel::classBegin()
{

}

void QVariantReaderModel::componentComplete()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }
}

bool QVariantReaderModel::isCompleted() const
{
    return m_completed;
}

QVariantReader* QVariantReaderModel::reader() const
{
    return m_reader;
}

QVariant QVariantReaderModel::data(const QModelIndex & index, const int role) const
{
    if(!index.isValid() || role < Qt::UserRole)
        return QVariant();

    int row = index.row();
    if (row!=m_bottom && row >= m_cache.size() && !const_cast<QVariantReaderModel*>(this)->seek(row))
        return QVariant();

    if(m_forwardOnly && row!=m_bottom)
        return QVariant();

    row = m_forwardOnly ? 0 : row;
    if(role == Qt::UserRole)
    {
        return m_cache.at(row);
    }
    else if(role > Qt::UserRole)
    {
        if(m_cache.at(row).metaType().id()==QMetaType::QVariantMap)
        {
            const QVariantMap& original = *reinterpret_cast<const QVariantMap*>(m_cache[row].constData());
            const QString& key = m_roleNames[role];
            return original.value(key, m_placeholder[key]);
        }
    }

    return QVariant();
}

QHash<int, QByteArray> QVariantReaderModel::roleNames(void) const
{
    return m_roleNames;
}

void QVariantReaderModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;
    seek(m_bottom+1);
}

bool QVariantReaderModel::canFetchMore(const QModelIndex &parent) const
{
    if(parent.isValid() || !m_initialized)
        return false;
    return m_reader->hasNext();
}

int QVariantReaderModel::rowCount(const QModelIndex & parent) const
{
    if(parent.isValid() || !m_initialized)
        return 0;

    if(m_rowCount>0)
        return m_rowCount;

    if(m_estimateCount && canFetchMore(parent))
        return qMax(m_bottom+2, int((double)(m_bottom+1) * ((double)m_reader->totalSize() / (double)m_reader->currentOffset())));

    return (canFetchMore(parent) ? m_bottom+2 : m_bottom+1);
}

int QVariantReaderModel::roleForName(const QString& name) const
{
    return m_roleIds.value(name, -1);
}

QByteArray QVariantReaderModel::roleName(int role) const
{
    return m_roleNames.value(role, QByteArray());
}

void QVariantReaderModel::clearRoleNames()
{
    m_roleNames.clear();
    m_roleIds.clear();
}

void QVariantReaderModel::updateRoleNames(const QVariant& var)
{
    if(m_roleNames.isEmpty())
    {
        QHash<int, QByteArray> roleNames;
        if(var.canConvert<QVariantMap>())
        {
            const QVariantMap& firstElement = var.toMap();
            QMapIterator<QString,QVariant> iter(firstElement);
            int role = Qt::UserRole+1;

            roleNames.reserve(firstElement.size());
            while (iter.hasNext())
            {
                iter.next();
                roleNames[role] = iter.key().toLocal8Bit();
                role++;
            }
        }
        else
        {
            roleNames[Qt::UserRole] = QByteArrayLiteral("qtVariant");
        }

        m_roleNames = roleNames;
        reverseStringIntHash(m_roleIds, m_roleNames);
    }
}

bool QVariantReaderModel::setCbor(const QByteArray& cbor)
{
    if(m_reader) {
        delete m_reader;
        m_reader = nullptr;
    }

    m_reader = new QCborVariantReader(cbor);

    return initilized();
}
bool QVariantReaderModel::setJson(const QByteArray& json)
{
    if(m_reader) {
        delete m_reader;
        m_reader = nullptr;
    }

    m_reader = new QJsonVariantReader(json);

    return initilized();
}
bool QVariantReaderModel::clear()
{
    if(m_reader) {
        delete m_reader;
        m_reader = nullptr;
    }

    beginResetModel();

    m_initialized = false;
    m_cache.clear();
    m_cache.squeeze();;
    m_bottom = -1;
    m_rowCount = -1;

    endResetModel();

    return true;
}

bool QVariantReaderModel::seek(int index)
{
    if(!m_initialized)
        return false;

    if (!m_forwardOnly && index <= m_bottom)
        return true;

    while (!m_reader->hasError() && m_reader->hasNext() && m_bottom<index) {
        QVariant value = m_reader->read();
        if(m_forwardOnly && !m_cache.isEmpty())
            m_cache.replace(0, std::move(value));
        else
            m_cache.append(std::move(value));
        m_bottom++;
    }

    if(m_reader->hasError())
    {
        setCode(m_reader->errorCode());
        setError(m_reader->errorString());
        emit this->error();
    }

    if(!m_reader->hasNext())
        m_rowCount = m_bottom+1;

    return index<=m_bottom;
}

bool QVariantReaderModel::initilized()
{
    m_initialized = false;

    if(!m_reader)
        return false;

    if(!m_reader->isList())
        return false;

    beginResetModel();

    m_cache.clear();
    m_bottom = -1;

    if(m_reader->isLengthKnown())
        m_rowCount = m_reader->length();
    else
        m_rowCount = -1;

    if(m_forwardOnly)
        m_cache.reserve(1);
    else
        m_cache.reserve(m_rowCount);

    m_reader->enterContainer();
    m_initialized = true;

    if(seek(0))
        updateRoleNames(m_cache.first());

    endResetModel();

    return true;
}

void QVariantReaderModel::countInvalidate()
{
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
