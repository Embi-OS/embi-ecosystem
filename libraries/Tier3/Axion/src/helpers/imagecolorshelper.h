#ifndef IMAGECOLORSHELPER_H
#define IMAGECOLORSHELPER_H

#include "utils/colorutils.h"

#include <QDefs>
#include <QColor>
#include <QImage>
#include <QObject>
#include <QPointer>
#include <QQuickItem>
#include <QQuickItemGrabResult>
#include <QQuickWindow>

#ifdef QT_CONCURRENT_LIB
#include <QFutureWatcher>
#endif

struct PaletteSwatch {
    Q_GADGET
    QML_VALUE_TYPE(imageColorsPaletteSwatch)

    Q_PROPERTY(double ratio READ ratio FINAL)
    Q_PROPERTY(QColor color READ color FINAL)
    Q_PROPERTY(QColor contrastColor READ contrastColor FINAL)

public:
    explicit PaletteSwatch();
    explicit PaletteSwatch(double ratio, const QColor &color, const QColor &contrastColor);

    double ratio() const;
    const QColor &color() const;
    const QColor &contrastColor() const;

    bool operator==(const PaletteSwatch &other) const;

private:
    double m_ratio;
    QColor m_color;
    QColor m_contrastColor;
};

struct ImageData {
    struct colorStat {
        QList<QRgb> colors;
        QRgb centroid = 0;
        double ratio = 0;
    };

    struct colorSet {
        QColor average;
        QColor text;
        QColor background;
        QColor highlight;
    };

    QList<QRgb> m_samples;
    QList<colorStat> m_clusters;
    QList<PaletteSwatch> m_palette;
    QVariantList m_colors;

    bool m_darkPalette = true;
    QColor m_dominant = Qt::transparent;
    QColor m_dominantContrast;
    QColor m_dominantInverted;
    QColor m_dominantComplementary;
    QColor m_average;
    QColor m_highlight;

    QColor m_closestToBlack;
    QColor m_closestToWhite;
};

/**
 * Extracts the dominant colors from an element or an image and exports it to a color palette.
 */
class ImageColorsHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QColor, backgroundColor, BackgroundColor, Qt::black)
    Q_WRITABLE_REF_PROPERTY(QColor, textColor, TextColor, Qt::white)
    Q_READONLY_VAR_PROPERTY(bool, isReady, IsReady, false)

    /**
     * The source from which colors should be extracted from.
     *
     * `source` can be one of the following:
     * * Item
     * * QImage
     * * QIcon
     * * Icon name
     *
     * Note that an Item's color palette will only be extracted once unless you
     * call `update()`, regardless of how the item hanges.
     */
    Q_PROPERTY(QVariant source READ source WRITE setSource NOTIFY sourceChanged FINAL)

    /**
     * A list of colors and related information about then.
     *
     * Each list item has the following properties:
     * * `color`: The color of the list item.
     * * `ratio`: How dominant the color is in the source image.
     * * `contrastingColor`: The color from the source image that's closest to the inverse of `color`.
     *
     * The list is sorted by `ratio`; the first element is the most
     * dominant color in the source image and the last element is the
     * least dominant color of the image.
     *
     * \note K-means clustering is used to extract these colors; see https://en.wikipedia.org/wiki/K-means_clustering.
     */
    Q_PROPERTY(QList<PaletteSwatch> palette READ palette NOTIFY paletteChanged FINAL)
    Q_PROPERTY(QVariantList colors READ colors NOTIFY paletteChanged FINAL)

    /**
     * Information whether the palette is towards a light or dark color
     * scheme, possible values are:
     * * ColorUtils.Light
     * * ColorUtils.Dark
     */
    Q_PROPERTY(ColorBrightnesses::Enum paletteBrightness READ paletteBrightness NOTIFY paletteChanged FINAL)

    /**
     * The average color of the source image.
     */
    Q_PROPERTY(QColor average READ average NOTIFY paletteChanged FINAL)

    /**
     * The dominant color of the source image.
     *
     * The dominant color of the image is the color of the largest
     * cluster in the image.
     *
     * \sa https://en.wikipedia.org/wiki/K-means_clustering
     */
    Q_PROPERTY(QColor dominant READ dominant NOTIFY paletteChanged FINAL)

    /**
     * Suggested "contrasting" color to the dominant one. It's the color in the palette nearest to the negative of the dominant
     */
    Q_PROPERTY(QColor dominantContrast READ dominantContrast NOTIFY paletteChanged FINAL)

    /**
     * Suggested "inverted" color to the dominant one. It's the color in the palette nearest to the inverse of the dominant
     */
    Q_PROPERTY(QColor dominantInverted READ dominantInverted NOTIFY paletteChanged FINAL)

    /**
     * Suggested "complementary" color to the dominant one. It's the color in the palette nearest to the complementary of the dominant
     */
    Q_PROPERTY(QColor dominantComplementary READ dominantComplementary NOTIFY paletteChanged FINAL)

    /**
     * An accent color extracted from the source image.
     *
     * The accent color is the color cluster with the highest CIELAB
     * chroma in the source image.
     *
     * \sa https://en.wikipedia.org/wiki/Colorfulness#Chroma
     */
    Q_PROPERTY(QColor highlight READ highlight NOTIFY paletteChanged FINAL)

    /**
     * A color suitable for rendering text and other foreground
     * over the source image.
     *
     * On dark items, this will be the color closest to white in
     * the image if it's light enough, or a bright gray otherwise.
     * On light items, this will be the color closest to black in
     * the image if it's dark enough, or a dark gray otherwise.
     */
    Q_PROPERTY(QColor foreground READ foreground NOTIFY paletteChanged FINAL)

    /**
     * A color suitable for rendering a background behind the
     * source image.
     *
     * On dark items, this will be the color closest to black in the
     * image if it's dark enough, or a dark gray otherwise.
     * On light items, this will be the color closest to white
     * in the image if it's light enough, or a bright gray otherwise.
     */
    Q_PROPERTY(QColor background READ background NOTIFY paletteChanged FINAL)

    Q_PROPERTY(QColor closestToWhite READ closestToWhite NOTIFY paletteChanged FINAL)
    Q_PROPERTY(QColor closestToBlack READ closestToBlack NOTIFY paletteChanged FINAL)

    Q_PROPERTY(QList<PaletteSwatch> fallbackPalette MEMBER m_fallbackPalette NOTIFY fallbackPaletteChanged FINAL)
    Q_PROPERTY(ColorBrightnesses::Enum fallbackPaletteBrightness MEMBER m_fallbackPaletteBrightness NOTIFY fallbackPaletteBrightnessChanged FINAL)
    Q_PROPERTY(QColor fallbackAverage MEMBER m_fallbackAverage NOTIFY fallbackAverageChanged FINAL)
    Q_PROPERTY(QColor fallbackDominant MEMBER m_fallbackDominant NOTIFY fallbackDominantChanged FINAL)
    Q_PROPERTY(QColor fallbackDominantInverted MEMBER m_fallbackDominantInverted NOTIFY fallbackDominantInvertedChanged FINAL)
    Q_PROPERTY(QColor fallbackDominantComplementary MEMBER m_fallbackDominantComplementary NOTIFY fallbackDominantComplementaryChanged FINAL)
    Q_PROPERTY(QColor fallbackDominantContrasting MEMBER m_fallbackDominantContrasting NOTIFY fallbackDominantContrastingChanged FINAL)
    Q_PROPERTY(QColor fallbackHighlight MEMBER m_fallbackHighlight NOTIFY fallbackHighlightChanged FINAL)
    Q_PROPERTY(QColor fallbackForeground MEMBER m_fallbackForeground NOTIFY fallbackForegroundChanged FINAL)
    Q_PROPERTY(QColor fallbackBackground MEMBER m_fallbackBackground NOTIFY fallbackBackgroundChanged FINAL)

public:
    explicit ImageColorsHelper(QObject *parent = nullptr);

    void setSource(const QVariant &source);
    QVariant source() const;

    void setSourceImage(const QImage &image);
    QImage sourceImage() const;

    void setSourceItem(QQuickItem *source);
    QQuickItem *sourceItem() const;

    Q_INVOKABLE void update();

    QList<PaletteSwatch> palette() const;
    QVariantList colors() const;
    ColorBrightnesses::Enum paletteBrightness() const;
    QColor average() const;
    QColor dominant() const;
    QColor dominantContrast() const;
    QColor dominantInverted() const;
    QColor dominantComplementary() const;
    QColor highlight() const;
    QColor foreground() const;
    QColor background() const;
    QColor closestToWhite() const;
    QColor closestToBlack() const;

Q_SIGNALS:
    void sourceChanged();
    void paletteChanged();
    void fallbackPaletteChanged();
    void fallbackPaletteBrightnessChanged();
    void fallbackAverageChanged();
    void fallbackDominantChanged();
    void fallbackDominantInvertedChanged();
    void fallbackDominantComplementaryChanged();
    void fallbackDominantContrastingChanged();
    void fallbackHighlightChanged();
    void fallbackForegroundChanged();
    void fallbackBackgroundChanged();

private:
    static inline void positionColor(QRgb rgb, QList<ImageData::colorStat> &clusters);
    static void positionColorMP(const decltype(ImageData::m_samples) &samples, decltype(ImageData::m_clusters) &clusters, int numCore = 0);
    static ImageData generatePalette(const QImage &sourceImage);

    static double getClusterScore(const ImageData::colorStat &stat);
    void postProcess(ImageData &imageData, const QColor &backgroundColor, const QColor &textColor) const;

    // Arbitrary number that seems to work well
    static const int s_minimumSquareDistance = 32000;
    QPointer<QQuickWindow> m_window;
    QVariant m_source;
    QPointer<QQuickItem> m_sourceItem;
    QSharedPointer<QQuickItemGrabResult> m_grabResult;
    QImage m_sourceImage;

#ifdef QT_CONCURRENT_LIB
    QFutureWatcher<QImage> *m_futureSourceImageData = nullptr;
    QFutureWatcher<ImageData> *m_futureImageData = nullptr;
#endif

    ImageData m_imageData;

    QList<PaletteSwatch> m_fallbackPalette;
    ColorBrightnesses::Enum m_fallbackPaletteBrightness;
    QColor m_fallbackAverage;
    QColor m_fallbackDominant;
    QColor m_fallbackDominantInverted;
    QColor m_fallbackDominantComplementary;
    QColor m_fallbackDominantContrasting;
    QColor m_fallbackHighlight;
    QColor m_fallbackForeground;
    QColor m_fallbackBackground;
};

#endif // IMAGECOLORSHELPER_H
