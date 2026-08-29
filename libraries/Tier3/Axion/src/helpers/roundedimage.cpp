#include "roundedimage.h"

#include <QImageReader>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlFile>
#include <QQuickImageProvider>

RoundedImage::RoundedImage(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setSmooth(true);
    setAntialiasing(true);
    setOpaquePainting(false);

    connect(this, &RoundedImage::sourceChanged, this, &RoundedImage::reloadImage);
    connect(this, &RoundedImage::fillModeChanged, this, &QQuickItem::update);
    connect(this, &RoundedImage::radiusChanged, this, &QQuickItem::update);
}

void RoundedImage::paint(QPainter *painter)
{
    const QRectF bounds(0.0, 0.0, width(), height());
    if (bounds.isEmpty() || m_image.isNull())
        return;

    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, smooth());

    painter->setClipPath(clipPath(bounds));
    drawImage(painter, bounds);
}

void RoundedImage::componentComplete()
{
    QQuickPaintedItem::componentComplete();
    m_completed = true;
    reloadImage();
}

void RoundedImage::reloadImage()
{
    if (!m_completed)
        return;

    m_image = loadImage(m_source);

    if (!m_image.isNull())
        setImplicitSize(m_image.width(), m_image.height());
    else
        setImplicitSize(0.0, 0.0);

    update();
}

QRectF RoundedImage::alignedRect(const QSizeF &contentSize, const QRectF &bounds) const
{
    return QRectF(bounds.x() + (bounds.width() - contentSize.width()) / 2.0,
                  bounds.y() + (bounds.height() - contentSize.height()) / 2.0,
                  contentSize.width(),
                  contentSize.height());
}

QRectF RoundedImage::imageTargetRect(const QSizeF &imageSize, const QRectF &bounds) const
{
    if (imageSize.isEmpty())
        return QRectF();

    switch (m_fillMode) {
    case RoundedImageFillModes::PreserveAspectFit:
        return alignedRect(imageSize.scaled(bounds.size(), Qt::KeepAspectRatio), bounds);
    case RoundedImageFillModes::PreserveAspectCrop:
        return alignedRect(imageSize.scaled(bounds.size(), Qt::KeepAspectRatioByExpanding), bounds);
    case RoundedImageFillModes::Pad:
        return alignedRect(imageSize, bounds);
    case RoundedImageFillModes::Stretch:
    case RoundedImageFillModes::Tile:
    case RoundedImageFillModes::TileVertically:
    case RoundedImageFillModes::TileHorizontally:
        return bounds;
    }

    return bounds;
}

QPainterPath RoundedImage::clipPath(const QRectF &bounds) const
{
    QPainterPath path;
    const qreal radius = boundedRadius(bounds);
    path.addRoundedRect(bounds, radius, radius);
    return path;
}

QImage RoundedImage::loadImage(const QUrl &url) const
{
    if (url.isEmpty())
        return QImage();

    QUrl resolvedUrl = url;
    if (resolvedUrl.isRelative()) {
        if (QQmlContext *context = qmlContext(this))
            resolvedUrl = context->resolvedUrl(resolvedUrl);
    }

    if (resolvedUrl.scheme() == QLatin1String("image"))
        return loadProviderImage(resolvedUrl);

    const QString filePath = QQmlFile::urlToLocalFileOrQrc(resolvedUrl);
    QImageReader reader(filePath);
    reader.setAutoTransform(true);
    return reader.read();
}

QImage RoundedImage::loadProviderImage(const QUrl &url) const
{
    QQmlEngine *engine = qmlEngine(this);
    if (!engine)
        return QImage();

    QQmlImageProviderBase *baseProvider = engine->imageProvider(url.host());
    QQuickImageProvider *provider = dynamic_cast<QQuickImageProvider *>(baseProvider);
    if (!provider)
        return QImage();

    QString id = url.path();
    if (id.startsWith(QLatin1Char('/')))
        id.remove(0, 1);

    if (url.hasQuery())
        id += QLatin1Char('?') + url.query();

    QSize originalSize;
    switch (provider->imageType()) {
    case QQmlImageProviderBase::Image:
        return provider->requestImage(id, &originalSize, QSize());
    case QQmlImageProviderBase::Pixmap:
        return provider->requestPixmap(id, &originalSize, QSize()).toImage();
    case QQmlImageProviderBase::Invalid:
    case QQmlImageProviderBase::Texture:
    case QQmlImageProviderBase::ImageResponse:
        return QImage();
    }

    return QImage();
}

void RoundedImage::drawImage(QPainter *painter, const QRectF &bounds)
{
    if (m_fillMode == RoundedImageFillModes::Tile ||
        m_fillMode == RoundedImageFillModes::TileVertically ||
        m_fillMode == RoundedImageFillModes::TileHorizontally) {
        drawTiledImage(painter, bounds);
        return;
    }

    const QRectF target = imageTargetRect(m_image.size(), bounds);
    if (!target.isEmpty())
        painter->drawImage(target, m_image);
}

void RoundedImage::drawTiledImage(QPainter *painter, const QRectF &bounds)
{
    if (m_image.isNull())
        return;

    const qreal imageWidth = m_image.width();
    const qreal imageHeight = m_image.height();
    if (imageWidth <= 0.0 || imageHeight <= 0.0)
        return;

    if (m_fillMode == RoundedImageFillModes::Tile) {
        for (qreal y = bounds.top(); y < bounds.bottom(); y += imageHeight) {
            for (qreal x = bounds.left(); x < bounds.right(); x += imageWidth)
                painter->drawImage(QRectF(x, y, imageWidth, imageHeight), m_image);
        }
        return;
    }

    if (m_fillMode == RoundedImageFillModes::TileVertically) {
        for (qreal y = bounds.top(); y < bounds.bottom(); y += imageHeight)
            painter->drawImage(QRectF(bounds.left(), y, bounds.width(), imageHeight), m_image);
        return;
    }

    if (m_fillMode == RoundedImageFillModes::TileHorizontally) {
        for (qreal x = bounds.left(); x < bounds.right(); x += imageWidth)
            painter->drawImage(QRectF(x, bounds.top(), imageWidth, bounds.height()), m_image);
    }
}

qreal RoundedImage::boundedRadius(const QRectF &bounds) const
{
    return qBound<qreal>(0.0, m_radius, qMin(bounds.width(), bounds.height()) / 2.0);
}
