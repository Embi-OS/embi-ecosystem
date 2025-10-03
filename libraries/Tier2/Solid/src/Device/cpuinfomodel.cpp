#include "cpuinfomodel.h"
#include "solid_log.h"

#define CPUINFO_FILE_PATH (QStringLiteral("/proc/cpuinfo"))

CpuInfoModel::CpuInfoModel(QObject* parent):
    QVariantListModel(parent)
{
    m_baseName = CPUINFO_FILE_PATH;
    m_primaryField = "key";
    m_syncable = true;
    m_selectPolicy = QVariantListModelPolicies::Manual;
    m_submitPolicy = QVariantListModelPolicies::Disabled;

    select();
}

QVariantList CpuInfoModel::selectSource(bool* result)
{
    QVariantList storage;
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
                SOLIDLOG_TRACE().noquote()<<line;

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

    if(result)
        *result=true;

    return storage;
}
