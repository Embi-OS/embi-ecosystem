#ifndef VARIANTIMAGEPROVIDER_H
#define VARIANTIMAGEPROVIDER_H

#include <QDefs>

#include <QQuickImageProvider>

class VariantImageProvider : public QQuickImageProvider
{
public:
    VariantImageProvider();

    static void registerImage(const QString &id, const QImage &image);
    static void removeImage(const QString &id);

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override final;
};

#endif // VARIANTIMAGEPROVIDER_H
