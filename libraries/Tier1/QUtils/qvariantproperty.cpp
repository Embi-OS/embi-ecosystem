#include "qvariantproperty.h"

QVariantProperty::QVariantProperty(QObject* object) :
    QObject(object)
{
    m_inhibitTimer.setInterval(0);
    m_inhibitTimer.setSingleShot(true);
    connect(&m_inhibitTimer, &QTimer::timeout, this, &QVariantProperty::invalidate, Qt::UniqueConnection);
}

void QVariantProperty::queueInvalidate()
{
    if (m_inhibitTime<0) {
        invalidate();
    }
    else {
        if(!m_invalidateQueued) {
            m_invalidateQueued = true;
            m_inhibitTimer.start(m_inhibitTime);
        }
    }
}

void QVariantProperty::invalidate()
{
    m_invalidateQueued = false;

    setKeys(m_data.toMap().keys());
    setValues(m_data.toMap().values());
    setIsEmpty(m_keys.isEmpty());

    emit this->dataChanged(m_data);
}

QVariant QVariantProperty::getNestedValue(const QString& property) const
{
    return qVariantGetNestedValue(m_data, property);
}

bool QVariantProperty::clear()
{
    m_data = QVariant();

    queueInvalidate();

    return true;
}

bool QVariantProperty::setValue(const QVariant& value)
{
    m_data = value;

    queueInvalidate();

    return true;
}

bool QVariantProperty::setNestedValue(const QString& property, const QVariant& value)
{
    bool result = qVariantSetNestedValue(m_data, property, value);

    queueInvalidate();

    return result;
}

bool QVariantProperty::incNestedValue(const QString& property)
{
    bool result = qVariantSetNestedValue(m_data, property, qVariantGetNestedValue(m_data, property).toLongLong()+1);

    queueInvalidate();

    return result;
}

bool QVariantProperty::clearNestedValue(const QString& property)
{
    bool result = qVariantRemoveNestedValue(m_data, property);

    queueInvalidate();

    return result;
}
