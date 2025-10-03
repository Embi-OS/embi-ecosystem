#include "QsLogDestModel.h"

#include "QsLog.h"

QsLogging::ModelDestination::ModelDestination(QObject *parent) :
    QAbstractListModel(parent),
    m_logLevel(QsLogging::ModelDestination::NoticeLevel),
    m_limit(0)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &QsLogging::ModelDestination::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &QsLogging::ModelDestination::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &QsLogging::ModelDestination::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &QsLogging::ModelDestination::countInvalidate);

    m_roleNames[Qt::DisplayRole] ="display";
    m_roleNames[MessageRole] ="message";
    m_roleNames[TimeRole] ="time";
    m_roleNames[LevelRole] ="level";
    m_roleNames[FileRole] ="file";
    m_roleNames[FunctionRole] ="function";
    m_roleNames[CategoryRole] ="category";
    m_roleNames[LineRole] ="line";
    m_roleNames[ColorRole] ="color";

    m_functorDest = DestinationFactory::MakeFunctorDestination([this](const QsLogging::Message& message) mutable {
        addEntry(message);
    }, [this]() mutable {
        clear();
    });

    m_functorDest->setLoggingLevel(QsLogging::Level(m_logLevel));
    m_functorDest->setIncludeLine(false);
    m_functorDest->setIncludeFunction(false);
    m_functorDest->setIncludeColor(false);
    m_functorDest->setIncludeTimestamp(false);
    Logger::instance().addDestination(m_functorDest);

    connect(this, &ModelDestination::logLevelChanged, this, [this](QsLogging::ModelDestination::Level logLevel) {
        m_functorDest->setLoggingLevel(QsLogging::Level(logLevel));
    });
}

QsLogging::ModelDestination::~ModelDestination()
{
    Logger::instance().removeDestination(m_functorDest);
    m_functorDest.clear();
}

QVariant QsLogging::ModelDestination::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() >= m_messages.size())
        return QVariant();

    const QsLogging::Message message = m_messages.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return message.display;
    case MessageRole:
        return message.message;
    case TimeRole:
        return Message::formatTime(message.time);
    case LevelRole:
        return Message::formatLevel(message.level);
    case FileRole:
        return Message::formatFile(message.file);
    case FunctionRole:
        return Message::formatFunction(message.function);
    case CategoryRole:
        return Message::formatCategory(message.category);
    case LineRole:
        return Message::formatLine(message.line);
    case ColorRole:
        return QColor(message.color().rgb());
    }

    return QVariant();
}

void QsLogging::ModelDestination::addEntry(const QsLogging::Message& message)
{
    if(m_limit==1 && rowCount()==1)
    {
        const QModelIndex index = this->index(0);
        m_messages.replace(0, message);
        emit this->dataChanged(index, index);
        setLastMessage(message);
    }
    else
    {
        beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
        m_messages.append(message);
        endInsertRows();
        queueInvalidate();
    }
}

void QsLogging::ModelDestination::clear()
{
    beginResetModel();
    m_messages.clear();
    endResetModel();
    queueInvalidate();
}

void QsLogging::ModelDestination::queueInvalidate()
{
    if(!m_enabled)
        return;

    if (m_delayed) {
        if(m_invalidateQueued)
            return;
        m_invalidateQueued = true;
        QMetaObject::invokeMethod(this, &QsLogging::ModelDestination::invalidate, Qt::QueuedConnection);
    } else {
        invalidate();
    }
}

void QsLogging::ModelDestination::invalidate()
{
    m_invalidateQueued = false;

    if(m_limit > 0 && rowCount() > m_limit)
    {
        beginRemoveRows(QModelIndex(), 0, rowCount()-m_limit);
        while (m_limit > 0 && rowCount() > m_limit)
        {
            m_messages.removeFirst();
        }
        endRemoveRows();
    }

    if(!m_messages.isEmpty())
        setLastMessage(m_messages.last());
}

void QsLogging::ModelDestination::countInvalidate()
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

int QsLogging::ModelDestination::rowCount(const QModelIndex &index) const
{
    return index.isValid() ? 0 : m_messages.size();
}

QHash<int, QByteArray> QsLogging::ModelDestination::roleNames() const
{
    return m_roleNames;
}

QsLogging::Message QsLogging::ModelDestination::at(int index) const
{
    if(index < 0 || index >= rowCount())
        return QsLogging::Message();

    return m_messages.at(index);
}

bool QsLogging::ModelDestination::delayed() const
{
    return m_delayed;
}

void QsLogging::ModelDestination::setDelayed(bool delayed)
{
    if (m_delayed == delayed)
        return;
    m_delayed = delayed;
    emit delayedChanged(m_delayed);
}

bool QsLogging::ModelDestination::enabled() const
{
    return m_enabled;
}

void QsLogging::ModelDestination::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;
    m_enabled = enabled;
    emit enabledChanged(m_enabled);
    queueInvalidate();
}

QsLogging::ModelDestination::Level QsLogging::ModelDestination::getLogLevel() const
{
    return m_logLevel;
}

void QsLogging::ModelDestination::setLogLevel(QsLogging::ModelDestination::Level logLevel)
{
    if(m_logLevel==logLevel)
        return;
    m_logLevel = logLevel;
    emit logLevelChanged(m_logLevel);
}

int QsLogging::ModelDestination::getLimit() const
{
    return m_limit;
}

void QsLogging::ModelDestination::setLimit(int limit)
{
    if (m_limit == limit)
        return;
    m_limit = limit;
    emit limitChanged(m_limit);
    queueInvalidate();
}

const QsLogging::Message& QsLogging::ModelDestination::getLastMessage() const
{
    return m_lastMessage;
}

void QsLogging::ModelDestination::setLastMessage(const QsLogging::Message &lastMessage)
{
    if (m_lastMessage == lastMessage)
        return;
    m_lastMessage = lastMessage;

    emit lastMessageChanged(m_lastMessage);
}
