#include "sqltablemodel.h"
#include "sqlquerybuilder.h"
#include "sqldbpool.h"
#include "sql_log.h"

SqlTableModel::SqlTableModel(QObject *parent):
    QSqlTableModel(parent,SqlDbPool::defaultDatabase())
{
    connect(this, &QAbstractItemModel::rowsInserted, this, &SqlTableModel::countInvalidate);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &SqlTableModel::countInvalidate);
    connect(this, &QAbstractItemModel::modelReset, this, &SqlTableModel::countInvalidate);
    connect(this, &QAbstractItemModel::layoutChanged, this, &SqlTableModel::countInvalidate);

    connect(this, &SqlTableModel::tableNameChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::rawFilterChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::filterRoleNameChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::filterPatternChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::filterValueChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::limitChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::sortColumnChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::sortRoleNameChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::sortOrderChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::delayedChanged, this, &SqlTableModel::queueInvalidate);
    connect(this, &SqlTableModel::enabledChanged, this, &SqlTableModel::queueInvalidate);

    QMetaObject::invokeMethod(this, &SqlTableModel::countInvalidate, Qt::QueuedConnection);

    setEditStrategy(OnRowChange);

    connect(this, &SqlTableModel::tableNameChanged, this, [this](const QString& tableName){
        QSqlRecord rec = database().record(getTableName());
        m_roleNames.clear();
        for(int i=0;i<rec.count();i++)
            m_roleNames.insert(i+Qt::UserRole, rec.fieldName(i).toUtf8());
    });
}

SqlTableModel::~SqlTableModel()
{
    submit();
}

QVariant SqlTableModel::data(const QModelIndex &index, int role) const
{
    int column = 0;
    if (role >= Qt::UserRole)
        column = role - Qt::UserRole;
    else
        column = index.column();

    QVariant ret = QSqlTableModel::data(QSqlTableModel::index(index.row(),column),Qt::DisplayRole);

    if(!ret.isValid() && column < record().count())
        ret.setValue(QVariant(record().field(column).metaType()));

    return ret;
}
bool SqlTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int column = 0;
    int row = index.row();
    if (role >= Qt::UserRole)
        column = role - Qt::UserRole;
    else
        column = index.column();

    if(m_dirtyRow>=0 && m_dirtyRow!=row)
        submit();

    bool ret = QSqlTableModel::setData(QSqlTableModel::index(row,column), value, Qt::EditRole);

    if(isDirty())
        m_dirtyRow = row;

    if(ret)
        queueSubmit();

    return ret;
}

int SqlTableModel::columnIndex(const QString &pColumnName) const
{
    return roleNames().key(pColumnName.toUtf8(),-1);
}
QString SqlTableModel::columnName(int pColumn) const
{
    int aColumn=-1;
    QString roleName="";

    if(pColumn >= Qt::UserRole)
        aColumn = pColumn-Qt::UserRole;
    else
        aColumn = pColumn;

    roleName = QString::fromUtf8(roleNames().value(aColumn+Qt::UserRole));
    return roleName;
}

bool SqlTableModel::select()
{
    m_selectQueued = false;

    if(!getEnabled())
        return false;

    if (getTableName().isEmpty())
    {
        SQLLOG_WARNING()<<"No table name has been given to the model";
        return false;
    }

    QElapsedTimer aElapsedTimer;
    aElapsedTimer.start();

    if(isDirty())
        submit();

    beginResetModel();

    bool result = QSqlTableModel::select();

    if(canFetchMore())
    {
        SQLLOG_TRACE()<<getTableName()<<"canFetchMore";

        while(canFetchMore())
            fetchMore();
    }

    if(this->lastError().type()!=QSqlError::NoError)
    {
        SQLLOG_WARNING()<<"Error while fetching the model:"<<getTableName()<<this->query().lastQuery()<<this->query().lastError();
    }

    SQLLOG_DEBUG()<<"The select of"<<getTableName()<<"took"<<aElapsedTimer.nsecsElapsed()/1000000.0<<"ms";

    endResetModel();

    return result;
}

QHash<int, QByteArray> SqlTableModel::roleNames() const
{
    return m_roleNames;
}

void SqlTableModel::classBegin()
{

}
void SqlTableModel::componentComplete()
{
    this->invalidate();
}

void SqlTableModel::queueSubmit()
{
    if (!m_submitQueued)
    {
        m_submitQueued = true;
        QMetaObject::invokeMethod(this, &SqlTableModel::submit, Qt::QueuedConnection);
    }
}
bool SqlTableModel::submit()
{
    m_submitQueued = false;
    m_dirtyRow=-1;

    if(!isDirty())
        return true;

    QElapsedTimer aTimer;
    aTimer.start();

    bool aRet = QSqlTableModel::submit();

    SQLLOG_DEBUG()<<"The submit of"<<getTableName()<<"took"<<aTimer.nsecsElapsed()/1000000.0<<"ms";

    return aRet;
}

QString SqlTableModel::selectStatement() const
{
    QString final = QSqlTableModel::selectStatement();
    if(final.isEmpty())
        return QString();

    final = Sql::concat(final,Sql::limit(this->getLimit()));
    final = Sql::concat(final,Sql::offset(this->getOffset()));

    SQLLOG_TRACE()<<final;

    return final;
}

void SqlTableModel::queueInvalidate()
{
    bool wasActive=isActive();

    if(wasActive)
        clear();

    setTable(getTableName());

    QSqlIndex primaryIndex = database().primaryIndex(getTableName());
    if(!primaryIndex.isEmpty())
        setPrimaryField(primaryIndex.fieldName(0));

    QString aFilter;
    if(!getRawFilter().isEmpty())
    {
        aFilter = Sql::where(getRawFilter());
    }
    else if(!getFilterMap().isEmpty())
    {
        aFilter = Sql::whereStatement(getTableName(), getFilterMap(), database().driver());
    }
    else if(!getFilterRoleName().isEmpty() && !getFilterValue().isNull())
    {
        aFilter = Sql::whereStatement(getTableName(), {{getFilterRoleName(),getFilterValue()}}, database().driver());
    }
    else if(!getFilterRoleName().isEmpty() && !getFilterPattern().isEmpty())
    {
        aFilter = Sql::where(QString("%1 LIKE '%%2%'")
                                .arg(Sql::prepareIdentifier(getFilterRoleName(),QSqlDriver::FieldName,database().driver()),
                                     Sql::formatValue(getFilterPattern(), database().driver())));
    }
    aFilter.remove("WHERE ");

    if(getFilterInverted())
    {
        aFilter = Sql::inv(aFilter);
    }

    if(!aFilter.isEmpty())
        this->setFilter(aFilter);

    int aSortColumn=0;
    if(getSortColumn()>0)
        aSortColumn=getSortColumn();
    else if(getSortRoleName()!="")
        aSortColumn=record().indexOf(getSortRoleName());

    if(aSortColumn>0)
        this->setSort(aSortColumn, getSortOrder());

    if(wasActive)
        invalidate();
}
void SqlTableModel::invalidate()
{
    emit aboutToBeInvalidated();

    if (m_delayed)
    {
        if (!m_selectQueued)
        {
            m_selectQueued = true;
            QMetaObject::invokeMethod(this, &SqlTableModel::select, Qt::QueuedConnection);
        }
    }
    else
    {
        select();
    }

    emit invalidated();
}

bool SqlTableModel::isActive() const
{
    return query().isActive() && getEnabled();
}
bool SqlTableModel::clearTable()
{
    bool result = false;
    SqlBuilder::truncate().table(m_tableName).exec(&result);

    queueInvalidate();

    return result;
}
bool SqlTableModel::remove(int row, int count)
{
    if(isDirty())
        submit();

    setEditStrategy(OnManualSubmit);
    bool result = QSqlTableModel::removeRows(row,count);
    submitAll();
    setEditStrategy(OnFieldChange);

    if(this->lastError().type()!=QSqlError::NoError) {
        SQLLOG_WARNING()<<this->lastError()<<result;
    }

    return result;
}
bool SqlTableModel::remove(const QList<int>& indexes)
{
    int primaryRole = columnIndex(getPrimaryField().toUtf8());

    QVariantList primaryValues;
    for(int index: indexes)
    {
        primaryValues+=this->data(this->index(index, 0), primaryRole);
    }

    bool result = false;
    SqlBuilder::delete_().from(m_tableName).where(m_primaryField, primaryValues).exec(&result);

    invalidate();

    return result;
}
bool SqlTableModel::append(QJSValue value)
{
    if(value.isObject())
    {
        const QVariant& variant = value.toVariant();
        if(variant.metaType() == QMetaType(QMetaType::QVariantList))
        {
            return append(variant.toList());
        }
        else
        {
            return append(variant.toMap());
        }
    }

    SQLLOG_WARNING()<<"Fail to append"<<value.toString()<<"Wrong data type";
    return false;
}

bool SqlTableModel::append(const QVariantMap& row)
{
    QVariantMap preparredRow = prepareArray(row);
    QSqlRecord record = Sql::prepareRecord(getTableName(), preparredRow, database().driver());

    if(isDirty())
        submit();

    bool result = false;
    result=insertRecord(-1,record);

    if(!result) {
        SQLLOG_WARNING()<<this->lastError()<<record;
    }

    return result;
}
bool SqlTableModel::append(const QVariantList& rows)
{
    if(!isActive())
        return false;

    if(rows.isEmpty())
        return true;

    QVariantList preparredRows;
    preparredRows.reserve(rows.size());
    for(const QVariant& row: rows)
    {
        preparredRows.append(prepareArray(row.toMap()));
    }

    bool result = false;
    QSqlQuery reply = SqlBuilder::insert(preparredRows).into(m_tableName).exec(&result);
    setLastError(reply.lastError());

    if(result)
    {
        invalidate();
    }
    else
    {
        SQLLOG_WARNING()<<this->lastError();
    }

    return result;
}

QVariantMap SqlTableModel::prepareArray(const QVariantMap& map)
{
    QVariantMap correctedMap = map;

    if(!m_primaryField.isEmpty())
        correctedMap.remove(m_primaryField);

    return correctedMap;
}

void SqlTableModel::countInvalidate()
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
