#include "ubootsettings.h"

#include <QProcess>
#include <QStandardPaths>

#define UBOOTLOG_WARNING QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"UBOOT").warning

#define PRINTENV_CMD QStringLiteral("fw_printenv")
#define SETENV_CMD QStringLiteral("fw_setenv")
#define OVERLAYS_PATH QStringLiteral("/boot/overlays.txt")

UBootSettings::UBootSettings(QObject *parent) :
    QObject(parent)
{

}

bool UBootSettings::canPrintEnv()
{
    static bool canPrintEnv = !QStandardPaths::findExecutable(PRINTENV_CMD).isEmpty();
    return canPrintEnv;
}

bool UBootSettings::canSetEnv()
{
    static bool canSetEnv = !QStandardPaths::findExecutable(SETENV_CMD).isEmpty();
    return canSetEnv;
}

bool UBootSettings::canSetOverlays()
{
    static bool canSetOverlays = QFileInfo::exists(OVERLAYS_PATH);
    return canSetOverlays;
}

QString UBootSettings::printEnv(const QString& name)
{
    if(!canPrintEnv())
    {
        UBOOTLOG_WARNING()<<"Cannot print u-boot env"<<name;
        return QString("%1 unavailable").arg(PRINTENV_CMD);
    }

    QStringList arguments;
    if(!name.isEmpty())
        arguments<<name;

    QProcess process;
    process.setProgram(PRINTENV_CMD);
    process.setArguments(arguments);
    process.start();

    bool result = process.waitForFinished(1000);

    const QString processError = process.readAllStandardError();
    const QString processOutput = process.readAllStandardOutput();

    if(!result)
        return processError;

    if(name.isEmpty())
        return processOutput;
    return processOutput.mid(name.size()+1, processOutput.size()-(name.size()+2));
}

bool UBootSettings::setEnv(const QString& name, const QString& value)
{
    if(!canSetEnv())
    {
        UBOOTLOG_WARNING()<<"Cannot set u-boot env"<<name<<"with"<<value;
        return false;
    }

    QStringList arguments = QStringList()<<name;
    if(!value.isEmpty())
        arguments<<value;

    QProcess process;
    process.setProgram(SETENV_CMD);
    process.setArguments(arguments);
    process.start();

    return process.waitForFinished(1000);
}

bool UBootSettings::clearEnv(const QString& name)
{
    if(!canSetEnv())
    {
        UBOOTLOG_WARNING()<<"Cannot clear u-boot env"<<name;
        return false;
    }

    return setEnv(name, "");
}

QStringList UBootSettings::readOverlays()
{
    if(!canSetOverlays())
    {
        UBOOTLOG_WARNING()<<"Cannot set overlays";
        return QStringList();
    }

    QStringList overlays;

    QFile file(OVERLAYS_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        UBOOTLOG_WARNING()<<"Cannot open"<<OVERLAYS_PATH;
        return overlays;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith("fdt_overlays")) {
            // Remove the "fdt_overlays=" prefix
            QString fdt_overlays = line.split("=", Qt::SkipEmptyParts).last();
            // Split on spaces
            static QRegularExpression regExp = QRegularExpression("\\s+");
            overlays = fdt_overlays.split(regExp, Qt::SkipEmptyParts);
            break; // done, we only care about the first occurrence
        }
    }

    return overlays;
}

bool UBootSettings::writeOverlays(const QStringList& overlays)
{
    if(!canSetOverlays())
    {
        UBOOTLOG_WARNING()<<"Cannot set overlays";
        return false;
    }

    QFile file(OVERLAYS_PATH);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        UBOOTLOG_WARNING()<<"Cannot open"<<OVERLAYS_PATH;
        return false;
    }

    QTextStream out(&file);
    out << "fdt_overlays=" << overlays.join(' ');
    return true;
}

UBootEnvModel::UBootEnvModel(QObject *parent) :
    QVariantListModel(parent)
{
    m_baseName = "U-Boot";
    m_primaryField = "key";
    m_syncable = true;
    m_selectPolicy = QVariantListModelPolicies::Manual;
    m_submitPolicy = QVariantListModelPolicies::Disabled;

    select();
}

bool UBootEnvModel::doSelect()
{
    if(!UBootSettings::canPrintEnv())
    {
        emitSelectDone(false);
        return false;
    }

    QVariantList storage = selectEnvs();
    setStorage(std::move(storage));
    emitSelectDone(true);

    return true;
}

QVariantList UBootEnvModel::selectEnvs()
{
    QStringList envs = UBootSettings::printEnv().split("\n", Qt::SkipEmptyParts);
    envs.sort();

    QVariantList storage;
    storage.reserve(envs.size());
    for(const QString& env: std::as_const(envs))
    {
        QStringList lineSplit = env.split("=");
        QVariantMap map;
        map["key"] = lineSplit.takeFirst();
        map["value"] = lineSplit.join("=");
        storage.append(map);
    }

    return storage;
}