#include "thermalzonemodel.h"
#include "solid_log.h"

#include <QtConcurrentRun>

#define THERMALCLASS_FILE_PATH (QStringLiteral("/sys/class/thermal/"))

ThermalZoneModel::ThermalZoneModel(QObject *parent) :
    QVariantListModel(parent)
{
    m_baseName = THERMALCLASS_FILE_PATH;
    m_primaryField = "key";
    m_syncable = true;
    m_selectPolicy = QVariantListModelPolicies::Delayed;
    m_submitPolicy = QVariantListModelPolicies::Disabled;

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ThermalZoneModel::queueSelect);
    timer->setSingleShot(false);

    connect(this, &ThermalZoneModel::tickChanged, this, [timer](int tick) {
        timer->stop();
        if(tick<=0)
            return;
        timer->setInterval(1000);
        timer->start();
    });
}

bool ThermalZoneModel::doSelect()
{
    if(!m_selectWatcher.isNull()) {
        m_selectWatcher->cancel();
        m_selectWatcher->deleteLater();
    }

    QFuture<QVariantList> future = QtConcurrent::run([]() {
        if(!QFileInfo::exists(THERMALCLASS_FILE_PATH))
            return QVariantList();

        QVariantList storage;
        const QFileInfoList thermal_zones = QDir(THERMALCLASS_FILE_PATH).entryInfoList(QStringList()<<"thermal_zone*", QDir::Dirs, QDir::Name);
        for(const QFileInfo& thermal_zone: thermal_zones)
        {
            QVariantMap info;

            QFile type(thermal_zone.absoluteFilePath()+"/type");
            if(type.open(QIODevice::ReadOnly))
                info["key"] = type.readAll().trimmed();
            else
                info["key"] = "N/A";

            QFile temp(thermal_zone.absoluteFilePath()+"/temp");
            if(temp.open(QIODevice::ReadOnly))
                info["value"] = temp.readAll().trimmed();
            else
                info["value"] = 0;

            storage.append(info);
        }

        return storage;
    });

    m_selectWatcher = new QFutureWatcher<QVariantList>(this);
    connect(m_selectWatcher, &QFutureWatcherBase::finished, this, [this]() {
        if(m_selectWatcher.isNull())
            return;
        QVariantList storage = m_selectWatcher->future().result();
        setStorage(std::move(storage));
        emitSelectDone(true);
    });
    m_selectWatcher->setFuture(future);

    return true;
}
