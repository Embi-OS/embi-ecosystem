#include "displaysettings.h"
#include "solid_log.h"

#if defined(Q_OS_BOOT2QT)
#include "displaycomponentb2qt.h"
#endif
#include "abstractdisplaycomponent.h"

static AbstractDisplayComponent* getComponent()
{
    static AbstractDisplayComponent* instance = nullptr;

    if(!instance)
    {
#if defined(Q_OS_BOOT2QT)
        instance = new DisplayComponentB2qt();
#endif
    }

    return instance;
}

DisplaySettings::DisplaySettings(QObject *parent) :
    QObject(parent)
{
    if(!getComponent()) {
        SOLIDLOG_WARNING()<<"Could not find a display component matching this platform";
    }
}

bool DisplaySettings::canSetBrightness()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetBrightness();
}

bool DisplaySettings::canSetHighDpi()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetHighDpi();
}

bool DisplaySettings::canSetScaleFactor()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetScaleFactor();
}

bool DisplaySettings::canSetRotation()
{
    if(!getComponent())
        return false;
    return getComponent()->canSetRotation();
}

bool DisplaySettings::canHideCursor()
{
    if(!getComponent())
        return false;
    return getComponent()->canHideCursor();
}

bool DisplaySettings::canHideKeyboard()
{
    if(!getComponent())
        return false;
    return getComponent()->canHideKeyboard();
}

int DisplaySettings::getBrightness() const
{
    if(!canSetScaleFactor())
    {
        SOLIDLOG_DEBUG()<<"Cannot get brightness, fallback to default";
        return 0;
    }

    return getComponent()->getBrightness();
}

void DisplaySettings::setBrightness(int brightness)
{
    if(!canSetBrightness())
    {
        SOLIDLOG_WARNING()<<"Cannot set brightness";
        return;
    }

    if(getComponent()->setBrightness(brightness))
        emit this->brightnessChanged();
}

bool DisplaySettings::getHighDpi() const
{
    if(!canSetScaleFactor())
    {
        SOLIDLOG_DEBUG()<<"Cannot get high dpi, fallback to default";
        return false;
    }

    return getComponent()->getHighDpi();
}

void DisplaySettings::setHighDpi(bool highDpi)
{
    if(!canSetHighDpi())
    {
        SOLIDLOG_WARNING()<<"Cannot set high dpi";
        return;
    }

    if(getComponent()->setHighDpi(highDpi))
        emit this->highDpiChanged();
}

float DisplaySettings::getScaleFactor() const
{
    if(!canSetScaleFactor())
    {
        SOLIDLOG_DEBUG()<<"Cannot get factor, fallback to default";
        return 100.0;
    }

    return getComponent()->getScaleFactor();
}

void DisplaySettings::setScaleFactor(float scaleFactor)
{
    if(!canSetScaleFactor())
    {
        SOLIDLOG_WARNING()<<"Cannot set scale factor";
        return;
    }

    if(getComponent()->setScaleFactor(scaleFactor))
        emit this->scaleFactorChanged();
}

float DisplaySettings::getRotation() const
{
    if(!canSetRotation())
    {
        SOLIDLOG_DEBUG()<<"Cannot get rotation, fallback to default";
        return 0.0;
    }

    return getComponent()->getRotation();
}

void DisplaySettings::setRotation(float rotation)
{
    if(!canSetRotation())
    {
        SOLIDLOG_WARNING()<<"Cannot set rotation";
        return;
    }

    if(getComponent()->setRotation(rotation))
        emit this->rotationChanged();
}

bool DisplaySettings::getHideCursor() const
{
    if(!canHideCursor())
    {
        SOLIDLOG_DEBUG()<<"Cannot get hideCursor, fallback to default";
        return false;
    }

    return getComponent()->getHideCursor();
}

void DisplaySettings::setHideCursor(bool hideCursor)
{
    if(!canHideCursor())
    {
        SOLIDLOG_WARNING()<<"Cannot set hideCursor";
        return;
    }

    if(getComponent()->setHideCursor(hideCursor))
        emit this->hideCursorChanged();
}

bool DisplaySettings::getHideKeyboard() const
{
    if(!canHideKeyboard())
    {
        SOLIDLOG_DEBUG()<<"Cannot get hideKeyboard, fallback to default";
        return false;
    }

    return getComponent()->getHideKeyboard();
}

void DisplaySettings::setHideKeyboard(bool hideKeyboard)
{
    if(!canHideKeyboard())
    {
        SOLIDLOG_WARNING()<<"Cannot set hideKeyboard";
        return;
    }

    if(getComponent()->setHideKeyboard(hideKeyboard))
        emit this->hideKeyboardChanged();
}


