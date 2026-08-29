#include "hass_helpertypes.h"

#include <cmath>

HassHelper::HassHelper(QObject* parent):
    QObject(parent)
{}

QString HassHelper::entityPattern(const QString& domain)
{
    return QString("^%1\\..+$").arg(domain);
}

bool HassHelper::isEntityGroup(const QVariantMap& attributes)
{
    return attributes.contains("group_entities")
           || attributes.contains("entity_id")
           || attributes.value("group").toBool();
}

QColor HassHelper::rgbToColor(const QVariant& rgb)
{
    if(rgb.isNull() || !rgb.isValid())
        return QColor(0, 0, 0, 0);

    const QVariantList colors = rgb.toList();
    if(colors.size() != 3)
        return QColor(0, 0, 0, 0);

    return QColor(colors.at(0).toInt(), colors.at(1).toInt(), colors.at(2).toInt());
}

QColor HassHelper::colorTemperatureToColor(int kelvin)
{
    const qreal temperature = qBound(1000, kelvin, 40000) / 100.0;
    qreal red = 255.0;
    qreal green = 255.0;
    qreal blue = 255.0;

    if(temperature <= 66.0) {
        green = 99.4708025861 * std::log(temperature) - 161.1195681661;
        if(temperature <= 19.0)
            blue = 0.0;
        else
            blue = 138.5177312231 * std::log(temperature - 10.0) - 305.0447927307;
    }
    else {
        red = 329.698727446 * std::pow(temperature - 60.0, -0.1332047592);
        green = 288.1221695283 * std::pow(temperature - 60.0, -0.0755148492);
    }

    return QColor(qBound(0, qRound(red), 255),
                  qBound(0, qRound(green), 255),
                  qBound(0, qRound(blue), 255));
}

QColor HassHelper::lightColor(const QVariantMap& attributes)
{
    const QString colorMode = attributes.value("color_mode").toString();
    if(colorMode == "color_temp") {
        bool ok = false;
        int kelvin = attributes.value("color_temp_kelvin").toInt(&ok);
        if(!ok || kelvin <= 0) {
            const int mired = attributes.value("color_temp").toInt(&ok);
            if(ok && mired > 0)
                kelvin = qRound(1000000.0 / mired);
        }

        return kelvin > 0 ? colorTemperatureToColor(kelvin) : QColor(0, 0, 0, 0);
    }

    const bool isColorMode = colorMode.isEmpty()
                             || colorMode == "hs"
                             || colorMode == "rgb"
                             || colorMode == "rgbw"
                             || colorMode == "rgbww"
                             || colorMode == "xy";
    if(!isColorMode)
        return QColor(0, 0, 0, 0);

    const QColor rgbColor = rgbToColor(attributes.value("rgb_color"));
    if(rgbColor.alpha() > 0)
        return rgbColor;

    const QVariantList hsColor = attributes.value("hs_color").toList();
    if(hsColor.size() != 2)
        return QColor(0, 0, 0, 0);

    return QColor::fromHsvF(qBound(0.0, hsColor.at(0).toDouble() / 360.0, 1.0),
                            qBound(0.0, hsColor.at(1).toDouble() / 100.0, 1.0),
                            1.0);
}

int HassHelper::lightBrightness(const QVariantMap& attributes)
{
    bool ok = false;
    const int brightness = attributes.value("brightness").toInt(&ok);
    if(!ok)
        return -1;

    return qBound(0, (brightness * 100 + 254) / 255, 100);
}

int HassHelper::lightBrightnessLevel(const QVariantMap& attributes)
{
    const int brightness = lightBrightness(attributes);
    if(brightness <= 0)
        return brightness;

    return qBound(1, (brightness + 9) / 10, 10);
}

bool HassHelper::lightSupportsBrightness(const QVariantMap& attributes)
{
    if(lightBrightness(attributes) >= 0)
        return true;

    const QVariantList colorModes = attributes.value("supported_color_modes").toList();
    for(const QVariant& colorMode: colorModes) {
        const QString mode = colorMode.toString();
        if(mode != "onoff" && mode != "unknown")
            return true;
    }

    return false;
}

bool HassHelper::lightSupportsColor(const QVariantMap& attributes)
{
    const QVariantList colorModes = attributes.value("supported_color_modes").toList();
    for(const QVariant& colorMode: colorModes) {
        const QString mode = colorMode.toString();
        if(mode == "hs" || mode == "rgb" || mode == "rgbw" || mode == "rgbww" || mode == "xy")
            return true;
    }

    return false;
}

QVariantMap HassHelper::defaultStateMap()
{
    QVariantMap map;

    map.insert("attributes", QVariantMap());
    map.insert("entity_id", QString());
    map.insert("last_changed", QDateTime());
    map.insert("state", QString());

    return map;
}
