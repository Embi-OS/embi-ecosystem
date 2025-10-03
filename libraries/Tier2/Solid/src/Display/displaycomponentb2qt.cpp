#include "displaycomponentb2qt.h"
#include "solid_log.h"

#include <QDirIterator>

DisplayComponentB2qt::DisplayComponentB2qt() :
    m_brightness(255),
    m_lightDevicesInitialized(false),
    m_writeConfigQueued(false),
    m_highDpi(false),
    m_scaleFactor(100.0),
    m_rotation(0.0),
    m_hideCursor(true),
    m_hideKeyboard(false)
{
    readConfig();
    initLightDevices();
    initDisplayDevices();
}

int DisplayComponentB2qt::getCapabilities()
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

int DisplayComponentB2qt::getBrightness() const
{
    return m_brightness;
}

bool DisplayComponentB2qt::setBrightness(int brightness)
{
    if(getBrightness()==brightness)
        return false;
    quint8 value = qBound(0, brightness, 255);
    initLightDevices();
    for (int i = 0; i < m_lightDevices.size(); i++) {
        LightDevice &ld = m_lightDevices[i];
        QFile devFile(ld.deviceFile);
        if (!devFile.open(QIODevice::WriteOnly))
            continue;
        // Maps only 0 to 0, since 0 often means "off"; other values are degrees of "on".
        uint newVal = value ? 1 + ((value * ld.maxValue) / 256) : 0;
        devFile.write(QByteArray::number(newVal));
        ld.value = value;
    }
    m_brightness = value;
    return true;
}

bool DisplayComponentB2qt::initLightDevices()
{
    if (m_lightDevicesInitialized)
        return true;
    QDirIterator it(QStringLiteral("/sys/class/backlight"));
    while (it.hasNext()) {
        LightDevice ld;
        ld.deviceFile = it.next() + QStringLiteral("/brightness");
        QFile maxFile(it.filePath() + QStringLiteral("/max_brightness"));
        if (!maxFile.open(QIODevice::ReadOnly))
            continue;
        bool ok = false;
        ld.maxValue = maxFile.read(10).simplified().toUInt(&ok);
        if (!ok || !ld.maxValue)
            continue;
        QFile valFile(ld.deviceFile);
        if (!valFile.open(QIODevice::ReadOnly))
            continue;
        ok = false;
        uint val = valFile.read(10).simplified().toUInt(&ok);
        if (!ok)
            continue;
        // map max->max as that is a common case, otherwise choose a reasonable value
        ld.value = (val == ld.maxValue) ? 255 : (val * 256)/(ld.maxValue+1);
        ld.name = it.fileName();
        m_lightDevices.append(ld);
    }
    if (m_lightDevices.isEmpty())
        return false;

    m_brightness = m_lightDevices.at(0).value;
    m_lightDevicesInitialized = true;
    return true;
}

bool DisplayComponentB2qt::initDisplayDevices()
{
    if (m_displayDevicesInitialized)
        return true;

    readConfig();

    m_displayDevicesInitialized = true;
    return true;
}

bool DisplayComponentB2qt::getHighDpi() const
{
    return m_highDpi;
}

bool DisplayComponentB2qt::setHighDpi(bool highDpi)
{
    if(m_highDpi==highDpi)
        return false;
    m_highDpi = highDpi;

    queueWriteConfig();
    return true;
}

float DisplayComponentB2qt::getScaleFactor() const
{
    return m_scaleFactor;
}

bool DisplayComponentB2qt::setScaleFactor(float scaleFactor)
{
    if(m_scaleFactor==scaleFactor)
        return false;
    m_scaleFactor = scaleFactor;

    queueWriteConfig();
    return true;
}

float DisplayComponentB2qt::getRotation() const
{
    return m_rotation;
}

bool DisplayComponentB2qt::setRotation(float rotation)
{
    if(m_rotation==rotation)
        return false;
    m_rotation = rotation;

    queueWriteConfig();
    return true;
}

bool DisplayComponentB2qt::getHideCursor() const
{
    return m_hideCursor;
}

bool DisplayComponentB2qt::setHideCursor(bool hideCursor)
{
    if(m_hideCursor==hideCursor)
        return false;
    m_hideCursor = hideCursor;

    queueWriteConfig();
    return true;
}

bool DisplayComponentB2qt::getHideKeyboard() const
{
    return m_hideKeyboard;
}

bool DisplayComponentB2qt::setHideKeyboard(bool hideKeyboard)
{
    if(m_hideKeyboard==hideKeyboard)
        return false;
    m_hideKeyboard = hideKeyboard;

    queueWriteConfig();
    return true;
}

#define SETTINGS_FILENAME "/var/lib/b2qt/appcontroller.conf.d/display.conf"
bool DisplayComponentB2qt::readConfig()
{
    QFile f(SETTINGS_FILENAME);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    while (!f.atEnd())
    {
        QByteArray line = f.readLine().trimmed();
        if(line.startsWith(QByteArrayLiteral("env=")))
        {
            QByteArrayList values = line.split('=');
            if(values.count() == 3)
            {
                bool ok;
                if(values[1] == QByteArrayLiteral("QT_ENABLE_HIGHDPI_SCALING"))
                {
                    int val = values[2].toInt(&ok);
                    if(ok)
                        m_highDpi = val;
                }
                else if(values[1] == QByteArrayLiteral("QT_SCALE_FACTOR"))
                {
                    double val = values[2].toDouble(&ok);
                    if(ok)
                        m_scaleFactor = val*100.0;
                }
                else if(values[1] == QByteArrayLiteral("QT_QUICK_ROTATION"))
                {
                    double val = values[2].toDouble(&ok);
                    if(ok)
                        m_rotation = val;
                }
                else if(values[1] == QByteArrayLiteral("QT_HIDE_CURSOR"))
                {
                    bool val = values[2].toUInt(&ok);
                    if(ok)
                        m_hideCursor = val;
                }
                else if(values[1] == QByteArrayLiteral("QT_HIDE_KEYBOARD"))
                {
                    bool val = values[2].toUInt(&ok);
                    if(ok)
                        m_hideKeyboard = val;
                }
            }
        }
    }

    f.close();

    return true;
}

void DisplayComponentB2qt::queueWriteConfig()
{
    if(m_writeConfigQueued)
        return;

    m_writeConfigQueued=true;
    QTimer::singleShot(0, nullptr, [this](){writeConfig();});
}

void DisplayComponentB2qt::writeConfig()
{
    m_writeConfigQueued=false;

    QDir(QStringLiteral("/var/lib")).mkpath(QStringLiteral("b2qt/appcontroller.conf.d"));

    QFile f(SETTINGS_FILENAME);
    QByteArrayList lines;

    if (f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while(!f.atEnd())
        {
            QByteArray line = f.readLine().trimmed();
            if(!line.startsWith(QByteArrayLiteral("env=QT_ENABLE_HIGHDPI_SCALING=")) &&
               !line.startsWith(QByteArrayLiteral("env=QT_SCALE_FACTOR=")) &&
               !line.startsWith(QByteArrayLiteral("env=QT_QUICK_ROTATION=")) &&
               !line.startsWith(QByteArrayLiteral("env=QT_HIDE_CURSOR=")) &&
               !line.startsWith(QByteArrayLiteral("env=QT_HIDE_KEYBOARD=")))
                lines.append(line);
        }
        f.close();
    }

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        SOLIDLOG_WARNING()<<"Cannot open file:"<<f.fileName()<<f.errorString();
        return;
    }

    for(const QByteArray &line: std::as_const(lines))
        f.write(line + QByteArrayLiteral("\n"));

    f.write(QByteArrayLiteral("env=QT_ENABLE_HIGHDPI_SCALING=") + QByteArray::number(m_highDpi) + QByteArrayLiteral("\n"));
    f.write(QByteArrayLiteral("env=QT_SCALE_FACTOR=") + QByteArray::number(m_scaleFactor/100.0) + QByteArrayLiteral("\n"));
    f.write(QByteArrayLiteral("env=QT_QUICK_ROTATION=") + QByteArray::number(m_rotation) + QByteArrayLiteral("\n"));
    f.write(QByteArrayLiteral("env=QT_HIDE_CURSOR=") + QByteArray::number(m_hideCursor) + QByteArrayLiteral("\n"));
    f.write(QByteArrayLiteral("env=QT_HIDE_KEYBOARD=") + QByteArray::number(m_hideKeyboard) + QByteArrayLiteral("\n"));

    f.close();
}


