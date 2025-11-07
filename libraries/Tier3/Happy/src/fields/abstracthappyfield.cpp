#include "abstracthappyfield.h"
#include "happyserver.h"

AbstractHappyField::AbstractHappyField(QObject* parent):
    QObject(parent)
{
    connect(this, &AbstractHappyField::nameChanged, this, [this](const QString& name){
        m_nameJson = QUtils::Json::variantToJson(name);
        m_nameCbor = name.toUtf8();
    });
}

int AbstractHappyField::nameIndex() const
{
    return m_nameIndex;
}

const QByteArray& AbstractHappyField::nameJson() const
{
    return m_nameJson;
}

const QByteArray& AbstractHappyField::nameCbor() const
{
    return m_nameCbor;
}

bool AbstractHappyField::init(HappyCrudRouter* crudRouter, HappyServer* happyServer)
{
    m_connection = happyServer->getSqlConnection();
    m_crudRouter = crudRouter;
    m_happyServer = happyServer;

    return true;
}

void AbstractHappyField::preRead(const HappyHttpParameters& parameter, const QSqlRecord& baseRec)
{
    m_nameIndex = baseRec.indexOf(m_name);
}

void AbstractHappyField::preWrite(const HappyHttpParameters& parameters)
{

}

QVariant AbstractHappyField::formatValue(const QVariant& value, bool* ok) const
{
    if(ok)
        *ok = true;
    return std::move(value);
}

QVariant AbstractHappyField::formatRead(const QVariant& value, bool* ok) const
{
    if(ok)
        *ok = true;
    return std::move(value);
}

QVariant AbstractHappyField::formatWrite(const QVariant& value, bool* ok) const
{
    if(ok)
        *ok = true;
    return std::move(value);
}
