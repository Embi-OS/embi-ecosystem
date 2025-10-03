#include "deviceinfo.h"
#include "axion_log.h"

#include <QInputDevice>
#include <QGuiApplication>
#include <QCryptographicHash>
#include <QProcess>
#include <QtGlobal>

DeviceInfo::DeviceInfo(QObject *parent) :
    QObject(parent)
{

}

void DeviceInfo::dumpInfos()
{
    QString debugStr;
    debugStr.append(QString("\n\n"));
    debugStr.append(qPrintable(QString("\t Running on: %1 [%2] arch %3\n").arg(platformName(),platformKernelVersion(), platformCpuArchitecture())));
    debugStr.append(qPrintable(QString("\t Qt Version: %1 [%2]\n").arg(qtVersion(), buildAbi())));
    debugStr.append(qPrintable(QString("\t Qt Version build: %1\n").arg(qtVersionBuild())));
    debugStr.append(qPrintable(QString("\t Build date: %1 at %2\n").arg(buildDate(), buildTime())));
    AXIONLOG_INFO().noquote()<<debugStr;
}

QString DeviceInfo::about()
{
    QString about;
    about += QString("Running on: %1 [%2] arch %3\n").arg(platformName(),platformKernelVersion(), platformCpuArchitecture());
    about += QString("Qt Version: %1 [%2]\n").arg(qtVersion(), buildAbi());
    about += QString("Qt Version build: %1\n").arg(qtVersionBuild());
    about += QString("Version: %1\n").arg(QVersionNumber(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_PATCH).toString());
    about += QString("Build date: %1 at %2").arg(buildDate(), buildTime());
    return about;
}

bool DeviceInfo::isMobile()
{
    return isTouchDevice() && !isTablet();
}

bool DeviceInfo::isTablet()
{
    return isTouchDevice() && screenPhysicalSize() >= 6;
}

bool DeviceInfo::isLargeTablet()
{
    return isTouchDevice() && screenPhysicalSize() >= 9;
}

bool DeviceInfo::isTouchDevice()
{
    return isAndroid() || isIOS() || isWindowsPhone() || isUbuntuTouch();
}

bool DeviceInfo::isDesktop()
{
    return !isTouchDevice();
}

bool DeviceInfo::isMacX()
{
#ifdef Q_OS_MAC
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isWindows()
{
#ifdef Q_OS_WIN
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isLinux()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_OPENBSD)
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isWasm()
{
#if defined(Q_OS_WASM)
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isAndroid()
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isIOS()
{
#ifdef Q_OS_IOS
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isBoot2Qt()
{
#ifdef Q_OS_BOOT2QT
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isRaspberryPi()
{
#ifdef Q_DEVICE_RASPBERRY_PI
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isApalisIMX8()
{
#ifdef Q_DEVICE_APALIS_IMX8
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isUbuntuTouch()
{
#ifdef Q_OS_UBUNTUTOUCH
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isWindowsPhone()
{
#ifdef Q_OS_WINPHONE
    return true;
#else
    return false;
#endif
}

bool DeviceInfo::isWindows8()
{
#ifdef Q_OS_WIN
    return QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS8 ||
           QSysInfo::windowsVersion() == QSysInfo::WV_WINDOWS8_1;
#else
    return false;
#endif
}

bool DeviceInfo::hasTouchScreen()
{
    const auto devices = QInputDevice::devices();
    for (const QInputDevice *dev : devices)
        if (dev->type() == QInputDevice::DeviceType::TouchScreen)
            return true;
    return false;
}

bool DeviceInfo::hasKeyboard()
{
    const auto devices = QInputDevice::devices();
    for (const QInputDevice *dev : devices)
        if (dev->type() == QInputDevice::DeviceType::Keyboard)
            return true;
    return false;
}

bool DeviceInfo::hasMouse()
{
    const auto devices = QInputDevice::devices();
    for (const QInputDevice *dev : devices)
        if (dev->type() == QInputDevice::DeviceType::Mouse || dev->type() == QInputDevice::DeviceType::TouchPad)
            return true;
    return false;
}

QScreen *DeviceInfo::screen()
{
    const QList<QScreen*>& screens = QGuiApplication::screens();
    if(screens.isEmpty())
        return 0;

    return screens.first();
}

float DeviceInfo::screenAvailableWidth()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return screen()->availableVirtualSize().width();
}

float DeviceInfo::screenAvailableHeight()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return screen()->availableVirtualSize().height();
}

float DeviceInfo::screenPhysicalSize()
{
    float w = screenPhysicalWidth();
    float h = screenPhysicalHeight();

    return qSqrt( h*h + w*w );
}

float DeviceInfo::screenPhysicalWidth()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return (float)screenSize().width()/screenDpiX();
}

float DeviceInfo::screenPhysicalHeight()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return (float)screenSize().height()/screenDpiY();
}

float DeviceInfo::screenDpiX()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return screen()->physicalDotsPerInchX();
}

float DeviceInfo::screenDpiY()
{
    if( QGuiApplication::screens().isEmpty() )
        return 0;

    return screen()->physicalDotsPerInchY();
}

QSize DeviceInfo::screenSize()
{
    if( QGuiApplication::screens().isEmpty() )
        return QSize();

    return screen()->size();
}

QString DeviceInfo::deviceName()
{
    return QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture();
}

QString DeviceInfo::deviceId()
{
#if defined(Q_OS_MACX)
    io_registry_entry_t ioRegistryRoot = IORegistryEntryFromPath(kIOMasterPortDefault, "IOService:/");
    CFStringRef uuidCf = (CFStringRef) IORegistryEntryCreateCFProperty(ioRegistryRoot, CFSTR(kIOPlatformUUIDKey), kCFAllocatorDefault, 0);
    IOObjectRelease(ioRegistryRoot);
    char buf[128];
    CFStringGetCString(uuidCf, buf, sizeof(buf), kCFStringEncodingMacRoman);
    CFRelease(uuidCf);
    return QString::fromUtf8( QByteArray(buf, sizeof(buf)) );
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN32)
    static QString cg_hostId;
    if(!cg_hostId.isEmpty())
        return cg_hostId;

    QProcess prc;
#ifdef Q_OS_WIN
    prc.setProgram(QStringLiteral("wmic"));
    prc.setArguments({QStringLiteral("csproduct"), QStringLiteral("get"), QStringLiteral("UUID")});
#else
    prc.setProgram(QStringLiteral("hostid"));
#endif
    prc.start();
    prc.waitForStarted();
    prc.waitForReadyRead();
    prc.waitForFinished();

    cg_hostId = QString::fromUtf8(prc.readAll());
    cg_hostId = cg_hostId.remove(QStringLiteral("UUID")).trimmed();
    if(cg_hostId.isEmpty())
        cg_hostId = QStringLiteral("noid");
    return cg_hostId;
#else
    return QStringLiteral("noid");
#endif
}

QString DeviceInfo::deviceShortId()
{
    QString hash = QString::fromUtf8(QCryptographicHash::hash(deviceId().toUtf8(), QCryptographicHash::Md5).toHex());
    return hash.remove(QStringLiteral("-")).left(8);
}

QString DeviceInfo::platformVersion()
{
    return QSysInfo::productVersion();
}

QString DeviceInfo::platformType()
{
    return QSysInfo::productType();
}

QString DeviceInfo::platformName()
{
    return QSysInfo::prettyProductName();
}

QString DeviceInfo::platformKernel()
{
    return QSysInfo::kernelType();
}

QString DeviceInfo::platformKernelVersion()
{
    return QSysInfo::kernelVersion();
}

QString DeviceInfo::platformCpuArchitecture()
{
    return QSysInfo::currentCpuArchitecture();
}

QString DeviceInfo::qtVersion()
{
    return QString(qVersion());
}

QString DeviceInfo::qtVersionBuild()
{
    return QString(QT_VERSION_STR);
}

QString DeviceInfo::buildDate()
{
    return QStringLiteral(__DATE__);
}

QString DeviceInfo::buildTime()
{
    return QStringLiteral(__TIME__);
}

QString DeviceInfo::buildAbi()
{
    return QSysInfo::buildAbi();
}

QString DeviceInfo::buildCpuArchitecture()
{
    return QSysInfo::buildCpuArchitecture();
}
