#include "variantimageprovider.h"
#include "axion_log.h"

typedef QMap<QString, QImage> Cache;
Q_GLOBAL_STATIC(Cache, s_cache)

VariantImageProvider::VariantImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Image)
{

}

void VariantImageProvider::registerImage(const QString &id, const QImage &image)
{
    Cache *d = s_cache();
    d->insert(id, image);

    AXIONLOG_DEBUG()<<"VariantImageProvider register image:"<<id<<image;
}

void VariantImageProvider::removeImage(const QString &id)
{
    Cache *d = s_cache();
    d->remove(id);

    AXIONLOG_DEBUG()<<"VariantImageProvider remove image:"<<id;
}

QImage VariantImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Cache *d = s_cache();

    QImage res = d->value(id);
    if (res.isNull())
        return QImage();

    if (!requestedSize.isEmpty())
        res = res.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    if (size)
        *size = res.size();

    return res;
}
