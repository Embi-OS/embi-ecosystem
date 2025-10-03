#ifndef VARIANTIMAGEHELPER_H
#define VARIANTIMAGEHELPER_H

#include <QDefs>

#include <QImage>
#include <QQmlParserStatus>
#include <QQmlPropertyValueSource>

class VariantImageHelper : public QObject,
                           public QQmlParserStatus,
                           public QQmlPropertyValueSource
{
    Q_OBJECT
    QML_ELEMENT

    Q_INTERFACES (QQmlParserStatus)
    Q_INTERFACES (QQmlPropertyValueSource)

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_READONLY_VAR_PROPERTY(bool, ready, Ready, false)

    Q_WRITABLE_REF_PROPERTY(QVariant, source, Source, {})
    Q_WRITABLE_REF_PROPERTY(QString, backup, Backup, {})
    Q_READONLY_REF_PROPERTY(QImage, image, Image, {})
    Q_READONLY_REF_PROPERTY(QString, imageId, ImageId, {})

public:
    explicit VariantImageHelper(QObject* parent=nullptr);

    static QImage stringToImage(const QString& source);

    void classBegin() override;
    void componentComplete() override;
    void setTarget(const QQmlProperty& target) override;

public slots:
    void scheduleRefresh();

protected:
    void doProcessImage();
    void doForceRegen();
    void doUpdateProperty(const QVariant& source);

    void onBackupChanged(const QString& backup);
    void setSourceImage(const QImage &sourceImage);

private:
    bool m_completed;
    QTimer m_inhibitTimer;
    QQmlProperty m_property;

    QImage m_backupImage;
};

#endif // VARIANTIMAGEHELPER_H
