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
    Q_PROPERTY(bool  canHideCursor     READ canHideCursor     CONSTANT FINAL)
    Q_PROPERTY(bool  canHideKeyboard   READ canHideKeyboard   CONSTANT FINAL)
    Q_PROPERTY(int   brightness        READ getBrightness     WRITE setBrightness    NOTIFY brightnessChanged FINAL)
    Q_PROPERTY(bool  highDpi           READ getHighDpi        WRITE setHighDpi       NOTIFY highDpiChanged FINAL)
    Q_PROPERTY(int   scaleFactor       READ getScaleFactor    WRITE setScaleFactor   NOTIFY scaleFactorChanged FINAL)
    Q_PROPERTY(float rotation          READ getRotation       WRITE setRotation      NOTIFY rotationChanged FINAL)
    Q_PROPERTY(int   hideCursor        READ getHideCursor     WRITE setHideCursor    NOTIFY hideCursorChanged FINAL)
    Q_PROPERTY(int   hideKeyboard      READ getHideKeyboard   WRITE setHideKeyboard  NOTIFY hideKeyboardChanged FINAL)

    Q_WRITABLE_VAR_PROPERTY(bool, showMouseClick, ShowMouseClick, false)

protected:
    friend QQmlSingleton<DisplaySettings>;
    explicit DisplaySettings(QObject *parent = nullptr);

public:
    static bool canSetBrightness();
    static bool canSetHighDpi();
    static bool canSetScaleFactor();
    static bool canSetRotation();
    static bool canHideCursor();
    static bool canHideKeyboard();

    int getBrightness() const;
    bool getHighDpi() const;
    float getScaleFactor() const;
    float getRotation() const;
    bool getHideCursor() const;
    bool getHideKeyboard() const;

public slots:
    void setBrightness(int brightness);
    void setHighDpi(bool highDpi);
    void setScaleFactor(float scaleFactor);
    void setRotation(float rotation);
    void setHideCursor(bool hideCursor);
    void setHideKeyboard(bool hideKeyboard);

signals:
    void brightnessChanged();
    void highDpiChanged();
    void scaleFactorChanged();
    void rotationChanged();
    void hideCursorChanged();
    void hideKeyboardChanged();
};

#endif // DISPLAYSETTINGS_H
