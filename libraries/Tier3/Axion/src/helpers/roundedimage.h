#ifndef ROUNDEDIMAGE_H
#define ROUNDEDIMAGE_H

#include <QDefs>
#include <QQuickPaintedItem>
#include <QUrl>

Q_ENUM_CLASS(RoundedImageFillModes, RoundedImageFillMode,
             Stretch = 0,
             PreserveAspectFit = 1,
             PreserveAspectCrop = 2,
             Tile = 3,
             TileVertically = 4,
             TileHorizontally = 5,
             Pad = 6)

class RoundedImage : public QQuickPaintedItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QUrl, source, Source, {})
    Q_WRITABLE_VAR_PROPERTY(RoundedImageFillModes::Enum, fillMode, FillMode, {})
    Q_WRITABLE_FUZ_PROPERTY(qreal, radius, Radius, 0)

public:
    explicit RoundedImage(QQuickItem *parent = nullptr);

    void paint(QPainter *painter) override;

protected:
    void componentComplete() override;

private slots:
    void reloadImage();

private:
    QRectF alignedRect(const QSizeF &contentSize, const QRectF &bounds) const;
    QRectF imageTargetRect(const QSizeF &imageSize, const QRectF &bounds) const;
    QPainterPath clipPath(const QRectF &bounds) const;
    QImage loadImage(const QUrl &url) const;
    QImage loadProviderImage(const QUrl &url) const;
    void drawImage(QPainter *painter, const QRectF &bounds);
    void drawTiledImage(QPainter *painter, const QRectF &bounds);
    qreal boundedRadius(const QRectF &bounds) const;

    bool m_completed = false;
    QImage m_image;
};

#endif // ROUNDEDIMAGE_H
