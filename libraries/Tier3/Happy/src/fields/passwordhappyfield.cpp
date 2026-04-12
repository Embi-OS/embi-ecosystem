#include "passwordhappyfield.h"
#include "happycrudrouter.h"
#include "happy_log.h"

PasswordHappyField::PasswordHappyField(QObject* parent):
    VarHappyField(parent),
    m_hasher(new QPasswordHasher(this))
{
    m_hasher->setIterations(100000);
    m_hasher->setSaltLength(16);
    m_hasher->setHashLength(32);

    m_type = SqlColumnTypes::Char;
    m_precision = 128;

    m_sqlColumnPreparator->setType(m_type);
    m_sqlColumnPreparator->setOptions(m_options);
    m_sqlColumnPreparator->setPrecision(m_precision);
}

bool PasswordHappyField::init(HappyCrudRouter* crudRouter, HappyServer* happyServer)
{
    VarHappyField::init(crudRouter, happyServer);

    return true;
}

QVariant PasswordHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    if(!values.contains(m_name) && !creation)
    {
        if(ok)
            *ok = false;
        return QVariant();
    }

    QVariant value;

    const QString password = formatWrite(values.value(m_name), ok).toString();
    const QString stored = m_hasher->makePassword(password);

    if(ok)
        *ok = true;

    return stored;
}
