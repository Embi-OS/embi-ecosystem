#include "displaysettings.h"
#include "fluid_log.h"

#include <QUtils>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QTimer>

namespace {

constexpr auto HighDpiKey = "QT_ENABLE_HIGHDPI_SCALING";
constexpr auto ScaleFactorKey = "QT_SCALE_FACTOR";
constexpr auto RotationKey = "QT_QUICK_ROTATION";
constexpr auto FullscreenKey = "QT_QUICK_FULLSCREEN";
constexpr auto HideCursorKey = "QT_HIDE_CURSOR";
constexpr auto HideKeyboardKey = "QT_HIDE_KEYBOARD";

QString displaySettingsFilePath()
{
#ifdef Q_OS_BOOT2QT
    return QStringLiteral("/var/lib/b2qt/appcontroller.conf.d/display.conf");
#else
    const QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (appConfigLocation.isEmpty())
        return {};

    return QDir(appConfigLocation).filePath(QStringLiteral("environment/display.conf"));
#endif
}

int platformDisplayCapabilities()
{
#ifdef Q_OS_BOOT2QT
    return DisplaySettings::Capabilities::HighDpi |
           DisplaySettings::Capabilities::ScaleFactor |
           DisplaySettings::Capabilities::Rotation |
           DisplaySettings::Capabilities::HideCursor |
           DisplaySettings::Capabilities::HideKeyboard;
#elif defined(Q_OS_LINUX) || defined(Q_OS_WIN) || defined(Q_OS_MACOS)
    return DisplaySettings::Capabilities::HighDpi |
           DisplaySettings::Capabilities::ScaleFactor |
           DisplaySettings::Capabilities::Fullscreen;
#else
    return 0;
#endif
}

QStringList displaySettingsKeys()
{
    QStringList keys;
    const int capabilities = platformDisplayCapabilities();

    if (capabilities & DisplaySettings::Capabilities::HighDpi)
        keys.append(QString::fromLatin1(HighDpiKey));
    if (capabilities & DisplaySettings::Capabilities::ScaleFactor)
        keys.append(QString::fromLatin1(ScaleFactorKey));
    if (capabilities & DisplaySettings::Capabilities::Rotation)
        keys.append(QString::fromLatin1(RotationKey));
    if (capabilities & DisplaySettings::Capabilities::Fullscreen)
        keys.append(QString::fromLatin1(FullscreenKey));
    if (capabilities & DisplaySettings::Capabilities::HideCursor)
        keys.append(QString::fromLatin1(HideCursorKey));
    if (capabilities & DisplaySettings::Capabilities::HideKeyboard)
        keys.append(QString::fromLatin1(HideKeyboardKey));

    return keys;
}

}

DisplaySettings::DisplaySettings(QObject *parent) :
    QObject(parent)
{
    initLightDevices();
    initDisplayDevices();
}

int DisplaySettings::getCapabilities() const
{
    int capabilities=0;

    if(initLightDevices()) {
        capabilities += Capabilities::Brightness;
    }

    if(initDisplayDevices()) {
        capabilities += platformDisplayCapabilities();
    }

    return capabilities;
}

int DisplaySettings::getBrightness() const
{
    if(!canSetBrightness())
        return 0;
    return m_brightness;
}

bool DisplaySettings::setBrightness(int brightness)
{
    if(!canSetBrightness())
    {
        FLUIDLOG_WARNING()<<"Cannot set brightness";
        return false;
    }

    if (m_brightness == brightness)
        return false;

    const quint8 value = qBound(0, brightness, 255);
    initLightDevices();
    for (LightDevice &device : m_lightDevices) {
        QFile devFile(device.deviceFile);
        if (!devFile.open(QIODevice::WriteOnly))
            continue;

        const uint newValue = value ? 1 + ((value * device.maxValue) / 256) : 0;
        devFile.write(QByteArray::number(newValue));
        device.value = value;
    }

    m_brightness = value;
    emit this->brightnessChanged();

    return true;
}

bool DisplaySettings::getHighDpi() const
{
    if(!canSetHighDpi())
        return false;
    return m_highDpi;
}

bool DisplaySettings::setHighDpi(bool highDpi)
{
    if(!canSetHighDpi())
    {
        FLUIDLOG_WARNING()<<"Cannot set high dpi";
        return false;
    }

    if (m_highDpi == highDpi)
        return false;

    m_highDpi = highDpi;
    queueWriteConfig();
    emit this->highDpiChanged();

    return true;
}

float DisplaySettings::getScaleFactor() const
{
    if(!canSetScaleFactor())
        return 0;
    return m_scaleFactor;
}

bool DisplaySettings::setScaleFactor(float scaleFactor)
{
    if(!canSetScaleFactor())
    {
        FLUIDLOG_WARNING()<<"Cannot set scale factor";
        return false;
    }

    if (m_scaleFactor == scaleFactor)
        return false;

    m_scaleFactor = scaleFactor;
    queueWriteConfig();
    emit this->scaleFactorChanged();

    return true;
}

float DisplaySettings::getRotation() const
{
    if(!canSetRotation())
        return 0;
    return m_rotation;
}

bool DisplaySettings::setRotation(float rotation)
{
    if(!canSetRotation())
    {
        FLUIDLOG_WARNING()<<"Cannot set rotation";
        return false;
    }

    if (m_rotation == rotation)
        return false;

    m_rotation = rotation;
    queueWriteConfig();
    emit this->rotationChanged();

    return true;
}

bool DisplaySettings::getFullscreen() const
{
    if(!canSetFullscreen())
        return false;
    return m_fullscreen;
}

bool DisplaySettings::setFullscreen(bool fullscreen)
{
    if(!canSetFullscreen())
    {
        FLUIDLOG_WARNING()<<"Cannot set fullscreen";
        return false;
    }

    if (m_fullscreen == fullscreen)
        return false;

    m_fullscreen = fullscreen;
    queueWriteConfig();
    emit this->fullscreenChanged();

    return true;
}

bool DisplaySettings::getHideCursor() const
{
    if(!canHideCursor())
        return false;
    return m_hideCursor;
}

bool DisplaySettings::setHideCursor(bool hideCursor)
{
    if(!canHideCursor())
    {
        FLUIDLOG_WARNING()<<"Cannot set hideCursor";
        return false;
    }

    if (m_hideCursor == hideCursor)
        return false;

    m_hideCursor = hideCursor;
    queueWriteConfig();
    emit this->hideCursorChanged();

    return true;
}

bool DisplaySettings::getHideKeyboard() const
{
    if(!canHideKeyboard())
        return false;
    return m_hideKeyboard;
}

bool DisplaySettings::setHideKeyboard(bool hideKeyboard)
{
    if(!canHideKeyboard())
    {
        FLUIDLOG_WARNING()<<"Cannot set hideKeyboard";
        return false;
    }

    if (m_hideKeyboard == hideKeyboard)
        return false;

    m_hideKeyboard = hideKeyboard;
    queueWriteConfig();
    emit this->hideKeyboardChanged();

    return true;
}

bool DisplaySettings::initLightDevices() const
{
    if (m_lightDevicesInitialized)
        return !m_lightDevices.isEmpty();

#ifdef Q_OS_BOOT2QT
    m_lightDevicesInitialized = true;
    return readLightDevices();
#else
    return false;
#endif
}

bool DisplaySettings::readLightDevices() const
{
    QDirIterator it(QStringLiteral("/sys/class/backlight"));
    while (it.hasNext()) {
        LightDevice device;
        device.deviceFile = it.next() + QStringLiteral("/brightness");

        QFile maxFile(it.filePath() + QStringLiteral("/max_brightness"));
        if (!maxFile.open(QIODevice::ReadOnly))
            continue;

        bool ok = false;
        device.maxValue = maxFile.read(10).simplified().toUInt(&ok);
        if (!ok || !device.maxValue)
            continue;

        QFile valueFile(device.deviceFile);
        if (!valueFile.open(QIODevice::ReadOnly))
            continue;

        ok = false;
        const uint value = valueFile.read(10).simplified().toUInt(&ok);
        if (!ok)
            continue;

        device.value = (value == device.maxValue) ? 255 : (value * 256) / (device.maxValue + 1);
        device.name = it.fileName();
        m_lightDevices.append(device);
    }

    if (m_lightDevices.isEmpty())
        return false;

    m_brightness = m_lightDevices.at(0).value;

    return true;
}

bool DisplaySettings::initDisplayDevices() const
{
    if (m_displayDevicesInitialized)
        return platformDisplayCapabilities() != 0;

    readConfig();
    m_displayDevicesInitialized = true;

    return platformDisplayCapabilities() != 0;
}

bool DisplaySettings::readConfig() const
{
    bool read = false;
    const QByteArrayMap variables = QUtils::EnvironmentVariables::readFile(displaySettingsFilePath(), &read);
    if (!read)
        return false;

    bool ok = false;
    if (variables.contains(QString::fromLatin1(HighDpiKey))) {
        const int value = variables.value(QString::fromLatin1(HighDpiKey)).toInt(&ok);
        if (ok)
            m_highDpi = value;
    }

    if (variables.contains(QString::fromLatin1(ScaleFactorKey))) {
        const double value = variables.value(QString::fromLatin1(ScaleFactorKey)).toDouble(&ok);
        if (ok)
            m_scaleFactor = value * 100.0F;
    }

    if (variables.contains(QString::fromLatin1(RotationKey))) {
        const double value = variables.value(QString::fromLatin1(RotationKey)).toDouble(&ok);
        if (ok)
            m_rotation = value;
    }

    if (variables.contains(QString::fromLatin1(FullscreenKey))) {
        const bool value = variables.value(QString::fromLatin1(FullscreenKey)).toUInt(&ok);
        if (ok)
            m_fullscreen = value;
    }

    if (variables.contains(QString::fromLatin1(HideCursorKey))) {
        const bool value = variables.value(QString::fromLatin1(HideCursorKey)).toUInt(&ok);
        if (ok)
            m_hideCursor = value;
    }

    if (variables.contains(QString::fromLatin1(HideKeyboardKey))) {
        const bool value = variables.value(QString::fromLatin1(HideKeyboardKey)).toUInt(&ok);
        if (ok)
            m_hideKeyboard = value;
    }

    return true;
}

void DisplaySettings::queueWriteConfig()
{
    if (m_writeConfigQueued)
        return;

    m_writeConfigQueued = true;
    QTimer::singleShot(0, nullptr, [this]() { writeConfig(); });
}

void DisplaySettings::writeConfig()
{
    m_writeConfigQueued = false;

    const QString filePath = displaySettingsFilePath();
    if (filePath.isEmpty()) {
        FLUIDLOG_WARNING() << "Cannot resolve display settings file path";
        return;
    }

    QByteArrayMap variables;
    if (m_highDpi)
        variables.insert(QString::fromLatin1(HighDpiKey), QByteArray::number(m_highDpi));
    if (m_scaleFactor != 100.0F)
        variables.insert(QString::fromLatin1(ScaleFactorKey), QByteArray::number(m_scaleFactor / 100.0F));
    if (m_rotation != 0.0F)
        variables.insert(QString::fromLatin1(RotationKey), QByteArray::number(m_rotation));
    if (m_fullscreen)
        variables.insert(QString::fromLatin1(FullscreenKey), QByteArray::number(m_fullscreen));
    if (!m_hideCursor)
        variables.insert(QString::fromLatin1(HideCursorKey), QByteArray::number(m_hideCursor));
    if (m_hideKeyboard)
        variables.insert(QString::fromLatin1(HideKeyboardKey), QByteArray::number(m_hideKeyboard));

    if (!QUtils::EnvironmentVariables::writeFile(filePath, variables, displaySettingsKeys())) {
        FLUIDLOG_WARNING() << "Cannot write display settings file:" << filePath;
    }
}

