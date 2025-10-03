#include "mediametadatamodel.h"
#include "media_log.h"

MediaMetaDataModel::MediaMetaDataModel(QObject* parent):
    QVariantListModel(parent)
{
    m_syncable = true;
    m_primaryField = "key";

    connect(this, &MediaMetaDataModel::metaDataChanged, this, &QVariantListModel::queueSelect);

}

QVariantList MediaMetaDataModel::selectSource(bool* result)
{
    QVariantList storage;

    const QList<QMediaMetaData::Key> keys = m_metaData.keys();
    for(QMediaMetaData::Key key: keys)
    {
        QVariantMap map;
        map.insert("key", key);
        map.insert("name", m_metaData.metaDataKeyToString(key));
        map.insert("value", m_metaData.stringValue(key));
        map.insert("raw", m_metaData.value(key));
        storage.append(map);
    }

    if(result)
        *result=true;

    return storage;
}
