#include "thermalzonemodel.h"
#include "fluid_log.h"

#define THERMALCLASS_FILE_PATH (QStringLiteral("/sys/class/thermal/"))

#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
#define THERMALZONE_BASE_NAME THERMALCLASS_FILE_PATH
#else
#define THERMALZONE_BASE_NAME QStringLiteral("thermal-zones")
#endif

ThermalZoneModel::ThermalZoneModel(QObject *parent) :
    QVariantListModel(parent)
{
    m_baseName = THERMALZONE_BASE_NAME;
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
    QVariantList storage = selectZones();
    setStorage(std::move(storage));
    emitSelectDone(true);

    return true;
}

QVariantList ThermalZoneModel::selectZones()
{
#if defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
    if(!QFileInfo::exists(THERMALCLASS_FILE_PATH))
        return QVariantList();

    QVariantList storage;
    const QFileInfoList thermal_zones = QDir(THERMALCLASS_FILE_PATH).entryInfoList(QStringList()<<"thermal_zone*", QDir::Dirs, QDir::Name);
    for(const QFileInfo& thermal_zone: thermal_zones)
    {
        QVariantMap info;
        info["key"] = thermal_zone.fileName();

        QFile type(thermal_zone.absoluteFilePath()+"/type");
        if(type.open(QIODevice::ReadOnly))
            info["type"] = type.readAll().trimmed();
        else
            info["type"] = "N/A";

        QFile temp(thermal_zone.absoluteFilePath()+"/temp");
        if(temp.open(QIODevice::ReadOnly))
        {
            bool ok = false;
            const int value = temp.readAll().trimmed().toInt(&ok);
            info["value"] = ok ? value : 0;
        }
        else
            info["value"] = 0;

        storage.append(info);
    }

    return storage;
#else
    return QVariantList();
#endif
}
