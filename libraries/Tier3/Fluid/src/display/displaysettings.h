#ifndef DISPLAYSETTINGS_H
#define DISPLAYSETTINGS_H

#include <QDefs>
#include "qsingleton.h"

Q_ENUM_CLASS(DisplayScaleFactors, DisplayScaleFactor,
             Pct75 = 75,
             Default = 100,
             Pct125 = 125,
             Pct150 = 150,
             Pct175 = 175,
             Pct200 = 200)

Q_ENUM_CLASS(DisplayOrientations, DisplayOrientation,
             Default = 0,
             Deg90 = 90,
             Deg180 = 180,
             Deg270 = 270,)

class DisplaySettings : public QObject,
                        public QQmlSingleton<DisplaySettings>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool  canSetBrightness  READ canSetBrightness  CONSTANT FINAL)
    Q_PROPERTY(bool  canSetHighDpi     READ canSetHighDpi     CONSTANT FINAL)
    Q_PROPERTY(bool  canSetScaleFactor READ canSetScaleFactor CONSTANT FINAL)
    Q_PROPERTY(bool  canSetRotation    READ canSetRotation    CONSTANT FINAL)
    Q_PROPERTY(bool  canSetFullscreen  READ canSetFullscreen  CONSTANT FINAL)
    Q_PROPERTY(bool  canHideCursor     READ canHideCursor     CONSTANT FINAL)
    Q_PROPERTY(bool  canHideKeyboard   READ canHideKeyboard   CONSTANT FINAL)
    Q_PROPERTY(int   brightness        READ getBrightness     WRITE setBrightness    NOTIFY brightnessChanged FINAL)
    Q_PROPERTY(bool  highDpi           READ getHighDpi        WRITE setHighDpi       NOTIFY highDpiChanged FINAL)
    Q_PROPERTY(int   scaleFactor       READ getScaleFactor    WRITE setScaleFactor   NOTIFY scaleFactorChanged FINAL)
    Q_PROPERTY(float rotation          READ getRotation       WRITE setRotation      NOTIFY rotationChanged FINAL)
    Q_PROPERTY(bool  fullscreen        READ getFullscreen     WRITE setFullscreen    NOTIFY fullscreenChanged FINAL)
    Q_PROPERTY(int   hideCursor        READ getHideCursor     WRITE setHideCursor    NOTIFY hideCursorChanged FINAL)
    Q_PROPERTY(int   hideKeyboard      READ getHideKeyboard   WRITE setHideKeyboard  NOTIFY hideKeyboardChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, showMouseClick, ShowMouseClick, false)

protected:
    friend QQmlSingleton<DisplaySettings>;
    explicit DisplaySettings(QObject *parent = nullptr);

public:
    enum Capabilities
    {
        Brightness = 0x01,
        HighDpi = 0x02,
        ScaleFactor = 0x04,
        Rotation = 0x08,
        Fullscreen = 0x10,
        HideCursor = 0x20,
        HideKeyboard = 0x40
    };

    int getCapabilities() const;

    bool hasCapability(int capability) const { return getCapabilities() & capability; };
    bool canSetBrightness() const { return hasCapability(Capabilities::Brightness); }
    bool canSetHighDpi() const { return hasCapability(Capabilities::HighDpi); }
    bool canSetScaleFactor() const { return hasCapability(Capabilities::ScaleFactor); }
    bool canSetRotation() const { return hasCapability(Capabilities::Rotation); }
    bool canSetFullscreen() const { return hasCapability(Capabilities::Fullscreen); }
    bool canHideCursor() const { return hasCapability(Capabilities::HideCursor); }
    bool canHideKeyboard() const { return hasCapability(Capabilities::HideKeyboard); }

    int getBrightness() const;
    bool getHighDpi() const;
    float getScaleFactor() const;
    float getRotation() const;
    bool getFullscreen() const;
    bool getHideCursor() const;
    bool getHideKeyboard() const;

public slots:
    bool setBrightness(int brightness);
    bool setHighDpi(bool highDpi);
    bool setScaleFactor(float scaleFactor);
    bool setRotation(float rotation);
    bool setFullscreen(bool fullscreen);
    bool setHideCursor(bool hideCursor);
    bool setHideKeyboard(bool hideKeyboard);

signals:
    void brightnessChanged();
    void highDpiChanged();
    void scaleFactorChanged();
    void rotationChanged();
    void fullscreenChanged();
    void hideCursorChanged();
    void hideKeyboardChanged();

private:
    struct LightDevice
    {
        QString name;
        QString deviceFile;
        quint8 value = 0;
        uint maxValue = 0;
    };

    bool initLightDevices() const;
    bool readLightDevices() const;
    bool initDisplayDevices() const;
    bool readConfig() const;
    void queueWriteConfig();
    void writeConfig();

    mutable int m_brightness = 255;
    mutable QList<LightDevice> m_lightDevices;
    mutable bool m_lightDevicesInitialized = false;
    mutable bool m_displayDevicesInitialized = false;
    mutable bool m_writeConfigQueued = false;
    mutable bool m_highDpi = false;
    mutable float m_scaleFactor = 100.0F;
    mutable float m_rotation = 0.0F;
    mutable bool m_fullscreen = false;
    mutable bool m_hideCursor = true;
    mutable bool m_hideKeyboard = false;
};

#endif // DISPLAYSETTINGS_H
