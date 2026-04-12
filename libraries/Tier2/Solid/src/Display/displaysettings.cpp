#include "displaysettings.h"
#include "displaybackend.h"
#include "solid_log.h"

#include "displaybackendb2qt.h"

DisplaySettings::DisplaySettings(QObject *parent) :
    QObject(parent),
#if defined(Q_OS_BOOT2QT)
    m_backend(new DisplayBackendB2Qt(this))
#else
    m_backend(nullptr)
#endif
{
    if(!m_backend) {
        SOLIDLOG_WARNING()<<"Could not find a displaysettings backend matching this platform";
    }
}

bool DisplaySettings::canSetBrightness() const
{
    return m_backend && m_backend->canSetBrightness();
}

bool DisplaySettings::canSetHighDpi() const
{
    return m_backend && m_backend->canSetHighDpi();
}

bool DisplaySettings::canSetScaleFactor() const
{
    return m_backend && m_backend->canSetScaleFactor();
}

bool DisplaySettings::canSetRotation() const
{
    return m_backend && m_backend->canSetRotation();
}

bool DisplaySettings::canHideCursor() const
{
    return m_backend && m_backend->canHideCursor();
}

bool DisplaySettings::canHideKeyboard() const
{
    return m_backend && m_backend->canHideKeyboard();
}

int DisplaySettings::getBrightness() const
{
    if(!m_backend)
        return 0;
    return m_backend->getBrightness();
}

void DisplaySettings::setBrightness(int brightness)
{
    if(!canSetBrightness())
    {
        SOLIDLOG_WARNING()<<"Cannot set brightness";
        return;
    }

    if(m_backend->setBrightness(brightness))
        emit this->brightnessChanged();
}

bool DisplaySettings::getHighDpi() const
{
    if(!m_backend)
        return false;
    return m_backend->getHighDpi();
}

void DisplaySettings::setHighDpi(bool highDpi)
{
    if(!canSetHighDpi())
    {
        SOLIDLOG_WARNING()<<"Cannot set high dpi";
        return;
    }

    if(m_backend->setHighDpi(highDpi))
        emit this->highDpiChanged();
}

float DisplaySettings::getScaleFactor() const
{
    if(!m_backend)
        return 0;
    return m_backend->getScaleFactor();
}

void DisplaySettings::setScaleFactor(float scaleFactor)
{
    if(!canSetScaleFactor())
    {
        SOLIDLOG_WARNING()<<"Cannot set scale factor";
        return;
    }

    if(m_backend->setScaleFactor(scaleFactor))
        emit this->scaleFactorChanged();
}

float DisplaySettings::getRotation() const
{
    if(!m_backend)
        return 0;
    return m_backend->getRotation();
}

void DisplaySettings::setRotation(float rotation)
{
    if(!canSetRotation())
    {
        SOLIDLOG_WARNING()<<"Cannot set rotation";
        return;
    }

    if(m_backend->setRotation(rotation))
        emit this->rotationChanged();
}

bool DisplaySettings::getHideCursor() const
{
    if(!m_backend)
        return false;
    return m_backend->getHideCursor();
}

void DisplaySettings::setHideCursor(bool hideCursor)
{
    if(!canHideCursor())
    {
        SOLIDLOG_WARNING()<<"Cannot set hideCursor";
        return;
    }

    if(m_backend->setHideCursor(hideCursor))
        emit this->hideCursorChanged();
}

bool DisplaySettings::getHideKeyboard() const
{
    if(!m_backend)
        return false;
    return m_backend->getHideKeyboard();
}

void DisplaySettings::setHideKeyboard(bool hideKeyboard)
{
    if(!canHideKeyboard())
    {
        SOLIDLOG_WARNING()<<"Cannot set hideKeyboard";
        return;
    }

    if(m_backend->setHideKeyboard(hideKeyboard))
        emit this->hideKeyboardChanged();
}
