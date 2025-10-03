#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QDefs>
#include "qsingleton.h"

#include <QScreen>

class DeviceInfo : public QObject,
                   public QQmlSingleton<DeviceInfo>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool isMobile       READ isMobile       CONSTANT FINAL)
    Q_PROPERTY(bool isTablet       READ isTablet       CONSTANT FINAL)
    Q_PROPERTY(bool isLargeTablet  READ isLargeTablet  CONSTANT FINAL)
    Q_PROPERTY(bool isTouchDevice  READ isTouchDevice  CONSTANT FINAL)
    Q_PROPERTY(bool isDesktop      READ isDesktop      CONSTANT FINAL)
    Q_PROPERTY(bool isMacX         READ isMacX         CONSTANT FINAL)
    Q_PROPERTY(bool isWindows      READ isWindows      CONSTANT FINAL)
    Q_PROPERTY(bool isWindows8     READ isWindows8     CONSTANT FINAL)
    Q_PROPERTY(bool isLinux        READ isLinux        CONSTANT FINAL)
    Q_PROPERTY(bool isWasm         READ isWasm         CONSTANT FINAL)
    Q_PROPERTY(bool isAndroid      READ isAndroid      CONSTANT FINAL)
    Q_PROPERTY(bool isIOS          READ isIOS          CONSTANT FINAL)
    Q_PROPERTY(bool isBoot2Qt      READ isBoot2Qt      CONSTANT FINAL)
    Q_PROPERTY(bool isRaspberryPi  READ isRaspberryPi  CONSTANT FINAL)
    Q_PROPERTY(bool isApalisIMX8   READ isApalisIMX8   CONSTANT FINAL)
    Q_PROPERTY(bool isUbuntuTouch  READ isUbuntuTouch  CONSTANT FINAL)
    Q_PROPERTY(bool isWindowsPhone READ isWindowsPhone CONSTANT FINAL)
    Q_PROPERTY(bool hasTouchScreen READ hasTouchScreen CONSTANT FINAL)
    Q_PROPERTY(bool hasKeyboard    READ hasKeyboard    CONSTANT FINAL)
    Q_PROPERTY(bool hasMouse       READ hasMouse       CONSTANT FINAL)

    Q_PROPERTY(QScreen* screen              READ screen                 CONSTANT FINAL)
    Q_PROPERTY(double screenAvailableWidth  READ screenAvailableWidth   CONSTANT FINAL)
    Q_PROPERTY(double screenAvailableHeight READ screenAvailableHeight  CONSTANT FINAL)
    Q_PROPERTY(double screenPhysicalSize    READ screenPhysicalSize     CONSTANT FINAL)
    Q_PROPERTY(double screenPhysicalWidth   READ screenPhysicalWidth    CONSTANT FINAL)
    Q_PROPERTY(double screenPhysicalHeight  READ screenPhysicalHeight   CONSTANT FINAL)
    Q_PROPERTY(double screenDpiX            READ screenDpiX             CONSTANT FINAL)
    Q_PROPERTY(double screenDpiY            READ screenDpiY             CONSTANT FINAL)

    Q_PROPERTY(QString deviceName       READ deviceName     CONSTANT FINAL)
    Q_PROPERTY(QString deviceId         READ deviceId       CONSTANT FINAL)
    Q_PROPERTY(QString deviceShortId    READ deviceShortId  CONSTANT FINAL)

    Q_PROPERTY(QString platformVersion         READ platformVersion         CONSTANT FINAL)
    Q_PROPERTY(QString platformType            READ platformType            CONSTANT FINAL)
    Q_PROPERTY(QString platformName            READ platformName            CONSTANT FINAL)
    Q_PROPERTY(QString platformKernel          READ platformKernel          CONSTANT FINAL)
    Q_PROPERTY(QString platformKernelVersion   READ platformKernelVersion   CONSTANT FINAL)
    Q_PROPERTY(QString platformCpuArchitecture READ platformCpuArchitecture CONSTANT FINAL)

    Q_PROPERTY(QString qtVersion    READ qtVersion      CONSTANT FINAL)

    Q_PROPERTY(QString buildDate READ buildDate CONSTANT FINAL)
    Q_PROPERTY(QString buildTime READ buildTime CONSTANT FINAL)
    Q_PROPERTY(QString buildAbi  READ buildAbi CONSTANT FINAL)

protected:
    friend QQmlSingleton<DeviceInfo>;
    explicit DeviceInfo(QObject *parent = nullptr);

public:
    Q_INVOKABLE static void dumpInfos();
    Q_INVOKABLE static QString about();

    static bool isMobile();
    static bool isTablet();
    static bool isLargeTablet();
    static bool isTouchDevice();
    static bool isDesktop();
    static bool isMacX();
    static bool isWindows();
    static bool isLinux();
    static bool isWasm();
    static bool isAndroid();
    static bool isIOS();
    static bool isBoot2Qt();
    static bool isRaspberryPi();
    static bool isApalisIMX8();
    static bool isUbuntuTouch();
    static bool isWindowsPhone();
    static bool isWindows8();
    static bool hasTouchScreen();
    static bool hasKeyboard();
    static bool hasMouse();

    static QScreen *screen();
    static float screenAvailableWidth();
    static float screenAvailableHeight();
    static float screenPhysicalSize();
    static float screenPhysicalWidth();
    static float screenPhysicalHeight();
    static float screenDpiX();
    static float screenDpiY();
    static QSize screenSize();

    static QString deviceName();
    static QString deviceId();
    static QString deviceShortId();

    static QString platformVersion();
    static QString platformType();
    static QString platformName();
    static QString platformKernel();
    static QString platformKernelVersion();
    static QString platformCpuArchitecture();

    static QString qtVersion();
    static QString qtVersionBuild();

    static QString buildDate();
    static QString buildTime();
    static QString buildAbi();
    static QString buildCpuArchitecture();
};

struct QtScreenForeign {
    Q_GADGET
    QML_FOREIGN(QScreen)
    QML_NAMED_ELEMENT(Screen)
    QML_UNCREATABLE("Abstract")
};

#endif // DEVICEINFO_H
