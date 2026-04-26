#include "timedatectlunitcontroller.h"
#include "solid_log.h"

#include <QStandardPaths>
#include <QProcess>

TimedateCtlUnitController::TimedateCtlUnitController(QObject *parent) :
    QObject(parent)
{

}

bool TimedateCtlUnitController::available()
{
    static bool available = !QStandardPaths::findExecutable("timedatectl").isEmpty();
    return available;
}

void TimedateCtlUnitController::refreshStatus()
{
    setStatus(timedateCtl());
}

void TimedateCtlUnitController::refreshTimesyncStatus()
{
    const QStringList arguments = QStringList()<<"timesync-status";
    setTimesyncStatus(timedateCtl(arguments));
}

void TimedateCtlUnitController::refreshTimesyncProperties()
{
    const QStringList arguments = QStringList()<<"show-timesync";
    setTimesyncProperties(timedateCtl(arguments));
}

QString TimedateCtlUnitController::timedateCtl(const QStringList& arguments)
{
    const QString program = "timedatectl";

    if(!available())
        return QString("%1 unavailable").arg(program);

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();

    process.waitForFinished(1000);

    QString processOutput = process.readAllStandardOutput();
    QString processError = process.readAllStandardError();

    if(!processError.isEmpty())
        return processError;

    QStringList ret;
    const QStringList processOutputs = processOutput.split("\n", Qt::SkipEmptyParts);
    for(const QString& output: processOutputs)
        ret.append(output.trimmed());

    return ret.join("\n");
}
