#ifndef HASS_HELPERTYPES_H
#define HASS_HELPERTYPES_H

#include <QDefs>
#include <QUtils>

class HassHelper: public QObject,
                  public QQmlSingleton<HassHelper>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<HassHelper>;
    explicit HassHelper(QObject* parent=nullptr);

public:
    Q_INVOKABLE static QString entityPattern(const QString& domain);
    Q_INVOKABLE static bool isEntityGroup(const QVariantMap& attributes);

    Q_INVOKABLE static QColor rgbToColor(const QVariant& rgb);
    Q_INVOKABLE static QColor colorTemperatureToColor(int kelvin);
    Q_INVOKABLE static QColor lightColor(const QVariantMap& attributes);
    Q_INVOKABLE static int lightBrightness(const QVariantMap& attributes);
    Q_INVOKABLE static int lightBrightnessLevel(const QVariantMap& attributes);
    Q_INVOKABLE static bool lightSupportsBrightness(const QVariantMap& attributes);
    Q_INVOKABLE static bool lightSupportsColor(const QVariantMap& attributes);

    Q_INVOKABLE static QVariantMap defaultStateMap();
};

#endif // HASS_HELPERTYPES_H
