#include "abstractvalidator.h"

AbstractValidator::AbstractValidator(QObject* parent) :
    QValidator(parent)
{
    setLocale(QLocale("C"));
    connect(this, &AbstractValidator::changed, this, &AbstractValidator::updateSummary);

    connect(this, &AbstractValidator::enabledChanged, this, &AbstractValidator::queueInvalidate);
}

void AbstractValidator::classBegin()
{

}

void AbstractValidator::componentComplete()
{
    invalidate();
}

bool AbstractValidator::isReady() const
{
    return m_enabled && m_completed;
}

void AbstractValidator::queueInvalidate()
{
    if(!m_completed)
        return;

    if (m_delayed) {
        if (!m_invalidateQueued) {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, &AbstractValidator::invalidate, Qt::QueuedConnection);
        }
    } else {
        invalidate();
    }
}

void AbstractValidator::invalidate()
{
    m_completed = true;
    m_invalidateQueued = false;

    emit this->changed();
}
