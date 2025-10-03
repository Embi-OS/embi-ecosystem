#include "primaryhappyfield.h"

PrimaryHappyField::PrimaryHappyField(QObject* parent):
    VarHappyField(parent),
    m_startValue(1000)
{
    m_type = SqlColumnTypes::Integer;
    m_options = SqlColumnOptions::AutoIncrement | SqlColumnOptions::Primary;

    m_sqlColumnPreparator->deleteLater();
    SqlPrimaryColumnPreparator* sqlPreparator = new SqlPrimaryColumnPreparator(this);
    m_sqlColumnPreparator = sqlPreparator;

    m_sqlColumnPreparator->setName(m_name);
    m_sqlColumnPreparator->setType(m_type);
    m_sqlColumnPreparator->setOptions(m_options);
    m_sqlColumnPreparator->setPrecision(m_precision);
    sqlPreparator->setStartValue(m_startValue);
    connect(this, &PrimaryHappyField::startValueChanged, sqlPreparator, &SqlPrimaryColumnPreparator::setStartValue);
    connect(this, &VarHappyField::nameChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setName);
    connect(this, &VarHappyField::typeChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setType);
    connect(this, &VarHappyField::precisionChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setPrecision);
    connect(this, &VarHappyField::optionsChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setOptions);
}

QVariant PrimaryHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    if(!values.contains(m_name))
    {
        if(ok)
            *ok = false;
        return QVariant();
    }

    QVariant value;

    const int luid = formatWrite(values.value(m_name), ok).toInt();

    if(luid!=0)
    {
        value = luid;

        if(ok)
            *ok = true;
    }
    else
    {
        if(ok)
            *ok = false;
    }

    return value;
}
