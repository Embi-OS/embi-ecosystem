#ifndef COLORUTILS_H
#define COLORUTILS_H

#include <QDefs>
#include <QColor>
#include "qsingleton.h"

Q_ENUM_CLASS(ColorBrightnesses, ColorBrightness,
             Dark, /**< The item is dark and requires a light foreground color to achieve readable contrast. */
             Light,) /**< The item is light and requires a dark foreground color to achieve readable contrast. */

class ColorUtils : public QObject,
                   public QQmlSingleton<ColorUtils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<ColorUtils>;
    explicit ColorUtils(QObject *parent = nullptr);

public:
    Q_INVOKABLE static QColor invert(const QColor &color);
    Q_INVOKABLE static QColor complement(const QColor &color);
    Q_INVOKABLE static QColor transparent(const QColor &color, float alpha);
    Q_INVOKABLE static QColor blend(const QColor &color1, const QColor &color2, float ratio=0.5);
    Q_INVOKABLE static float luminance(const QColor &color);
    Q_INVOKABLE static bool isDarkColor(const QColor &color);
    Q_INVOKABLE static QColor lightDark(const QColor &background, const QColor &lightColor, const QColor &darkColor);
    Q_INVOKABLE static float chroma(const QColor &color);
    Q_INVOKABLE static QString name(const QColor &color, int argb=true);

    struct XYZColor {
        float x = 0;
        float y = 0;
        float z = 0;
    };

    struct LabColor {
        float l = 0;
        float a = 0;
        float b = 0;
    };

    // Not for QML, returns the convertion from srgb of a QColor and XYZ colorspace
    static ColorUtils::XYZColor colorToXYZ(const QColor &color);

    // Not for QML, returns the convertion from srgb of a QColor and Lab colorspace
    static ColorUtils::LabColor colorToLab(const QColor &color);
};

#endif // COLORUTILS_H
