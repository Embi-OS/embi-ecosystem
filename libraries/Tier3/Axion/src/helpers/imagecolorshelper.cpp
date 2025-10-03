#include "imagecolorshelper.h"
#include "axion_log.h"

#include <QDebug>
#include <QGuiApplication>
#include <QTimer>

#define IMAGE_COLOR_HELPER_THREADED

#ifdef QT_CONCURRENT_LIB
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#else
#undef IMAGE_COLOR_HELPER_THREADED
#endif

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, qrcRegExp, ("qrc:/+"))

PaletteSwatch::PaletteSwatch()
{

}

PaletteSwatch::PaletteSwatch(double ratio, const QColor &color, const QColor &contrastColor):
    m_ratio(ratio),
    m_color(color),
    m_contrastColor(contrastColor)
{

}

double PaletteSwatch::ratio() const
{
    return m_ratio;
}

const QColor &PaletteSwatch::color() const
{
    return m_color;
}

const QColor &PaletteSwatch::contrastColor() const
{
    return m_contrastColor;
}

bool PaletteSwatch::operator==(const PaletteSwatch &other) const
{
    return m_ratio == other.m_ratio && m_color == other.m_color && m_contrastColor == other.m_contrastColor;
}

ImageColorsHelper::ImageColorsHelper(QObject *parent):
    QObject(parent)
{

}

void ImageColorsHelper::setSource(const QVariant &source)
{
#ifdef IMAGE_COLOR_HELPER_THREADED
    if (m_futureSourceImageData) {
        m_futureSourceImageData->cancel();
        m_futureSourceImageData->deleteLater();
        m_futureSourceImageData = nullptr;
    }
#endif

    if(source==m_source)
        return;

    if (source.canConvert<QQuickItem *>()) {
        setSourceItem(source.value<QQuickItem *>());
    } else if (source.canConvert<QImage>()) {
        setSourceImage(source.value<QImage>());
    } else if (source.canConvert<QIcon>()) {
        setSourceImage(source.value<QIcon>().pixmap(128, 128).toImage());
    } else if (source.canConvert<QString>()) {
        QString sourceString = source.toString();

        if(sourceString.startsWith("file://"))
            sourceString = QUrl(sourceString).toLocalFile();
        else if(sourceString.startsWith("qrc:/"))
            sourceString.replace(*qrcRegExp, ":/");

        if (QIcon::hasThemeIcon(sourceString)) {
            setSourceImage(QIcon::fromTheme(sourceString).pixmap(128, 128).toImage());
        } else {
#ifdef SVG_IMAGE_HELPER_THREADED
            QFuture<QImage> future = QtConcurrent::run([sourceString]() {
                if (auto url = QUrl(sourceString); url.isLocalFile()) {
                    return QImage(url.toLocalFile());
                }
                return QImage(sourceString);
            });
            m_futureSourceImageData = new QFutureWatcher<QImage>(this);
            connect(m_futureSourceImageData, &QFutureWatcherBase::finished, this, [this, source]() {
                if(!m_futureSourceImageData)
                    return;
                const QImage image = m_futureSourceImageData->future().result();
                m_futureSourceImageData->deleteLater();
                m_futureSourceImageData = nullptr;
                setSourceImage(image);
                m_source = source;
                emit this->sourceChanged();
            });
            m_futureSourceImageData->setFuture(future);
            return;
#else
            if (auto url = QUrl(sourceString); url.isLocalFile()) {
                setSourceImage(QImage(url.toLocalFile()));
            }
            else {
                setSourceImage(QImage(sourceString));
            }
#endif
        }
    } else {
        return;
    }

    m_source = source;
    emit this->sourceChanged();
}

QVariant ImageColorsHelper::source() const
{
    return m_source;
}

void ImageColorsHelper::setSourceImage(const QImage &image)
{
    if (m_window) {
        disconnect(m_window.data(), nullptr, this, nullptr);
    }
    if (m_sourceItem) {
        disconnect(m_sourceItem.data(), nullptr, this, nullptr);
    }
    if (m_grabResult) {
        disconnect(m_grabResult.data(), nullptr, this, nullptr);
        m_grabResult.clear();
    }

    m_sourceItem.clear();

    m_sourceImage = image;
    update();
}

QImage ImageColorsHelper::sourceImage() const
{
    return m_sourceImage;
}

void ImageColorsHelper::setSourceItem(QQuickItem *source)
{
    if (m_sourceItem == source) {
        return;
    }

    if (m_window) {
        disconnect(m_window.data(), nullptr, this, nullptr);
    }
    if (m_sourceItem) {
        disconnect(m_sourceItem, nullptr, this, nullptr);
    }
    m_sourceItem = source;
    update();

    if (m_sourceItem) {
        auto syncWindow = [this]() {
            if (m_window) {
                disconnect(m_window.data(), nullptr, this, nullptr);
            }
            m_window = m_sourceItem->window();
            if (m_window) {
                connect(m_window, &QWindow::visibleChanged, this, &ImageColorsHelper::update);
            }
            update();
        };

        connect(m_sourceItem, &QQuickItem::windowChanged, this, syncWindow);
        syncWindow();
    }
}

QQuickItem *ImageColorsHelper::sourceItem() const
{
    return m_sourceItem;
}

void ImageColorsHelper::update()
{
    setIsReady(false);

#ifdef IMAGE_COLOR_HELPER_THREADED
    if (m_futureImageData) {
        m_futureImageData->disconnect(this, nullptr);
        m_futureImageData->cancel();
        m_futureImageData->deleteLater();
        m_futureImageData = nullptr;
    }

    auto runUpdate = [this]() {
        auto sourceImage{m_sourceImage};
        QFuture<ImageData> future = QtConcurrent::run([sourceImage = std::move(sourceImage)]() {
            return generatePalette(sourceImage);
        });
        m_futureImageData = new QFutureWatcher<ImageData>(this);
        connect(m_futureImageData, &QFutureWatcherBase::finished, this, [this]() {
            if (!m_futureImageData)
                return;
            m_imageData = m_futureImageData->future().result();
            postProcess(m_imageData, m_backgroundColor, m_textColor);
            m_futureImageData->deleteLater();
            m_futureImageData = nullptr;

            emit this->paletteChanged();
            setIsReady(true);
        });
        m_futureImageData->setFuture(future);
    };
#else
    auto runUpdate = [this]() {
        m_imageData = generatePalette(m_sourceImage);
        postProcess(m_imageData, m_backgroundColor, m_textColor);
        emit this->paletteChanged();
        setIsReady(true);
    };
#endif

    if (!m_sourceItem || !m_sourceItem->window() || !m_sourceItem->window()->isVisible()) {
        if (!m_sourceImage.isNull()) {
            runUpdate();
        } else {
            m_imageData = {};
            emit this->paletteChanged();
            setIsReady(true);
        }
        return;
    }

    if (m_grabResult) {
        disconnect(m_grabResult.data(), nullptr, this, nullptr);
        m_grabResult.clear();
    }

    m_grabResult = m_sourceItem->grabToImage(QSize(128, 128));

    if (m_grabResult) {
        connect(m_grabResult.data(), &QQuickItemGrabResult::ready, this, [this, runUpdate]() {
            m_sourceImage = m_grabResult->image();
            m_grabResult.clear();
            runUpdate();
        });
    }
}

static inline int squareDistance(QRgb color1, QRgb color2)
{
    // https://en.wikipedia.org/wiki/Color_difference
    // Using RGB distance for performance, as CIEDE2000 is too complicated
    if (qRed(color1) - qRed(color2) < 128) {
        return 2 * pow(qRed(color1) - qRed(color2), 2) //
               + 4 * pow(qGreen(color1) - qGreen(color2), 2) //
               + 3 * pow(qBlue(color1) - qBlue(color2), 2);
    } else {
        return 3 * pow(qRed(color1) - qRed(color2), 2) //
               + 4 * pow(qGreen(color1) - qGreen(color2), 2) //
               + 2 * pow(qBlue(color1) - qBlue(color2), 2);
    }
}

void ImageColorsHelper::positionColor(QRgb rgb, QList<ImageData::colorStat> &clusters)
{
    for (auto &stat : clusters) {
        if (squareDistance(rgb, stat.centroid) < s_minimumSquareDistance) {
            stat.colors.append(rgb);
            return;
        }
    }

    ImageData::colorStat stat;
    stat.colors.append(rgb);
    stat.centroid = rgb;
    clusters << stat;
}

void ImageColorsHelper::positionColorMP(const decltype(ImageData::m_samples) &samples, decltype(ImageData::m_clusters) &clusters, int numCore)
{
    // Fall back to single thread
    for (auto color : samples) {
        positionColor(color, clusters);
    }
}

ImageData ImageColorsHelper::generatePalette(const QImage &sourceImage)
{
    ImageData imageData;

    if (sourceImage.isNull() || sourceImage.width() == 0) {
        return imageData;
    }

    imageData.m_clusters.clear();
    imageData.m_samples.clear();

    constexpr int numCore = 1;
    int r = 0;
    int g = 0;
    int b = 0;
    int c = 0;

    for (int x = 0; x < sourceImage.width(); ++x) {
        for (int y = 0; y < sourceImage.height(); ++y) {
            const QColor sampleColor = sourceImage.pixelColor(x, y);
            if (sampleColor.alpha() == 0) {
                continue;
            }
            if (ColorUtils::chroma(sampleColor) < 0) {
                continue;
            }
            QRgb rgb = sampleColor.rgb();
            ++c;
            r += qRed(rgb);
            g += qGreen(rgb);
            b += qBlue(rgb);
            imageData.m_samples << rgb;
        }
    }

    if (imageData.m_samples.isEmpty()) {
        return imageData;
    }

    positionColorMP(imageData.m_samples, imageData.m_clusters, numCore);

    if(c!=0)
        imageData.m_average = QColor(r / c, g / c, b / c, 255);
    else
        imageData.m_average = QColor(0, 0, 0, 255);

    for (int iteration = 0; iteration < 5; ++iteration) {
        for (int i = 0; i < imageData.m_clusters.size(); ++i) {
            auto &stat = imageData.m_clusters[i];
            r = 0;
            g = 0;
            b = 0;
            c = 0;

            for (auto color : std::as_const(stat.colors)) {
                c++;
                r += qRed(color);
                g += qGreen(color);
                b += qBlue(color);
            }
            if(c!=0) {
                r = r / c;
                g = g / c;
                b = b / c;
            }
            stat.centroid = qRgb(r, g, b);
            stat.ratio = std::clamp(double(stat.colors.count()) / double(imageData.m_samples.count()), 0.0, 1.0);
            stat.colors = QList<QRgb>({stat.centroid});
        }

        positionColorMP(imageData.m_samples, imageData.m_clusters, numCore);
    }
    std::stable_sort(imageData.m_clusters.begin(), imageData.m_clusters.end(), [](const ImageData::colorStat &a, const ImageData::colorStat &b) {
        return getClusterScore(a) > getClusterScore(b);
    });

    // compress blocks that became too similar
    auto sourceIt = imageData.m_clusters.end();
    // Use index instead of iterator, because QList::erase may invalidate iterator.
    std::vector<int> itemsToDelete;
    while (sourceIt != imageData.m_clusters.begin()) {
        sourceIt--;
        for (auto destIt = imageData.m_clusters.begin(); destIt != imageData.m_clusters.end() && destIt != sourceIt; destIt++) {
            if (squareDistance((*sourceIt).centroid, (*destIt).centroid) < s_minimumSquareDistance) {
                const double ratio = (*sourceIt).ratio / (*destIt).ratio;
                const int r = ratio * double(qRed((*sourceIt).centroid)) + (1 - ratio) * double(qRed((*destIt).centroid));
                const int g = ratio * double(qGreen((*sourceIt).centroid)) + (1 - ratio) * double(qGreen((*destIt).centroid));
                const int b = ratio * double(qBlue((*sourceIt).centroid)) + (1 - ratio) * double(qBlue((*destIt).centroid));
                (*destIt).ratio += (*sourceIt).ratio;
                (*destIt).centroid = qRgb(r, g, b);
                itemsToDelete.push_back(std::distance(imageData.m_clusters.begin(), sourceIt));
                break;
            }
        }
    }

    for (auto i : std::as_const(itemsToDelete)) {
        imageData.m_clusters.removeAt(i);
    }

    imageData.m_highlight = QColor();
    imageData.m_dominant = QColor(imageData.m_clusters.first().centroid);
    imageData.m_closestToBlack = Qt::white;
    imageData.m_closestToWhite = Qt::black;

    imageData.m_palette.clear();
    imageData.m_colors.clear();

    bool first = true;

    for (int i = 0; i < imageData.m_clusters.size(); ++i) {
        const auto &stat = imageData.m_clusters[i];
        const QColor color(stat.centroid);

        QColor contrast = QColor(255 - color.red(), 255 - color.green(), 255 - color.blue());
        contrast.setHsl(contrast.hslHue(), //
                        contrast.hslSaturation(), //
                        128 + (128 - contrast.lightness()));
        QColor tempContrast;
        int minimumDistance = 4681800; // max distance: 4*3*2*3*255*255
        for (const auto &stat : std::as_const(imageData.m_clusters)) {
            const int distance = squareDistance(contrast.rgb(), stat.centroid);

            if (distance < minimumDistance) {
                tempContrast = QColor(stat.centroid);
                minimumDistance = distance;
            }
        }

        QColor inverted = ColorUtils::invert(imageData.m_dominant);
        QColor complementary = ColorUtils::complement(imageData.m_dominant);

        if (imageData.m_clusters.size() <= 3) {
            if (qGray(imageData.m_dominant.rgb()) < 120) {
                contrast = QColor(230, 230, 230);
            } else {
                contrast = QColor(20, 20, 20);
            }
            // TODO: replace m_clusters.size() > 3 with entropy calculation
        } else if (squareDistance(contrast.rgb(), tempContrast.rgb()) < s_minimumSquareDistance * 1.5) {
            contrast = tempContrast;
        } else {
            contrast = tempContrast;
            contrast.setHsl(contrast.hslHue(),
                            contrast.hslSaturation(),
                            contrast.lightness() > 128 ? qMin(contrast.lightness() + 20, 255) : qMax(0, contrast.lightness() - 20));
        }

        if (first) {
            imageData.m_dominantContrast = contrast;
            imageData.m_dominantInverted = inverted;
            imageData.m_dominantComplementary = complementary;
            imageData.m_dominant = color;
        }
        first = false;

        if (!imageData.m_highlight.isValid() || ColorUtils::chroma(color) > ColorUtils::chroma(imageData.m_highlight)) {
            imageData.m_highlight = color;
        }

        if (qGray(color.rgb()) > qGray(imageData.m_closestToWhite.rgb())) {
            imageData.m_closestToWhite = color;
        }
        if (qGray(color.rgb()) < qGray(imageData.m_closestToBlack.rgb())) {
            imageData.m_closestToBlack = color;
        }
        imageData.m_palette << PaletteSwatch(stat.ratio, color, contrast);
    }

    return imageData;
}

double ImageColorsHelper::getClusterScore(const ImageData::colorStat &stat)
{
    return stat.ratio * ColorUtils::chroma(QColor(stat.centroid));
}

void ImageColorsHelper::postProcess(ImageData &imageData, const QColor &backgroundColor, const QColor &textColor) const
{
    constexpr short unsigned WCAG_NON_TEXT_CONTRAST_RATIO = 3;
    constexpr double WCAG_TEXT_CONTRAST_RATIO = 4.5;

    const double backgroundLum = ColorUtils::luminance(backgroundColor);
    double lowerLum, upperLum;
    // 192 is from kcm_colors
    if (qGray(backgroundColor.rgb()) < 192) {
        // (lowerLum + 0.05) / (backgroundLum + 0.05) >= 3
        lowerLum = WCAG_NON_TEXT_CONTRAST_RATIO * (backgroundLum + 0.05) - 0.05;
        upperLum = 0.95;
    } else {
        // For light themes, still prefer lighter colors
        // (lowerLum + 0.05) / (textLum + 0.05) >= 4.5
        const double textLum = ColorUtils::luminance(textColor);
        lowerLum = WCAG_TEXT_CONTRAST_RATIO * (textLum + 0.05) - 0.05;
        upperLum = backgroundLum;
    }

    auto adjustSaturation = [](QColor &color) {
        // Adjust saturation to make the color more vibrant
        if (color.hsvSaturationF() < 0.5) {
            const double h = color.hsvHueF();
            const double v = color.valueF();
            color.setHsvF(h, 0.5, v);
        }
    };
    adjustSaturation(imageData.m_dominant);
    adjustSaturation(imageData.m_highlight);
    adjustSaturation(imageData.m_average);
    adjustSaturation(imageData.m_dominantInverted);
    adjustSaturation(imageData.m_dominantComplementary);

    auto adjustLightness = [lowerLum, upperLum](QColor &color) {
        short unsigned colorOperationCount = 0;
        const double h = color.hslHueF();
        const double s = color.hslSaturationF();
        const double l = color.lightnessF();
        while (ColorUtils::luminance(color.rgb()) < lowerLum && colorOperationCount++ < 10) {
            color.setHslF(h, s, std::min(1.0, l + colorOperationCount * 0.03));
        }
        while (ColorUtils::luminance(color.rgb()) > upperLum && colorOperationCount++ < 10) {
            color.setHslF(h, s, std::max(0.0, l - colorOperationCount * 0.03));
        }
    };
    adjustLightness(imageData.m_dominant);
    adjustLightness(imageData.m_highlight);
    adjustLightness(imageData.m_average);
    adjustLightness(imageData.m_dominantInverted);
    adjustLightness(imageData.m_dominantComplementary);

    imageData.m_colors += imageData.m_average;
    imageData.m_colors += imageData.m_dominant;
    imageData.m_colors += imageData.m_dominantContrast;
    imageData.m_colors += imageData.m_dominantInverted;
    imageData.m_colors += imageData.m_dominantComplementary;
    imageData.m_colors += imageData.m_highlight;

    if (qGray(imageData.m_closestToWhite.rgb()) < 200)
        imageData.m_colors += QColor(230, 230, 230);
    else
        imageData.m_colors += imageData.m_closestToWhite;

    if (qGray(imageData.m_closestToBlack.rgb()) > 80)
        imageData.m_colors += QColor(20, 20, 20);
    else
        imageData.m_colors += imageData.m_closestToBlack;
}

QList<PaletteSwatch> ImageColorsHelper::palette() const
{
#ifdef IMAGE_COLOR_HELPER_THREADED
    if (m_futureImageData) {
        AXIONLOG_WARNING()<<m_futureImageData->future().isFinished();
    }
#endif
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackPalette;
    return m_imageData.m_palette;
}

QVariantList ImageColorsHelper::colors() const
{
#ifdef IMAGE_COLOR_HELPER_THREADED
    if (m_futureImageData) {
        AXIONLOG_WARNING()<<m_futureImageData->future().isFinished();
    }
#endif
    return m_imageData.m_colors;
}

ColorBrightnesses::Enum ImageColorsHelper::paletteBrightness() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackPaletteBrightness;
    return qGray(m_imageData.m_dominant.rgb()) < 128 ? ColorBrightnesses::Dark : ColorBrightnesses::Light;
}

QColor ImageColorsHelper::average() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackAverage;
    return m_imageData.m_average;
}

QColor ImageColorsHelper::dominant() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackDominant;
    return m_imageData.m_dominant;
}

QColor ImageColorsHelper::dominantInverted() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackDominantInverted;
    return m_imageData.m_dominantInverted;
}

QColor ImageColorsHelper::dominantComplementary() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackDominantComplementary;
    return m_imageData.m_dominantComplementary;
}

QColor ImageColorsHelper::dominantContrast() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackDominantContrasting;
    return m_imageData.m_dominantContrast;
}

QColor ImageColorsHelper::foreground() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackForeground;

    if (paletteBrightness() == ColorBrightnesses::Dark) {
        if (qGray(m_imageData.m_closestToWhite.rgb()) < 200) {
            return QColor(230, 230, 230);
        }
        return m_imageData.m_closestToWhite;
    } else {
        if (qGray(m_imageData.m_closestToBlack.rgb()) > 80) {
            return QColor(20, 20, 20);
        }
        return m_imageData.m_closestToBlack;
    }
}

QColor ImageColorsHelper::background() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackBackground;

    if (paletteBrightness() == ColorBrightnesses::Dark) {
        if (qGray(m_imageData.m_closestToBlack.rgb()) > 80) {
            return QColor(20, 20, 20);
        }
        return m_imageData.m_closestToBlack;
    } else {
        if (qGray(m_imageData.m_closestToWhite.rgb()) < 200) {
            return QColor(230, 230, 230);
        }
        return m_imageData.m_closestToWhite;
    }
}

QColor ImageColorsHelper::highlight() const
{
    if (m_imageData.m_samples.size() == 0)
        return m_fallbackHighlight;
    return m_imageData.m_highlight;
}

QColor ImageColorsHelper::closestToWhite() const
{
    if (m_imageData.m_samples.size() == 0)
        return Qt::white;
    if (qGray(m_imageData.m_closestToWhite.rgb()) < 200) {
        return QColor(230, 230, 230);
    }
    return m_imageData.m_closestToWhite;
}

QColor ImageColorsHelper::closestToBlack() const
{
    if (m_imageData.m_samples.size() == 0)
        return Qt::black;
    if (qGray(m_imageData.m_closestToBlack.rgb()) > 80) {
        return QColor(20, 20, 20);
    }
    return m_imageData.m_closestToBlack;
}
