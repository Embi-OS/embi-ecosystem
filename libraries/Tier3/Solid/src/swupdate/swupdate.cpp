#include "swupdate.h"

#include "axion_helpertypes.h"
#include "dialogs/snackbarloader.h"
#include "dialogs/snackbarmanager.h"
#include "dialogs/dialogloader.h"
#include "ubootsettings.h"

#define SWUPDATELOG_WARNING QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"UBOOT").warning

#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
#include <progress_ipc.h>
#include <network_ipc.h>
#include <fcntl.h>
#include <unistd.h>
#endif

SwupdateProgressMessage::SwupdateProgressMessage():
    apiVersion(),
    status(SwupdateRecoveryStatuses::Idle),
    source(SwupdateSourceTypes::Unknown),
    info(),
    downloadPercent(0),
    downloadBytes(0),
    nbSteps(0),
    currentStep(0),
    currentStepPercent(0),
    currrentImage(),
    handlerName()
{

}

QString SwupdateProgressMessage::toString() const
{
    return QStringLiteral("%1 [%2], %3%, step %4/%5  %6% (%7)")
        .arg(SwupdateRecoveryStatuses::asString(status), SwupdateSourceTypes::asString(source)).arg(downloadPercent)
        .arg(currentStep).arg(nbSteps).arg(currentStepPercent)
        .arg(info);
}

QDebug operator<<(QDebug dbg, const SwupdateProgressMessage &msg)
{
    dbg.nospace().noquote()<<"SwupdateProgressMessage("<<msg.toString()<<")";
    return dbg.space().quote();
}

Swupdate::Swupdate(QObject* parent):
    QObject(parent),
    m_progressFd(-1)
{

}

void Swupdate::init()
{
    static bool initialized = false;
    if(initialized)
        return;
    initialized = true;

    Get()->test();
    Get()->open();

    SnackbarLoader* loader = new SnackbarLoader(Swupdate::Get());
    loader->setSeverity(SnackbarSeverities::None);
    loader->setTitle(tr("Mise à jour en cours!"));
    connect(Swupdate::Get(), &Swupdate::isRunningChanged, loader, &SnackbarLoader::setActive);
    connect(Swupdate::Get(), &Swupdate::statusChanged, loader, &SnackbarLoader::setCaption);
    connect(Swupdate::Get(), &Swupdate::progressChanged, loader, &SnackbarLoader::setProgress);

    // DialogLoader* loader = new DialogLoader(Swupdate::Get());
    // loader->setType(DialogTypes::Busy);
    // loader->setSeverity(DialogSeverities::None);
    // loader->setTitle(tr("Patienter"));
    // loader->setMessage(tr("Mise à jour en cours!"));
    // connect(Swupdate::Get(), &Swupdate::isRunningChanged, loader, &DialogLoader::setActive);
    // connect(Swupdate::Get(), &Swupdate::statusChanged, loader, &DialogLoader::setInfos);
    // connect(Swupdate::Get(), &Swupdate::progressChanged, loader, &DialogLoader::setProgress);
}

void Swupdate::unInit()
{
    Get()->deleteLater();
}

bool Swupdate::available() const
{
#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    return m_progressFd>=0;
#else
    return false;
#endif
}

void Swupdate::test()
{
    if(!UBootSettings::canPrintEnv())
        return;

    const QString ustate = UBootSettings::printEnv("ustate");

    if(!UBootSettings::canSetEnv())
        return;

    if(ustate=="1")
    {
        SnackbarManager::Get()->showInfo(QVariantMap({{"title", tr("Mise à jour réussie")}, {"closable", true}}));
        UBootSettings::setEnv("ustate", "0");
    }
    else if(ustate=="3")
    {
        SnackbarManager::Get()->showCritical(QVariantMap({{"title", tr("Mise à jour échouée")}, {"closable", true}}));
    }
}

void Swupdate::open()
{
#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    int fd = progress_ipc_connect(false);
    setProgressFd(fd);

    if (fd < 0) {
        SnackbarManager::Get()->showCritical(tr("Impossible de se connecter à SWUpdate"))->setClosable(true);
        SWUPDATELOG_WARNING()<<"Swupdate: Failed to connect to SWUpdate progress IPC";
        QTimer::singleShot(500, this, &Swupdate::open);
        return;
    }
    else {
        m_socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(m_socketNotifier, &QSocketNotifier::activated, this, &Swupdate::onProgressMessage);

        connect(m_socketNotifier, &QObject::destroyed, this, [fd]() {
            close(fd);
        });
    }
#else
    SWUPDATELOG_WARNING()<<"Swupdate has not been found on this system";
#endif
}

bool Swupdate::update(const QString& file)
{
#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    return QProcess::startDetached("swupdate-client", {"-q", "-p", file});
#else
    Q_UNUSED(file)
    return false;
#endif
}

bool Swupdate::restart()
{
#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    if (m_socketNotifier) {
        m_socketNotifier->deleteLater();
        m_socketNotifier = nullptr;
    }

    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
        bool ok = (exitCode >= 0);
        if(!ok)
            SWUPDATELOG_WARNING()<<"Failed to restart swupdate.service";
        proc->deleteLater();
        open();
    }, Qt::QueuedConnection);
    proc->start("systemctl", {"restart", "--now", "swupdate.service"});
    return true;
#else
    return false;
#endif
}

void Swupdate::onProgressMessage()
{
    int fd = m_progressFd;
    if(fd < 0)
        return;

    SwupdateProgressMessage msg;

#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    struct progress_msg raw;
    int rc = progress_ipc_receive(&fd, &raw);
    if (rc <= 0) {
        SWUPDATELOG_WARNING()<<"Swupdate: Error receiving progress message";
        setProgressFd(fd);
        QTimer::singleShot(500, this, &Swupdate::open);
        return;
    }

    msg.apiVersion       = QString("%1.%2")
                         .arg((raw.apiversion >> 16) & 0xFF)
                         .arg((raw.apiversion >> 8)  & 0xFF);
    msg.status           = (SwupdateRecoveryStatus)raw.status;
    msg.source           = (SwupdateSourceType)raw.source;
    msg.info             = QString::fromUtf8(raw.info, raw.infolen);
    msg.downloadPercent  = static_cast<int>(raw.dwl_percent);
    msg.downloadBytes    = static_cast<long long>(raw.dwl_bytes);
    msg.nbSteps          = static_cast<int>(raw.nsteps);
    msg.currentStep      = static_cast<int>(raw.cur_step);
    msg.currentStepPercent = static_cast<int>(raw.cur_percent);
    msg.currrentImage    = QString::fromUtf8(raw.cur_image);
    msg.handlerName      = QString::fromUtf8(raw.hnd_name);
#endif

    // update property
    setProgressMessage(msg);

    // optionally emit log
    // qNotice().noquote()<<msg.toString();

    setIsRunning(msg.status!=SwupdateRecoveryStatuses::Idle &&
                 msg.status!=SwupdateRecoveryStatuses::Done &&
                 msg.status!=SwupdateRecoveryStatuses::Success &&
                 msg.status!=SwupdateRecoveryStatuses::Failure);

    switch (msg.status) {
    case SwupdateRecoveryStatuses::Start:
        if(msg.currentStep==0 && !msg.info.isEmpty())
        {
            setFile(msg.info);
            setStatus(QString("%1, %2").arg(SwupdateRecoveryStatuses::asString(msg.status), msg.info));
            SnackbarManager::Get()->show(QVariantMap({{"title", SwupdateRecoveryStatuses::asString(msg.status)}, {"caption", msg.info}}));
        }
        else
        {
            setStatus(QString("%1, step %2/%3")
                          .arg(SwupdateRecoveryStatuses::asString(msg.status))
                          .arg(msg.currentStep).arg(msg.nbSteps));
        }
        break;
    case SwupdateRecoveryStatuses::Run:
        if(msg.currentStep==0 && !msg.info.isEmpty())
        {
            setVersion(msg.info);
            setStatus(QString("%1, %2").arg(SwupdateRecoveryStatuses::asString(msg.status), msg.info));
            SnackbarManager::Get()->show(QVariantMap({{"title", SwupdateRecoveryStatuses::asString(msg.status)}, {"caption", msg.info}}));
        }
        else
        {
            setStatus(QString("%1, step %2/%3")
                          .arg(SwupdateRecoveryStatuses::asString(msg.status))
                          .arg(msg.currentStep).arg(msg.nbSteps));
        }
        break;
    case SwupdateRecoveryStatuses::Download:
        setProgress(msg.downloadPercent*100.0);
        setStatus(QString("%1, %2  %3%")
                      .arg(SwupdateRecoveryStatuses::asString(msg.status), ::bytes(msg.downloadBytes))
                      .arg(msg.downloadPercent));
        break;
    case SwupdateRecoveryStatuses::Progress:
        setProgress(msg.currentStepPercent*100.0);
        setStatus(QString("%1, step %2/%3  %4%")
                      .arg(SwupdateRecoveryStatuses::asString(msg.status))
                      .arg(msg.currentStep).arg(msg.nbSteps).arg(msg.currentStepPercent));
        break;
    default:
        if(msg.status==SwupdateRecoveryStatuses::Done)
        {
            AxionHelper::warningReboot(tr("Mise à jour installée"));
        }
        if(msg.status==SwupdateRecoveryStatuses::Success)
        {
            SnackbarManager::Get()->showSuccess(tr("Mise à jour réussie"));
        }
        else if(msg.status==SwupdateRecoveryStatuses::Failure)
        {
            SnackbarManager::Get()->showCritical(tr("Mise à jour échouée"));
        }

        resetProgress();
        resetStatus();
        resetFile();
        resetVersion();
        break;
    }
}
