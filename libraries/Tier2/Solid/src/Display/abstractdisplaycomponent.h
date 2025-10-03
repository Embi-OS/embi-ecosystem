#ifndef ABSTRACTDISPLAYCOMPONENT_H
#define ABSTRACTDISPLAYCOMPONENT_H

#include <QObject>

class AbstractDisplayComponent
{
public:
    AbstractDisplayComponent() {};

    enum Capabilities
    {
        Brightness = 0x01,
        HighDpi = 0x02,
        ScaleFactor = 0x04,
        Rotation = 0x08,
        HideCursor = 0x10,
        HideKeyboard = 0x20
    };

    bool hasCapability(int capability) { return getCapabilities() & capability; };
    bool canSetBrightness() { return hasCapability(Capabilities::Brightness); }
    bool canSetHighDpi() { return hasCapability(Capabilities::HighDpi); }
    bool canSetScaleFactor() { return hasCapability(Capabilities::ScaleFactor); }
    bool canSetRotation() { return hasCapability(Capabilities::Rotation); }
    bool canHideCursor() { return hasCapability(Capabilities::HideCursor); }
    bool canHideKeyboard() { return hasCapability(Capabilities::HideKeyboard); }

    virtual int getCapabilities() = 0;

    virtual int getBrightness() const { return 0; };
    virtual bool setBrightness(int brightness) { Q_UNUSED(brightness); return false; };

    virtual bool getHighDpi() const { return false; };
    virtual bool setHighDpi(bool highDpi) { Q_UNUSED(highDpi); return false; };

    virtual float getScaleFactor() const { return 0; };
    virtual bool setScaleFactor(float scaleFactor) { Q_UNUSED(scaleFactor); return false; };

    virtual float getRotation() const { return 0; };
    virtual bool setRotation(float rotation) { Q_UNUSED(rotation); return false; };

    virtual bool getHideCursor() const { return 0; };
    virtual bool setHideCursor(bool hideCursor) { Q_UNUSED(hideCursor); return false; };

    virtual bool getHideKeyboard() const { return 0; };
    virtual bool setHideKeyboard(bool hideKeyboard) { Q_UNUSED(hideKeyboard); return false; };
};

#endif // ABSTRACTDISPLAYCOMPONENT_H
