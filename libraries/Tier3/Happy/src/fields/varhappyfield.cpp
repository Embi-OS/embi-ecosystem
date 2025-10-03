#include "varhappyfield.h"

VarHappyField::VarHappyField(QObject* parent):
    AbstractHappyField(parent),
    m_sqlColumnPreparator(new SqlColumnPreparator(this))
{
    m_sqlColumnPreparator->setName(m_name);
    m_sqlColumnPreparator->setType(m_type);
    m_sqlColumnPreparator->setOptions(m_options);
    m_sqlColumnPreparator->setPrecision(m_precision);
    connect(this, &VarHappyField::nameChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setName);
    connect(this, &VarHappyField::typeChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setType);
    connect(this, &VarHappyField::precisionChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setPrecision);
    connect(this, &VarHappyField::optionsChanged, m_sqlColumnPreparator, &SqlPrimaryColumnPreparator::setOptions);
}

QVariant VarHappyField::formatValue(const QVariant& value, bool* ok) const
{
    return Sql::formatValue(value, m_type, ok);
}

QVariant VarHappyField::formatRead(const QVariant& value, bool* ok) const
{
    if(ok)
        *ok = true;

    switch (m_type) {
    case SqlColumnTypes::Json: {
        const QByteArray json = value.toByteArray();
        QJsonParseError parseError;
        const QVariant data = QUtils::Json::jsonToVariant(json, &parseError);
        if (parseError.error != QJsonParseError::NoError && ok)
            *ok = false;
        return data;
    }
    case SqlColumnTypes::DateTime:
        return value.toDateTime().toLocalTime();
    case SqlColumnTypes::Boolean:
        return value.toBool();
    default:
        break;
    }

    return AbstractHappyField::formatRead(value, ok);
}

QVariant VarHappyField::formatWrite(const QVariant& value, bool* ok) const
{
    if(!value.isNull())
    {
        if(ok)
            *ok = true;
    }

    switch (m_type) {
    case SqlColumnTypes::Json:
        if(!value.isNull()) {
            return QString::fromUtf8(QUtils::Json::variantToJson(value));
        }
    case SqlColumnTypes::Boolean:
        return value.toBool();
    default:
        break;
    }

    return AbstractHappyField::formatWrite(value, ok);
}

QVariant VarHappyField::read(const QSqlRecord& record, bool* ok) const
{
    if(m_nameIndex<0)
    {
        if(ok)
            *ok = false;
        return m_defaultValue;
    }

    return formatRead(record.value(m_nameIndex), ok);
}

QVariant VarHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    if(!values.contains(m_name))
    {
        if(ok)
            *ok = false;
        return QVariant();
    }

    return formatWrite(values.value(m_name), ok);
}
