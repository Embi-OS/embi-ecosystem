#include "qrcodeimageprovider.h"

#include "vendors/qrcodegen/qrcodegen.hpp"

QrCodeImageProvider::QrCodeImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Image)
{

}

static QImage toSvgImage(const qrcodegen::QrCode &qrCode, quint16 border, quint16 size)
{
    const int qrSize = qrCode.getSize();
    const int totalSize = qrSize + 2 * border;
    // Calculate scaling factor to fit requested size
    const int pixelSize = size / totalSize;
    const int imageSize = pixelSize * totalSize;
    // Create the output image
    QImage image(imageSize, imageSize, QImage::Format_ARGB32);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    // Draw each QR module (black square)
    for (int y = 0; y < qrSize; ++y)
    {
        for (int x = 0; x < qrSize; ++x)
        {
            if (qrCode.getModule(x, y))
            {
                int xPos = (x + border) * pixelSize;
                int yPos = (y + border) * pixelSize;
                const QRect r(xPos, yPos, pixelSize, pixelSize);
                painter.fillRect(r, Qt::black);
            }
        }
    }

    return image;
}

QImage QrCodeImageProvider::requestImage(const QString &data, QSize *size, const QSize &requestedSize)
{
    if(data.isEmpty()) {
        QImage image(requestedSize, QImage::Format_ARGB32);
        image.fill(Qt::transparent);
        if (size)
            *size = requestedSize;
        return image;
    }

    const auto qrCode = qrcodegen::QrCode::encodeText(data.toUtf8().constData(), qrcodegen::QrCode::Ecc::LOW);

    QImage res = ::toSvgImage(qrCode, 0, qMin(requestedSize.width(), requestedSize.height()));
    if (res.isNull())
        return QImage();

    if (!requestedSize.isEmpty())
        res = res.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (size)
        *size = res.size();

    return res;
}
