#include "systemctlunitcontroller.h"
#include "solid_log.h"

SystemCtlUnitController::SystemCtlUnitController(QObject *parent) :
    QObject(parent)
{
    connect(this, &SystemCtlUnitController::unitChanged, this, &SystemCtlUnitController::refreshStatus);
}

void SystemCtlUnitController::refreshStatus()
{
    checkUnit();
    checkStatus();
}

void SystemCtlUnitController::enable()
{
    if(m_unit.isEmpty())
        return;

    setProcessing(true);
    setLoaded(true);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->enableFinished(ok, ok ? QStringLiteral("Unit enabled") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"enable", "--now", m_unit});
}

void SystemCtlUnitController::disable()
{
    if(m_unit.isEmpty())
        return;

    setProcessing(true);
    setLoaded(false);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->disableFinished(ok, ok ? QStringLiteral("Unit disabled") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"disable", "--now", m_unit});
}

void SystemCtlUnitController::start()
{
    if(m_unit.isEmpty())
        return;

    setProcessing(true);
    setActive(true);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->enableFinished(ok, ok ? QStringLiteral("Unit started") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"start", "--now", m_unit});
}

void SystemCtlUnitController::stop()
{
    if(m_unit.isEmpty())
        return;

    setProcessing(true);
    setActive(false);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->disableFinished(ok, ok ? QStringLiteral("Unit stoped") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"stop", "--now", m_unit});
}

void SystemCtlUnitController::restart()
{
    if(m_unit.isEmpty())
        return;

    setProcessing(true);
    setActive(true);

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        emit this->enableFinished(ok, ok ? QStringLiteral("Unit restarted") : proc->readAllStandardError());
        proc->deleteLater();
        checkStatus();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"restart", "--now", m_unit});
}

QStringList SystemCtlUnitController::units(const QString& pattern)
{
    QStringList result;

    QProcess proc;
    if(pattern.isEmpty())
        proc.start("systemctl", {"list-unit-files", "--no-pager"});
    else
        proc.start("bash", {"-c", QString("systemctl list-unit-files --no-pager | grep -E \'%1\'").arg(pattern)});
    proc.waitForFinished();
    int exitCode = proc.exitCode();
    bool ok = (exitCode >= 0);
    const QString output = ok ? proc.readAllStandardOutput() : proc.readAllStandardError();

    if (ok) {
        static QRegularExpression regExp("\\s+");
        const QStringList lines = output.split('\n');
        for (const QString& line: lines) {
            const QStringList parts = line.trimmed().split(regExp);
            const QString part = parts.first();
            if (!part.isEmpty() && part.contains('.'))
                result.append(part);
        }
    }

    return result;
}

void SystemCtlUnitController::checkUnit()
{
    if(m_unit.isEmpty()) {
        setUnitExists(false);
        return;
    }

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        QString output = ok ? proc->readAllStandardOutput() : proc->readAllStandardError();
        bool exists = output.contains(m_unit);
        setUnitExists(exists);
        proc->deleteLater();
    });
    proc->start("systemctl", {"list-unit-files", "--no-pager"});
}

void SystemCtlUnitController::checkStatus()
{
    if(m_unit.isEmpty()) {
        setLoaded(false);
        setActive(false);
        setProcessing(false);
        return;
    }

    setProcessing(true);

    // Query systemctl status for m_unit
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int, QProcess::ExitStatus) {
        const QString output =proc->readAllStandardOutput();
        const QString error =proc->readAllStandardError();
        parseStatusOutput(output.isEmpty() ? error : output);
        proc->deleteLater();
    });
    proc->start("systemctl", {"status", m_unit, "--no-pager", "--line=0"});
}

void SystemCtlUnitController::parseStatusOutput(const QString &output)
{
    setStatus(output);

    // Example output to parse:
    // ● <unit> - ...
    //    Loaded: loaded (/lib/systemd/system/<unit>; enabled; vendor preset: enabled)
    //    Active: active
    //    ...
    bool loaded = output.contains("Loaded: loaded");
    bool enabled = output.contains("; enabled");
    bool active = output.contains("Active: active");

    setLoaded(loaded && enabled);
    setActive(active);
    setProcessing(false);
}
