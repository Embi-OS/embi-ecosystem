#ifndef DISPLAYCOMPONENTB2QT_H
#define DISPLAYCOMPONENTB2QT_H

#include <QString>
#include <QTimer>
#include <QUtils>

class LightDevice
{
public:
    QString name;
    QString deviceFile;
    quint8 value;
    uint maxValue;
};

#include "abstractdisplaycomponent.h"
class DisplayComponentB2qt : public AbstractDisplayComponent
{
public:
    DisplayComponentB2qt();

    int getCapabilities() override;

    int getBrightness() const override;
    bool setBrightness(int brightness) override;

    bool getHighDpi() const override;
    bool setHighDpi(bool highDpi) override;

    float getScaleFactor() const override;
    bool setScaleFactor(float scaleFactor) override;

    float getRotation() const override;
    bool setRotation(float rotation) override;

    bool getHideCursor() const override;
    bool setHideCursor(bool hideCursor) override;

    bool getHideKeyboard() const override;
    bool setHideKeyboard(bool hideKeyboard) override;

private:
    bool initLightDevices();
    bool initDisplayDevices();

    mutable int m_brightness;
    mutable QList<LightDevice> m_lightDevices;
    mutable bool m_lightDevicesInitialized;
    mutable bool m_displayDevicesInitialized;

    bool readConfig();
    void queueWriteConfig();
    void writeConfig();

    bool m_writeConfigQueued;

    bool m_highDpi;
    float m_scaleFactor;
    float m_rotation;
    bool m_hideCursor;
    bool m_hideKeyboard;
};

#endif // DISPLAYCOMPONENTB2QT_H
