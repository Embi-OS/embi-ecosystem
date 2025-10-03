#include "datetimehappyfield.h"

DateTimeHappyField::DateTimeHappyField(QObject* parent):
    VarHappyField(parent)
{
    m_type = SqlColumnTypes::DateTime;

    m_sqlColumnPreparator->setType(m_type);
}

QVariant DateTimeHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    QDateTime datetime = values.value(m_name).toDateTime();

    if(datetime.isNull())
    {
        if(m_autoNow)
            datetime = QDateTime::currentDateTime();
        else if(creation && m_autoNowAdd)
            datetime = QDateTime::currentDateTime();
    }

    if(ok && !datetime.isNull() && datetime.isValid())
        *ok = true;
    else if(ok)
        *ok = false;

    return datetime.toLocalTime();
}
