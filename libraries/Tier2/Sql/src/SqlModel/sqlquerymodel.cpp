#include "sqlquerymodel.h"
#include "sqldbpool.h"
#include "sql_log.h"

SqlQueryModel::SqlQueryModel(QObject *parent):
    QSqlQueryModel(parent)
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &SqlQueryModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &SqlQueryModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &SqlQueryModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &SqlQueryModel::countInvalidate);

    connect(this, &SqlQueryModel::connectionChanged, this, &SqlQueryModel::queueInvalidate);
    connect(this, &SqlQueryModel::rawStatementChanged, this, &SqlQueryModel::queueInvalidate);
    connect(this, &SqlQueryModel::delayedChanged, this, &SqlQueryModel::queueInvalidate);
    connect(this, &SqlQueryModel::enabledChanged, this, &SqlQueryModel::queueInvalidate);

    QMetaObject::invokeMethod(this, &SqlQueryModel::countInvalidate, Qt::QueuedConnection);
}

QVariant SqlQueryModel::data(const QModelIndex &index, int role) const
{
    int column = 0;
    if (role >= Qt::UserRole)
        column = role - Qt::UserRole;
    else
        column = index.column();

    QVariant ret = QSqlQueryModel::data(QSqlQueryModel::index(index.row(),column),Qt::DisplayRole);

    if(!ret.isValid() && column < record().count())
        ret.setValue(QVariant(baseRecord().field(column).metaType()));

    return ret;
}

bool SqlQueryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    SQLLOG_WARNING()<<"This is a read only model";

    return QSqlQueryModel::setData(index, value, role);;
}

int SqlQueryModel::columnIndex(const QString &columnName) const
{
    return roleNames().key(columnName.toUtf8(),-1);
}
QString SqlQueryModel::columnName(int column) const
{
    QString roleName;

    if(column >= Qt::UserRole)
        column = column-Qt::UserRole;

    roleName = QString::fromUtf8(roleNames().value(column+Qt::UserRole));
    return roleName;
}

QHash<int, QByteArray> SqlQueryModel::roleNames() const
{
    return m_roleNames;
}

QSqlDatabase SqlQueryModel::database() const
{
    return SqlDbPool::database(m_connection);
}

const QSqlDriver* SqlQueryModel::driver() const
{
    return database().driver();
}

QSqlRecord SqlQueryModel::baseRecord() const
{
    return record();
}

void SqlQueryModel::classBegin()
{

}
void SqlQueryModel::componentComplete()
{
    invalidate();
}

void SqlQueryModel::queryChange()
{
    // m_roleNames.clear();
    // for(int i=0;i<record().count();i++)
    //     m_roleNames.insert(i+Qt::UserRole, record().fieldName(i).toUtf8());
}

QString SqlQueryModel::queryStatement()
{
    QString final;

    final = getRawStatement();

    SQLLOG_TRACE()<<final;

    return final;
}

void SqlQueryModel::queueInvalidate()
{
    if(!m_isActive)
        return;

    if(m_delayed)
    {
        if (!m_selectQueued)
        {
            m_selectQueued = true;
            QMetaObject::invokeMethod(this, &SqlQueryModel::invalidate, Qt::QueuedConnection);
        }
    }
    else
    {
        invalidate();
    }
}
void SqlQueryModel::invalidate()
{
    m_selectQueued = false;
    m_isActive = true;
    select();
}

bool SqlQueryModel::select()
{
    if(m_isQuerying)
        return false;

    m_selectQueued = false;

    if(!m_enabled)
        return false;

    return setRawQuery(queryStatement());
}

void SqlQueryModel::markDirty()
{
    m_isActive = false;
}

bool SqlQueryModel::isActive() const
{
    return m_isActive;
}

bool SqlQueryModel::isValid() const
{
    return isActive() && m_isValid;
}

void SqlQueryModel::countInvalidate()
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

bool SqlQueryModel::setRawQuery(const QString &query)
{
    if (query.isEmpty())
        return false;

    QElapsedTimer aElapsedTimer;
    aElapsedTimer.start();

    setIsQuerying(true);
    emit aboutToBeInvalidated();

    QSqlQuery sqlQuery = QSqlQuery(query, database());

    m_roleNames.clear();
    for(int i=0;i<sqlQuery.record().count();i++)
        m_roleNames.insert(i+Qt::UserRole, sqlQuery.record().fieldName(i).toUtf8());

    beginResetModel();

    setQuery(std::move(sqlQuery));

    if(!this->query().isActive() || lastError().isValid())
    {
        SQLLOG_WARNING()<<"Something went wrong - revert to non-exec state";
    }
    else
    {
        if(canFetchMore())
        {
            SQLLOG_TRACE()<<"canFetchMore";

            while(canFetchMore())
                fetchMore();
        }
    }

    m_isActive = true;
    m_isValid = this->query().isActive();

    endResetModel();
    emit invalidated();
    setIsQuerying(false);

    SQLLOG_DEBUG()<<"The query took"<<aElapsedTimer.nsecsElapsed()/1000000.0<<"ms";

    return true;
}
