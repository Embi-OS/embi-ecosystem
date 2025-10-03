#include "uuidhappyfield.h"
#include "happycrudrouter.h"
#include "happy_log.h"

UuidHappyField::UuidHappyField(QObject* parent):
    VarHappyField(parent)
{
    m_type = SqlColumnTypes::Uuid;
    m_options = SqlColumnOptions::Unique;

    m_sqlColumnPreparator->setType(m_type);
    m_sqlColumnPreparator->setOptions(m_options);
}

bool UuidHappyField::init(HappyCrudRouter* crudRouter, HappyServer* happyServer)
{
    VarHappyField::init(crudRouter, happyServer);

    connect(m_crudRouter, &HappyCrudRouter::objectInserted, this, &UuidHappyField::onObjectInserted);

    return true;
}

QVariant UuidHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    if(!values.contains(m_name) && !creation)
    {
        if(ok)
            *ok = false;
        return QVariant();
    }

    QVariant value;

    const QString uuid = formatWrite(values.value(m_name), ok).toString();

    if(uuid.isEmpty() && creation)
    {
        value = formatWrite(QUuid::createUuid().toString(QUuid::WithoutBraces));

        if(ok)
            *ok = true;
    }
    else
        value = uuid;

    return value;
}

void UuidHappyField::onObjectInserted(const QVariant& primaryValue, const QVariantMap& object)
{
    if(!m_simplify)
        return;

    const int luid = primaryValue.toInt();
    const QVariant uuid = formatWrite(uuidFromUInt128(luid).toString(QUuid::WithoutBraces));

    SqlBuilder::update(m_crudRouter->getTableName()).set(m_name, uuid).where(m_crudRouter->getPrimaryField(), luid).connection(m_connection).trust().exec();
}
