#ifndef QRCODEIMAGEPROVIDER_H
#define QRCODEIMAGEPROVIDER_H

#include <QDefs>

#include <QQuickImageProvider>

class QrCodeImageProvider : public QQuickImageProvider
{
public:
    QrCodeImageProvider();

    QImage requestImage(const QString &data, QSize *size, const QSize &requestedSize) override final;
};

#endif // QRCODEIMAGEPROVIDER_H
