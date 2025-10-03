#include "sqlbrowser.h"
#include "sqlbrowsertablemodel.h"

#include <Sql>
#include <QtSql>

SqlBrowser::SqlBrowser(QObject *parent):
    QObject(parent),
    m_tableNames(new QStringListModel(this)),
    m_connectionNames(new QStringListModel(this))
{
    connect(this, &SqlBrowser::connectionChanged, this, &SqlBrowser::queueInvalidate);
}

QSqlDatabase SqlBrowser::database() const
{
    return SqlDbPool::database(m_connection);
}

const QSqlDriver* SqlBrowser::driver() const
{
    return database().driver();
}

void SqlBrowser::classBegin()
{

}

void SqlBrowser::componentComplete()
{
    this->invalidate();
}

void SqlBrowser::queueInvalidate()
{
    if(m_delayed)
    {
        if (!m_selectQueued)
        {
            m_selectQueued = true;
            QMetaObject::invokeMethod(this, &SqlBrowser::invalidate, Qt::QueuedConnection);
        }
    }
    else
    {
        invalidate();
    }
}

void SqlBrowser::invalidate()
{
    m_tableNames->setStringList(database().tables(QSql::AllTables));
    m_connectionNames->setStringList(database().connectionNames());

    if(m_model)
        m_model->deleteLater();

    setModel(nullptr);
    setEditable(false);
}

void SqlBrowser::showTable(const QString &t)
{
    if(m_model)
    {
        m_model->deleteLater();
        m_model=nullptr;
    }

    SqlBrowserTableModel *model = new SqlBrowserTableModel(this, database());
    model->setTable(driver()->escapeIdentifier(t, QSqlDriver::TableName));
    model->select();
    if (model->lastError().type() != QSqlError::NoError)
        emit statusMessage(model->lastError().text());

    setModel(model);
    setEditable(true);
}

void SqlBrowser::showMetaData(const QString &t)
{
    if(m_model)
    {
        m_model->deleteLater();
        m_model=nullptr;
    }

    QSqlRecord rec = database().record(t);
    QStandardItemModel *model = new QStandardItemModel(this);

    model->insertRows(0, rec.count());
    model->insertColumns(0, 7);

    model->setHeaderData(0, Qt::Horizontal, "Fieldname");
    model->setHeaderData(1, Qt::Horizontal, "Type");
    model->setHeaderData(2, Qt::Horizontal, "Length");
    model->setHeaderData(3, Qt::Horizontal, "Precision");
    model->setHeaderData(4, Qt::Horizontal, "Required");
    model->setHeaderData(5, Qt::Horizontal, "AutoValue");
    model->setHeaderData(6, Qt::Horizontal, "DefaultValue");

    for (int i = 0; i < rec.count(); ++i) {
        QSqlField fld = rec.field(i);
        model->setData(model->index(i, 0), fld.name());
        model->setData(model->index(i, 1), QString("%1 (%2)").arg(fld.metaType().name()).arg(fld.metaType().id()));
        model->setData(model->index(i, 2), fld.length());
        model->setData(model->index(i, 3), fld.precision());
        model->setData(model->index(i, 4), fld.requiredStatus() == -1 ? QVariant("?") : QVariant(bool(fld.requiredStatus())));
        model->setData(model->index(i, 5), fld.isAutoValue());
        model->setData(model->index(i, 6), fld.defaultValue());
    }

    setModel(model);
    setEditable(false);
}
