#include "cpuinfomodel.h"
#include "fluid_log.h"

#include <QSysInfo>
#include <QThread>

#if defined(Q_OS_WINDOWS)
#include <QSettings>
#endif

#define CPUINFO_FILE_PATH (QStringLiteral("/proc/cpuinfo"))
#define CPUINFO_WINDOWS_REGISTRY_PATH (QStringLiteral("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor"))

#if defined(Q_OS_WINDOWS)
#define CPUINFO_BASE_NAME CPUINFO_WINDOWS_REGISTRY_PATH
#else
#define CPUINFO_BASE_NAME CPUINFO_FILE_PATH
#endif

CpuInfoModel::CpuInfoModel(QObject* parent):
    QVariantListModel(parent)
{
    m_baseName = CPUINFO_BASE_NAME;
    m_primaryField = "key";
    m_syncable = true;
    m_selectPolicy = QVariantListModelPolicies::Manual;
    m_submitPolicy = QVariantListModelPolicies::Disabled;

    select();
}

QVariantList CpuInfoModel::selectSource(bool* result)
{
    QVariantList storage;
    m_model = QString();
    m_revision = QStringLiteral("N/A");
    m_serial = QStringLiteral("N/A");
    m_productId = QStringLiteral("N/A");
    m_coreCount = 0;

#if defined(Q_OS_WINDOWS)
    QSettings cpuRegistry(CPUINFO_WINDOWS_REGISTRY_PATH, QSettings::NativeFormat);
    const QStringList cpuKeys = cpuRegistry.childGroups();
    for(const QString& cpuKey: cpuKeys)
    {
        cpuRegistry.beginGroup(cpuKey);

        const QVariantMap values = {
            {QStringLiteral("ProcessorNameString"), cpuRegistry.value(QStringLiteral("ProcessorNameString"))},
            {QStringLiteral("Identifier"), cpuRegistry.value(QStringLiteral("Identifier"))},
            {QStringLiteral("VendorIdentifier"), cpuRegistry.value(QStringLiteral("VendorIdentifier"))},
            {QStringLiteral("MHz"), cpuRegistry.value(QStringLiteral("~MHz"))},
        };

        if(m_model.isEmpty())
            m_model = values.value(QStringLiteral("ProcessorNameString")).toString().trimmed();

        for(auto it = values.constBegin(); it != values.constEnd(); ++it)
        {
            const QString value = it.value().toString().trimmed();
            if(value.isEmpty())
                continue;

            QVariantMap info;
            info["key"] = QStringLiteral("%1_%2").arg(cpuKey, it.key());
            info["name"] = it.key();
            info["value"] = value;

            storage.append(info);
        }

        cpuRegistry.endGroup();
    }

    m_coreCount = cpuKeys.size();

#elif defined(Q_OS_LINUX) || defined(Q_OS_ANDROID)
    if(QFile::exists(CPUINFO_FILE_PATH))
    {
        QFile file(CPUINFO_FILE_PATH);
        if(file.open(QIODevice::ReadOnly))
        {
            m_coreCount = 0;

            QTextStream fileStream(&file);
            do
            {
                QString line = fileStream.readLine();
                FLUIDLOG_TRACE().noquote()<<line;

                QStringList lineSplit = line.split(":");
                QString keyPart;
                QString valuePart;

                if(lineSplit.length() == 2)
                {
                    keyPart = lineSplit.at(0).trimmed(); //Trim extra spaces
                    valuePart = lineSplit.at(1).trimmed(); //Trim extra spaces
                }
                else if(lineSplit.length() == 1)
                {
                    keyPart = lineSplit.at(0).trimmed(); //Trim extra spaces
                }

                if(keyPart == "processor")
                    m_coreCount++;

                if(keyPart == "model name")
                    m_model = valuePart;

                if(keyPart == "Model")
                    m_model = valuePart;

                if(keyPart == "Serial")
                    m_serial = valuePart;

                if(keyPart == "Revision")
                    m_revision = valuePart;

                if(!keyPart.isEmpty())
                {
                    QVariantMap info;
                    info["key"] = QString("%1_%2").arg(m_coreCount).arg(keyPart);
                    info["name"] = keyPart;
                    info["value"] = valuePart;

                    storage.append(info);
                }

            }
            while(!fileStream.atEnd());

            file.close();
        }
    }

    if(QFile::exists("/proc/device-tree/model"))
    {
        QFile file("/proc/device-tree/model");
        if(file.open(QIODevice::ReadOnly))
            m_model = file.readAll().trimmed();
    }
    else if(QFile::exists("/sys/firmware/devicetree/base/model"))
    {
        QFile file("/sys/firmware/devicetree/base/model");
        if(file.open(QIODevice::ReadOnly))
            m_model = file.readAll().trimmed();
    }

    if(QFile::exists("/proc/device-tree/toradex,board-rev"))
    {
        QFile file("/proc/device-tree/toradex,board-rev");
        if(file.open(QIODevice::ReadOnly))
            m_revision = file.readAll().trimmed();
    }

    if(QFile::exists("/proc/device-tree/serial"))
    {
        QFile file("/proc/device-tree/serial");
        if(file.open(QIODevice::ReadOnly))
            m_serial = file.readAll().trimmed();
    }
    else if(QFile::exists("/proc/device-tree/serial-number"))
    {
        QFile file("/proc/device-tree/serial-number");
        if(file.open(QIODevice::ReadOnly))
            m_serial = file.readAll().trimmed();
    }

    if(QFile::exists("/proc/device-tree/toradex,product-id"))
    {
        QFile file("/proc/device-tree/toradex,product-id");
        if(file.open(QIODevice::ReadOnly))
            m_productId = file.readAll().trimmed();
    }
#endif

    if (m_coreCount <= 0)
        m_coreCount = qMax(1, QThread::idealThreadCount());

    if (m_model.isEmpty()) {
        const QString productName = QSysInfo::prettyProductName().trimmed();
        const QString architecture = QSysInfo::currentCpuArchitecture().trimmed();

        if (!productName.isEmpty() && !architecture.isEmpty())
            m_model = QStringLiteral("%1 (%2)").arg(productName, architecture);
        else if (!productName.isEmpty())
            m_model = productName;
        else if (!architecture.isEmpty())
            m_model = architecture;
        else
            m_model = QStringLiteral("N/A");
    }

    if(result)
        *result=true;

    return storage;
}
