#ifndef MEDIAMETADATAMODEL_H
#define MEDIAMETADATAMODEL_H

#include <QDefs>
#include <QModels>

#include <QMediaMetaData>

class MediaMetaDataModel: public QVariantListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QMediaMetaData, metaData, MetaData, {})

public:
    explicit MediaMetaDataModel(QObject* parent=nullptr);

protected:
    QVariantList selectSource(bool* result=nullptr) override final;
};

#endif // MEDIAMETADATAMODEL_H
