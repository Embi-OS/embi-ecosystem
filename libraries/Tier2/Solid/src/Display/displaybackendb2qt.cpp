#include "displaybackendb2qt.h"
#include "solid_log.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTimer>

#define SETTINGS_FILENAME "/var/lib/b2qt/appcontroller.conf.d/display.conf"

DisplayBackendB2Qt::DisplayBackendB2Qt(QObject* parent):
    DisplayBackend(parent)
{
    readConfig();
    initLightDevices();
    initDisplayDevices();
}

int DisplayBackendB2Qt::getCapabilities()
{
    int capabilities=0;

    if(initLightDevices())
    {
        capabilities += Capabilities::Brightness;
    }

    if(initDisplayDevices())
    {
        capabilities += Capabilities::HighDpi |
                        Capabilities::ScaleFactor |
                        Capabilities::Rotation |
                        Capabilities::HideCursor |
                        Capabilities::HideKeyboard;
    }

    return capabilities;
}

int DisplayBackendB2Qt::getBrightness() const
{
    return m_brightness;
}

bool DisplayBackendB2Qt::setBrightness(int brightnessValue)
{
    if (m_brightness == brightnessValue)
        return false;

    const quint8 value = qBound(0, brightnessValue, 255);
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
    return true;
}

bool DisplayBackendB2Qt::getHighDpi() const
{
    return m_highDpi;
}

bool DisplayBackendB2Qt::setHighDpi(bool highDpiValue)
{
    if (m_highDpi == highDpiValue)
        return false;

    m_highDpi = highDpiValue;
    queueWriteConfig();
    return true;
}

float DisplayBackendB2Qt::getScaleFactor() const
{
    return m_scaleFactor;
}

bool DisplayBackendB2Qt::setScaleFactor(float scaleFactorValue)
{
    if (m_scaleFactor == scaleFactorValue)
        return false;

    m_scaleFactor = scaleFactorValue;
    queueWriteConfig();
    return true;
}

float DisplayBackendB2Qt::getRotation() const
{
    return m_rotation;
}

bool DisplayBackendB2Qt::setRotation(float rotationValue)
{
    if (m_rotation == rotationValue)
        return false;

    m_rotation = rotationValue;
    queueWriteConfig();
    return true;
}

bool DisplayBackendB2Qt::getHideCursor() const
{
    return m_hideCursor;
}

bool DisplayBackendB2Qt::setHideCursor(bool hideCursorValue)
{
    if (m_hideCursor == hideCursorValue)
        return false;

    m_hideCursor = hideCursorValue;
    queueWriteConfig();
    return true;
}

bool DisplayBackendB2Qt::getHideKeyboard() const
{
    return m_hideKeyboard;
}

bool DisplayBackendB2Qt::setHideKeyboard(bool hideKeyboardValue)
{
    if (m_hideKeyboard == hideKeyboardValue)
        return false;

    m_hideKeyboard = hideKeyboardValue;
    queueWriteConfig();
    return true;
}

bool DisplayBackendB2Qt::initLightDevices()
{
    if (m_lightDevicesInitialized)
        return true;

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
    m_lightDevicesInitialized = true;
    return true;
}

bool DisplayBackendB2Qt::initDisplayDevices()
{
    if (m_displayDevicesInitialized)
        return true;

    readConfig();
    m_displayDevicesInitialized = true;
    return true;
}

bool DisplayBackendB2Qt::readConfig()
{
    QFile file(QString::fromLatin1(SETTINGS_FILENAME));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!file.atEnd()) {
        const QByteArray line = file.readLine().trimmed();
        if (!line.startsWith(QByteArrayLiteral("env=")))
            continue;

        const QByteArrayList values = line.split('=');
        if (values.count() != 3)
            continue;

        bool ok = false;
        if (values[1] == QByteArrayLiteral("QT_ENABLE_HIGHDPI_SCALING")) {
            const int value = values[2].toInt(&ok);
            if (ok)
                m_highDpi = value;
        } else if (values[1] == QByteArrayLiteral("QT_SCALE_FACTOR")) {
            const double value = values[2].toDouble(&ok);
            if (ok)
                m_scaleFactor = value * 100.0F;
        } else if (values[1] == QByteArrayLiteral("QT_QUICK_ROTATION")) {
            const double value = values[2].toDouble(&ok);
            if (ok)
                m_rotation = value;
        } else if (values[1] == QByteArrayLiteral("QT_HIDE_CURSOR")) {
            const bool value = values[2].toUInt(&ok);
            if (ok)
                m_hideCursor = value;
        } else if (values[1] == QByteArrayLiteral("QT_HIDE_KEYBOARD")) {
            const bool value = values[2].toUInt(&ok);
            if (ok)
                m_hideKeyboard = value;
        }
    }

    return true;
}

void DisplayBackendB2Qt::queueWriteConfig()
{
    if (m_writeConfigQueued)
        return;

    m_writeConfigQueued = true;
    QTimer::singleShot(0, nullptr, [this]() { writeConfig(); });
}

void DisplayBackendB2Qt::writeConfig()
{
    m_writeConfigQueued = false;

    QDir(QStringLiteral("/var/lib")).mkpath(QStringLiteral("b2qt/appcontroller.conf.d"));

    QFile file(QString::fromLatin1(SETTINGS_FILENAME));
    QByteArrayList lines;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            const QByteArray line = file.readLine().trimmed();
            if (!line.startsWith(QByteArrayLiteral("env=QT_ENABLE_HIGHDPI_SCALING=")) &&
                !line.startsWith(QByteArrayLiteral("env=QT_SCALE_FACTOR=")) &&
                !line.startsWith(QByteArrayLiteral("env=QT_QUICK_ROTATION=")) &&
                !line.startsWith(QByteArrayLiteral("env=QT_HIDE_CURSOR=")) &&
                !line.startsWith(QByteArrayLiteral("env=QT_HIDE_KEYBOARD="))) {
                lines.append(line);
            }
        }
        file.close();
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        SOLIDLOG_WARNING() << "Cannot open file:" << file.fileName() << file.errorString();
        return;
    }

    for (const QByteArray &line : std::as_const(lines))
        file.write(line + QByteArrayLiteral("\n"));

    file.write(QByteArrayLiteral("env=QT_ENABLE_HIGHDPI_SCALING=") + QByteArray::number(m_highDpi) + QByteArrayLiteral("\n"));
    file.write(QByteArrayLiteral("env=QT_SCALE_FACTOR=") + QByteArray::number(m_scaleFactor / 100.0F) + QByteArrayLiteral("\n"));
    file.write(QByteArrayLiteral("env=QT_QUICK_ROTATION=") + QByteArray::number(m_rotation) + QByteArrayLiteral("\n"));
    file.write(QByteArrayLiteral("env=QT_HIDE_CURSOR=") + QByteArray::number(m_hideCursor) + QByteArrayLiteral("\n"));
    file.write(QByteArrayLiteral("env=QT_HIDE_KEYBOARD=") + QByteArray::number(m_hideKeyboard) + QByteArrayLiteral("\n"));
}
