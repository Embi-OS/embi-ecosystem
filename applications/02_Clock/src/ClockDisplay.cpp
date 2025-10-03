#include "ClockDisplay.h"

#include "Display/displaysettings.h"

ClockDisplay::ClockDisplay(QObject *parent) :
    AbstractManager(parent),
    m_brightnessAnimation(new QPropertyAnimation(this)),
    m_timer(new QTimer(this))
{
    m_brightnessAnimation->setTargetObject(DisplaySettings::Get());
    m_brightnessAnimation->setPropertyName("brightness");

    m_timer->setSingleShot(true);

    connect(m_timer, &QTimer::timeout, this, &ClockDisplay::onTimerTimeout);
    connect(this, &ClockDisplay::stateChanged, this, &ClockDisplay::onStateChanged);
}

bool ClockDisplay::init()
{
    unlock();

    QSettingsMapper* persistantData = new QSettingsMapper(this);
    persistantData->setSelectPolicy(QVariantMapperPolicies::Manual);
    persistantData->setSubmitPolicy(QVariantMapperPolicies::Delayed);
    persistantData->setSettingsCategory(managerName());
    persistantData->select();
    persistantData->waitForSelect();

    persistantData->mapProperty(this,"onBrightness");
    persistantData->mapProperty(this,"lockedBrightness");
    persistantData->mapProperty(this,"lockedTimeout");
    persistantData->mapProperty(this,"wakeBrightness");
    persistantData->mapProperty(this,"wakeTimeout");
    persistantData->mapProperty(this,"alwaysOnEnabled");

    return true;
}

void ClockDisplay::gotoSleep()
{
    setState(ClockDisplayStates::Locking);
}
void ClockDisplay::gotoAlwaysOn()
{
    setState(ClockDisplayStates::AlwaysOn);
}
void ClockDisplay::gotoLocked()
{
    setState(ClockDisplayStates::Locked);
}
void ClockDisplay::gotoOff()
{
    setState(ClockDisplayStates::Off);
}
void ClockDisplay::gotoWaking()
{
    if(m_state!=ClockDisplayStates::On)
        setState(ClockDisplayStates::Waking);
}
void ClockDisplay::unlock()
{
    setState(ClockDisplayStates::On);
    m_timer->start(60 * 1000);
}

void ClockDisplay::onStateChanged()
{
    m_brightnessAnimation->stop();
    m_timer->stop();
    disconnect(this, nullptr, DisplaySettings::Get(), nullptr);

    switch (m_state) {
    case ClockDisplayStates::Off:
        m_brightnessAnimation->setDuration(500);
        m_brightnessAnimation->setEndValue(0);
        m_brightnessAnimation->setEasingCurve(QEasingCurve::InOutQuad);
        break;
    case ClockDisplayStates::AlwaysOn:
        m_brightnessAnimation->setDuration(500);
        m_brightnessAnimation->setEndValue(m_lockedBrightness);
        m_brightnessAnimation->setEasingCurve(QEasingCurve::InOutQuad);
        break;
    case ClockDisplayStates::Locking:
    case ClockDisplayStates::Locked:
        m_brightnessAnimation->setDuration(500);
        m_brightnessAnimation->setEndValue(m_lockedBrightness);
        m_brightnessAnimation->setEasingCurve(QEasingCurve::InOutQuad);
        m_timer->start(m_lockedTimeout * 1000);
        break;
    case ClockDisplayStates::Waking:
        m_brightnessAnimation->setDuration(m_wakeTimeout * 60 * 1000);
        m_brightnessAnimation->setEndValue(m_wakeBrightness);
        m_brightnessAnimation->setEasingCurve(QEasingCurve::Linear);
        break;
    default:
        m_brightnessAnimation->setDuration(500);
        m_brightnessAnimation->setEndValue(m_onBrightness);
        m_brightnessAnimation->setEasingCurve(QEasingCurve::InOutQuad);
        connect(this, &ClockDisplay::onBrightnessChanged, DisplaySettings::Get(), &DisplaySettings::setBrightness, Qt::UniqueConnection);
        break;
    }

    m_brightnessAnimation->start();
}

void ClockDisplay::onTimerTimeout()
{
    m_timer->stop();

    switch (m_state) {
    case ClockDisplayStates::Locking:
    case ClockDisplayStates::Locked:
        gotoOff();
        break;
    case ClockDisplayStates::On:
        if(m_alwaysOnEnabled)
            gotoAlwaysOn();
        else
            m_timer->start(60 * 1000);
        break;
    default:
        break;
    }
}
