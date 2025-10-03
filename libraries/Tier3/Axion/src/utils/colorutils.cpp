#include "colorutils.h"
#include <QFontDatabase>
#include <QUtils>

ColorUtils::ColorUtils(QObject *parent) :
    QObject(parent)
{

}

QColor ColorUtils::invert(const QColor &color)
{
    static int r, g, b;
    color.getRgb(&r, &g, &b);
    return QColor(255-r, 255-g, 255-b);
}

QColor ColorUtils::complement(const QColor &color)
{
    static int r, g, b;
    color.getRgb(&r, &g, &b);

    const int maxRGB = qMax(r, qMax(g, b));
    const int minRGB = qMin(r, qMin(g, b));

    return QColor(maxRGB + minRGB - r, maxRGB + minRGB - g, maxRGB + minRGB - b);
}

QColor ColorUtils::transparent(const QColor &color, float alpha)
{
    static float r, g, b;
    color.getRgbF(&r, &g, &b);
    return QColor::fromRgbF(r, g, b, alpha);
}

QColor ColorUtils::blend(const QColor &color1, const QColor &color2, float ratio)
{
    if (ratio <= 0.0)
        return color1;
    if (ratio >= 1.0)
        return color2;


    static int r1, g1, b1;
    color1.getRgb(&r1, &g1, &b1);

    static int r2, g2, b2;
    color2.getRgb(&r2, &g2, &b2);

    return QColor::fromRgb(r1 * (1.0 - ratio) + r2 * ratio,
                           g1 * (1.0 - ratio) + g2 * ratio,
                           b1 * (1.0 - ratio) + b2 * ratio);
}

float ColorUtils::luminance(const QColor &color)
{
    const XYZColor& xyz = colorToXYZ(color);

    // Luminance is equal to Y
    return xyz.y;
}

bool ColorUtils::isDarkColor(const QColor &color)
{
    static float r, g, b, a;
    color.getRgbF(&r, &g, &b);
    a = 1.0 - (0.299 * r + 0.587 * g + 0.114 * b);
    return a >= 0.3;
}

QColor ColorUtils::lightDark(const QColor &background, const QColor &lightColor, const QColor &darkColor)
{
    return isDarkColor(background) ? lightColor : darkColor;
}

float ColorUtils::chroma(const QColor &color)
{
    const LabColor& labColor = colorToLab(color);

    // Chroma is hypotenuse of a and b
    return sqrt(pow(labColor.a, 2) + pow(labColor.b, 2));
}

QString ColorUtils::name(const QColor &color, int argb)
{
    return color.name(argb ? QColor::HexArgb : QColor::HexRgb).toUpper();
}

ColorUtils::XYZColor ColorUtils::colorToXYZ(const QColor &color)
{
    // http://wiki.nuaj.net/index.php/Color_Transforms#RGB_.E2.86.92_XYZ
    float r, g, b;
    color.getRgbF(&r, &g, &b);
    // Apply gamma correction (i.e. conversion to linear-space)
    auto correct = [](float &v) {
        if (v > 0.04045) {
            v = std::pow((v + 0.055) / 1.055, 2.4);
        } else {
            v = v / 12.92;
        }
    };

    correct(r);
    correct(g);
    correct(b);

    // Observer. = 2°, Illuminant = D65
    const float x = r * 0.4124 + g * 0.3576 + b * 0.1805;
    const float y = r * 0.2126 + g * 0.7152 + b * 0.0722;
    const float z = r * 0.0193 + g * 0.1192 + b * 0.9505;

    return XYZColor{x, y, z};
}

ColorUtils::LabColor ColorUtils::colorToLab(const QColor &color)
{
    // First: convert to XYZ
    const auto xyz = colorToXYZ(color);

    // Second: convert from XYZ to L*a*b
    float x = xyz.x / 0.95047; // Observer= 2°, Illuminant= D65
    float y = xyz.y / 1.0;
    float z = xyz.z / 1.08883;

    auto pivot = [](float &v) {
        if (v > 0.008856) {
            v = std::pow(v, 1.0 / 3.0);
        } else {
            v = (7.787 * v) + (16.0 / 116.0);
        }
    };

    pivot(x);
    pivot(y);
    pivot(z);

    LabColor labColor;
    labColor.l = std::max<float>(0.0, (116 * y) - 16);
    labColor.a = 500 * (x - y);
    labColor.b = 200 * (y - z);

    return labColor;
}
