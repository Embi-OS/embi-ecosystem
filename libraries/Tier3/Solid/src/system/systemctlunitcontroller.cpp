#include "systemctlunitcontroller.h"
#include "solid_log.h"

#include <QStandardPaths>
#include <QProcess>

SystemCtlUnitController::SystemCtlUnitController(QObject *parent) :
    QObject(parent)
{
    connect(this, &SystemCtlUnitController::unitChanged, this, &SystemCtlUnitController::refreshStatus);
}

bool SystemCtlUnitController::available()
{
    static bool available = !QStandardPaths::findExecutable("systemctl").isEmpty();
    return available;
}

void SystemCtlUnitController::refreshStatus()
{
    checkUnit();
    checkStatus();
}

void SystemCtlUnitController::enable()
{
    if(m_unit.isEmpty() || !available())
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
    if(m_unit.isEmpty() || !available())
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
    if(m_unit.isEmpty() || !available())
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
    if(m_unit.isEmpty() || !available())
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
    if(m_unit.isEmpty() || !available())
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
    if(!available())
        return QStringList();

    QProcess proc;
    proc.start("systemctl", {"list-unit-files", "--no-pager"});
    if (!proc.waitForFinished())
        return QStringList();

    return parseUnitList(QString::fromUtf8(proc.readAllStandardOutput()), pattern);
}

QStringList SystemCtlUnitController::parseUnitList(const QString &output, const QString &pattern)
{

    QRegularExpression patternRegExp;
    const bool usePattern = !pattern.trimmed().isEmpty();
    if (usePattern) {
        patternRegExp.setPattern(pattern);
        if (!patternRegExp.isValid()) {
            SOLIDLOG_WARNING() << "Invalid unit filter pattern:" << pattern
                               << patternRegExp.errorString();
            return QStringList();
        }
    }

    static const QRegularExpression separator(QStringLiteral("\\s+"));
    const QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    QStringList result;
    for (const QString &rawLine : lines) {
        const QString line = rawLine.trimmed();
        if (line.isEmpty())
            continue;
        if (line.startsWith(QStringLiteral("UNIT FILE")))
            continue;
        if (line.startsWith(QStringLiteral("0 unit files listed.")))
            continue;

        const QStringList parts = line.split(separator, Qt::SkipEmptyParts);
        if (parts.isEmpty())
            continue;

        const QString unitName = parts.constFirst();
        if (!unitName.contains('.'))
            continue;

        if (usePattern && !patternRegExp.match(unitName).hasMatch())
            continue;

        result.append(unitName);
    }

    result.removeDuplicates();
    return result;
}

void SystemCtlUnitController::checkUnit()
{
    if(m_unit.isEmpty() || !available()) {
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
    if(m_unit.isEmpty() || !available()) {
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
