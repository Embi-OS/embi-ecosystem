#ifndef DISPLAYBACKENDB2QT_H
#define DISPLAYBACKENDB2QT_H

#include "displaybackend.h"

#include <QList>
#include <QString>

class DisplayBackendB2Qt final : public DisplayBackend
{
public:
    explicit DisplayBackendB2Qt(QObject* parent=nullptr);

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
    struct LightDevice
    {
        QString name;
        QString deviceFile;
        quint8 value = 0;
        uint maxValue = 0;
    };

    bool initLightDevices();
    bool initDisplayDevices();
    bool readConfig();
    void queueWriteConfig();
    void writeConfig();

    int m_brightness = 255;
    QList<LightDevice> m_lightDevices;
    bool m_lightDevicesInitialized = false;
    bool m_displayDevicesInitialized = false;
    bool m_writeConfigQueued = false;
    bool m_highDpi = false;
    float m_scaleFactor = 100.0F;
    float m_rotation = 0.0F;
    bool m_hideCursor = true;
    bool m_hideKeyboard = false;
};

#endif // DISPLAYBACKENDB2QT_H
