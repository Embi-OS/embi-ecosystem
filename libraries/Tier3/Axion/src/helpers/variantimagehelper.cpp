#include "variantimagehelper.h"
#include "variantimageprovider.h"

#include <QImage>
#include <QIcon>

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, qrcRegExp, ("qrc:/+"))

VariantImageHelper::VariantImageHelper(QObject * parent):
    QObject(parent),
    m_enabled(false),
    m_ready(false),
    m_source(QImage()),
    m_backup(QString()),
    m_image(QImage()),
    m_completed(false),
    m_inhibitTimer(this)
{
    m_inhibitTimer.setInterval(50);
    m_inhibitTimer.setSingleShot(true);
    connect(&m_inhibitTimer, &QTimer::timeout, this, &VariantImageHelper::doProcessImage, Qt::UniqueConnection);

    connect(this, &VariantImageHelper::enabledChanged, this, &VariantImageHelper::scheduleRefresh);
    connect(this, &VariantImageHelper::sourceChanged, this, &VariantImageHelper::scheduleRefresh);
    connect(this, &VariantImageHelper::backupChanged, this, &VariantImageHelper::onBackupChanged);
}

QImage VariantImageHelper::stringToImage(const QString& source)
{
    QImage image;
    QString sourceString = source;

    if(sourceString.startsWith("file://"))
        sourceString = QUrl(sourceString).toLocalFile();
    else if(sourceString.startsWith("qrc:/"))
        sourceString.replace(*qrcRegExp, ":/");

    if (QIcon::hasThemeIcon(sourceString)) {
        image = QIcon::fromTheme(sourceString).pixmap(256, 256).toImage();
    } else {
        QUrl url = QUrl(sourceString);

        if (url.isLocalFile())
            image = QImage(url.toLocalFile());
        else
            image = QImage(sourceString);
    }

    return image;
}

void VariantImageHelper::classBegin()
{
    m_completed = false;
}

void VariantImageHelper::componentComplete()
{
    m_completed = true;
    scheduleRefresh();
}

void VariantImageHelper::setTarget(const QQmlProperty& target)
{
    m_property = target;
    scheduleRefresh();
}

void VariantImageHelper::scheduleRefresh()
{
    setReady(false);
    m_inhibitTimer.stop();
    m_inhibitTimer.start();
}

void VariantImageHelper::doForceRegen()
{
    scheduleRefresh();
}

void VariantImageHelper::doUpdateProperty(const QVariant& source)
{
    if(m_property.isValid() && m_property.isWritable())
    {
        if(!m_property.write(source))
        {
            qWarning()<<">>> VariantImageHelper : Can't set"<<source;
            doForceRegen();
        }
    }
    setReady(true);
}

void VariantImageHelper::doProcessImage()
{
    if(!m_completed || !m_enabled)
        return;

    if (m_source.canConvert<QImage>()) {
        setSourceImage(m_source.value<QImage>());
    } else if (m_source.canConvert<QIcon>()) {
        setSourceImage(m_source.value<QIcon>().pixmap(256, 256).toImage());
    } else if (m_source.canConvert<QString>()) {
        QString sourceString = m_source.toString();
        setSourceImage(VariantImageHelper::stringToImage(sourceString));
    }
    else {
        setSourceImage(QImage());
    }
}

void VariantImageHelper::onBackupChanged(const QString& backup)
{
    m_backupImage = VariantImageHelper::stringToImage(backup);
    scheduleRefresh();
}

void VariantImageHelper::setSourceImage(const QImage &sourceImage)
{
    QImage image = sourceImage;

    if(image.isNull())
        image = m_backupImage;

    const QString imageId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    VariantImageProvider::removeImage(m_imageId);
    VariantImageProvider::registerImage(imageId, image);
    const QString imageData = QString::fromLatin1("image://VariantImage/%1").arg(imageId);

    setImageId(imageId);
    setImage(image);
    doUpdateProperty(imageData);
}
