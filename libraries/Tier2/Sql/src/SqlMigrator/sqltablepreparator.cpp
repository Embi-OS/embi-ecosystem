#include "sqltablepreparator.h"
#include "sqlquerybuilder.h"
#include "sqldbpool.h"
#include "sql_log.h"

SqlTablePreparator::SqlTablePreparator(QObject* parent) :
    SqlTablePreparator("", parent)
{

}

SqlTablePreparator::SqlTablePreparator(const QString& name, QObject* parent) :
    QObject(parent),
    m_name(name),
    m_columns(this),
    m_indexes(this)
{

}

bool SqlTablePreparator::isValid() const
{
    return (!m_name.isEmpty()) && (!m_columns.isEmpty()) && getPrimaryColumn();
}

bool SqlTablePreparator::hasColumn(const QString &columnName) const
{
    for(SqlColumnPreparator* column: m_columns)
    {
        if(column->getName()==columnName)
            return true;
    }

    return false;
}

SqlColumnPreparator* SqlTablePreparator::getPrimaryColumn() const
{
    for(SqlColumnPreparator* column: m_columns)
    {
        if(column->isPrimary())
            return column;
    }

    return nullptr;
}

bool SqlTablePreparator::init(const QString& connectionName)
{
    if(!SqlDbPool::database(connectionName, false).isValid())
    {
        SQLLOG_CRITICAL()<<"Invalid connection name given to SqlTablePreparator::init:"<<connectionName;
        return false;
    }
    if(!SqlDbPool::database(connectionName, false).isOpen())
    {
        SQLLOG_CRITICAL()<<"Connection name"<<connectionName<<"is closed";
        return false;
    }

    m_connectionName = connectionName;
    m_driver = SqlDbPool::driver(m_connectionName);

    if(!m_driver)
    {
        SQLLOG_CRITICAL()<<"Invalid driver:"<<m_driver;
        return false;
    }

    return true;
}

bool SqlTablePreparator::create()
{
    SQLLOG_INFO()<<"Creating table:"<<m_name;

    bool result = false;
    const QString definition = generateColumnsDefinition();
    SqlBuilder::create().table(m_name).ifNotExists().definition(definition).connection(m_connectionName).exec(&result);

    if(result)
        result = createIndexes();

    return result;
}

bool SqlTablePreparator::createIndexes()
{
    bool result = true;

    // 1. Lister les index actuels
    QStringList currentIndexes;
    QSqlQuery query = SqlBuilder::indexList().table(m_name).connection(m_connectionName).exec(&result);
    while (query.next())
        currentIndexes << SqlBuilder::value(query, query.at(), "Key_name").toString();

    // 2. Index déclarés dans ton code
    QStringList declaredIndexes;
    for (SqlIndexPreparator* index : m_indexes)
        declaredIndexes << QString("idx_%1_%2").arg(m_name, index->getFields().join('_'));

    // 3. Déterminer les index à supprimer (présents en DB mais pas déclarés)
    QStringList indexToRemove;
    for (const QString& index : currentIndexes)
    {
        if (!index.startsWith("idx_"))
            continue;
        if (!declaredIndexes.contains(index))
            indexToRemove << index;
    }

    // 4. Supprimer les index obsolètes
    for (const QString& index : indexToRemove)
    {
        SQLLOG_DEBUG()<<"Dropping index"<<index;
        SqlBuilder::dropIndex(index).ifExists().table(m_name).exec(&result);
    }

    // 5. Créer les index manquants
    bool indexCreated = false;
    for (SqlIndexPreparator* index : m_indexes)
    {
        QStringList fields = index->getFields();

        const QString indexName = QString("idx_%1_%2").arg(m_name, fields.join('_'));
        if (currentIndexes.contains(indexName))
            continue; // déjà présent

        for (QString& field : fields) {
            field = Sql::prepareIdentifier(field, QSqlDriver::FieldName, m_driver);
        }

        SQLLOG_INFO()<<"Creating index"<<indexName<<"on table:"<<m_name;
        SqlBuilder::createIndex(indexName).ifNotExists().table(m_name).field(fields.join(',')).exec(&result);

        if (!result) {
            SQLLOG_WARNING()<<"Failed to create index"<<indexName;
        }

        indexCreated = true;
    }

    if(indexCreated)
    {
        SQLLOG_INFO()<<"Analyze table:"<<m_name;
        SqlBuilder::analyze().table(m_name);
    }

    return result;
}

bool SqlTablePreparator::drop()
{
    SQLLOG_INFO()<<"Dropping table:"<<m_name;

    bool result = false;
    SqlBuilder::drop().table(m_name).connection(m_connectionName).exec(&result);

    return result;
}

bool SqlTablePreparator::update()
{
    bool result = true;

    if(!isDefinitionEqualToExistingTable())
    {
        SQLLOG_INFO()<<"Updating table schema:"<<m_name;

        SQLLOG_NOTICE()<<"Creating table:"<<m_name+"_new";
        const QString definition = generateColumnsDefinition();
        SqlBuilder::create().table(m_name+"_new").ifNotExists().definition(definition).connection(m_connectionName).exec(&result);

        SQLLOG_NOTICE()<<"Copying table:"<<m_name<<"to"<<m_name+"_new";
        SqlBuilder::copy().table(m_name).into(m_name+"_new").connection(m_connectionName).exec(&result);

        SQLLOG_NOTICE()<<"Droping table:"<<m_name;
        SqlBuilder::drop().table(m_name).ifExists().connection(m_connectionName).exec(&result);

        SQLLOG_NOTICE()<<"Renaming table:"<<m_name+"_new"<<"to"<<m_name;
        SqlBuilder::alter().table(m_name+"_new").rename(m_name).connection(m_connectionName).exec(&result);

        result = true;
    }

    if(result)
        result = createIndexes();

    return result;
}

bool SqlTablePreparator::truncate()
{
    SQLLOG_INFO()<<"Clearing table:"<<m_name;

    bool result = false;
    SqlBuilder::truncate().table(m_name).connection(m_connectionName).exec(&result);

    return result;
}

bool SqlTablePreparator::updatePrimary()
{
    SqlColumnPreparator* column = getPrimaryColumn();
    if(!column)
    {
        SQLLOG_WARNING()<<"No primary column was given to the table"<<m_name;
        return false;
    }

    SqlPrimaryColumnPreparator* primaryColumn = qobject_cast<SqlPrimaryColumnPreparator*>(column);
    if(!primaryColumn)
        return true;

    bool result=false;
    QSqlQuery query = SqlBuilder::autoIncrement().table(m_name).connection(m_connectionName).exec(&result);
    int autoIncrement = SqlBuilder::value(query, 0, "AUTO_INCREMENT").toInt();
    if(result && autoIncrement>=primaryColumn->getStartValue())
        return true;

    SQLLOG_INFO()<<"Updating primary in table:"<<m_name;

    QVariantMap map;
    map.insert(primaryColumn->getName(), primaryColumn->getStartValue());

    bool insert = SqlBuilder::insert(map).into(m_name).connection(m_connectionName).exec().result();
    bool remove = SqlBuilder::delete_().from(m_name).where(map).connection(m_connectionName).exec().result();

    return insert && remove;
}

bool SqlTablePreparator::fillDefaultEntries()
{
    if(m_defaultEntries.isEmpty())
        return true;

    bool result = false;
    SqlBuilder::insert(m_defaultEntries).into(m_name).replace().connection(m_connectionName).exec(&result);

    return result;
}

bool SqlTablePreparator::fillBasicEntries()
{
    if(m_basicEntries.isEmpty())
        return true;

    bool result = false;
    SqlBuilder::insert(m_basicEntries).into(m_name).ignore().connection(m_connectionName).exec(&result);

    return result;
}

bool SqlTablePreparator::doAll()
{
    if(!create())
        return false;

    if(!updatePrimary())
        return false;

    if(!fillDefaultEntries())
        return false;

    if(!fillBasicEntries())
        return false;

    return true;
}

bool SqlTablePreparator::reDoAll()
{
    if(!drop())
        return false;

    if(!doAll())
        return false;

    return true;
}

QString SqlTablePreparator::generateColumnsDefinition() const
{
    QStringList sqlColumnDefinitions;
    for(SqlColumnPreparator* column: m_columns)
    {
        QString sqlColumnDefinition = column->generateDefinition(m_driver);
        if(!sqlColumnDefinition.isEmpty())
            sqlColumnDefinitions.append(sqlColumnDefinition);
    }
    return sqlColumnDefinitions.join(", ");
}

bool SqlTablePreparator::isDefinitionEqualToExistingTable()
{
    QSqlQuery query = SqlBuilder::tableCreation().table(m_name).connection(m_connectionName).exec();
    if(m_driver->dbmsType()==QSqlDriver::SQLite)
    {
        const QString currentCreate = SqlBuilder::value(query,0,"CREATION_STMT").toString();
        const QString definition = generateColumnsDefinition();
        const QString newCreate = SqlBuilder::create().table(m_name).definition(definition).connection(m_connectionName).build();

        if(newCreate!=currentCreate)
        {
            SQLLOG_WARNING()<<m_name<<"table definition differs from the existing one";
            return false;
        }
    }
    else if(m_driver->dbmsType()==QSqlDriver::MySqlServer)
    {
        QSqlRecord tableRecord = m_driver->record(m_name);
        if(m_columns.count()!=tableRecord.count())
        {
            SQLLOG_WARNING()<<"columns count does not match between table"<<m_name<<"and the existing one";
            return false;
        }
        else
        {
            for(int i = 0; i < m_columns.count(); i++)
            {
                SqlColumnPreparator* column = m_columns.at(i);
                // Comparison is case sensitive so it's easier ton change case for later QSqlField
                if(QString::compare(column->getName(), tableRecord.field(i).name(), Qt::CaseSensitive)!=0)
                {
                    SQLLOG_WARNING()<<"columns names does not match between table"<<m_name<<"and the existing one";
                    return false;
                }
                if(column->getType()==SqlColumnTypes::Json && tableRecord.field(i).metaType().id()!=QMetaType::QByteArray)
                {
                    SQLLOG_WARNING()<<"column type does not match for field"<<column->getName()<<"and the existing one";
                    return false;
                }
                // TODO: compare type
                // TODO: compare default value
                // TODO: compare isAutoIncrement
                // TODO: compare isNullable
                // TODO: compare isPrimary
                // TODO: compare isUnique
            }
        }
    }

    return true;
}
