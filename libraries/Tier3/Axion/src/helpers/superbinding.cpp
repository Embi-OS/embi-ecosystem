#include "superbinding.h"
#include "axion_log.h"

// https://github.com/oKcerG/SuperBindingSandbox

SuperBinding::SuperBinding(QObject* parent) :
    QObject(parent),
    m_policy(SuperBindingPolicies::Direct),
    m_when(true),
    m_invalidateWhen(true),
    m_inhibitTime(0),
    m_value(QVariant()),
    m_pending(false),
    m_completed(false),
    m_inhibitTimer(this)
{
    m_inhibitTimer.setInterval(0);
    m_inhibitTimer.setSingleShot(true);
    connect(&m_inhibitTimer, &QTimer::timeout, this, &SuperBinding::invalidate, Qt::UniqueConnection);
    connect(this, &SuperBinding::inhibitTimeChanged, this, &SuperBinding::onInhibitTimeChanged);

    connect(this, &SuperBinding::invalidateWhenChanged, this, &SuperBinding::queueInvalidate);
    connect(this, &SuperBinding::valueChanged, this, &SuperBinding::queueInvalidate);
}

void SuperBinding::classBegin()
{

}

void SuperBinding::componentComplete()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    if(m_policy<SuperBindingPolicies::Manual && m_when && m_invalidateWhen)
        invalidate();
}

void SuperBinding::setTarget(const QQmlProperty& target)
{
    m_property = target;
    queueInvalidate();
}

void SuperBinding::queueInvalidate()
{
    if(!m_completed || !m_when || !m_invalidateWhen)
        return;

    setPending(true);

    if (m_policy==SuperBindingPolicies::Direct) {
        invalidate();
    }
    else if (m_policy==SuperBindingPolicies::Delayed) {
        if(!m_invalidateQueued) {
            m_invalidateQueued = true;
            m_inhibitTimer.start();
        }
    }
}

void SuperBinding::invalidate()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    m_invalidateQueued = false;
    m_inhibitTimer.stop();

    if(!m_when || !m_invalidateWhen)
        return;

    if (m_policy==SuperBindingPolicies::Disabled) {
        AXIONLOG_WARNING()<<this<<"policy is disabled";
        return;
    }

    doInvalidate();

    emit this->invalidateDone();
    setPending(false);
}

void SuperBinding::onInhibitTimeChanged(int inhibitTime)
{
    int wasActive = m_inhibitTimer.isActive();
    m_inhibitTimer.stop();
    m_inhibitTimer.setInterval(inhibitTime);
    if(wasActive)
        m_inhibitTimer.start();
}

void SuperBinding::doInvalidate()
{
    if(m_property.isValid() && m_property.isWritable())
    {
        if(!m_property.write(m_value))
        {
            AXIONLOG_WARNING()<<">>> SuperBinding: Can't set"<<m_value;
        }
    }

    setPending(false);
}
