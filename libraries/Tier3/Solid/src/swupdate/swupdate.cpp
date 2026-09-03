#include "swupdate.h"

#include "axion_helpertypes.h"
#include "dialogs/snackbarloader.h"
#include "dialogs/snackbarmanager.h"
#include "ubootsettings.h"

#include <QFileInfo>
#include <QProcess>
#include <QTimer>

#define SWUPDATELOG_WARNING QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"SWUPDATE").warning

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
    m_progressFd(-1),
    m_reconnectTimer(this)
{
    m_reconnectTimer.setInterval(500);
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &Swupdate::open);

}

Swupdate::~Swupdate()
{
    closeProgressChannel(true);

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
    if(m_progressFd>=0 && m_socketNotifier)
        return;

    closeProgressChannel(true);

    int fd = progress_ipc_connect(false);

    if (fd < 0) {
        if(!m_progressConnectionUnavailable)
            SnackbarManager::Get()->showCritical(tr("Impossible de se connecter à SWUpdate"))->setClosable(true);
        m_progressConnectionUnavailable = true;
        SWUPDATELOG_WARNING()<<"Swupdate: Failed to connect to SWUpdate progress IPC";
        scheduleOpen();
        return;
    }

    m_progressConnectionUnavailable = false;
    setProgressChannelFd(fd);
    m_socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_socketNotifier, &QSocketNotifier::activated, this, &Swupdate::onProgressMessage);
#else
    SWUPDATELOG_WARNING()<<"Swupdate has not been found on this system";
#endif
}

bool Swupdate::update(const QString& file)
{
#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    const QFileInfo updateFile(file);
    if(!updateFile.isFile() || !updateFile.isReadable()) {
        const QString message = tr("Fichier de mise à jour inaccessible");
        setStatus(message);
        SnackbarManager::Get()->showCritical(message)->setClosable(true);
        SWUPDATELOG_WARNING()<<message<<file;
        return false;
    }

    if(m_updateProcess) {
        const QString message = tr("Une mise à jour est déjà en cours");
        SnackbarManager::Get()->showCritical(message)->setClosable(true);
        return false;
    }

    QProcess* process = new QProcess(this);
    m_updateProcess = process;

    connect(process, &QProcess::errorOccurred, this, [this, process](QProcess::ProcessError error) {
        if(error != QProcess::FailedToStart || m_updateProcess != process)
            return;

        const QString message = tr("Impossible de démarrer swupdate-client");
        setStatus(message);
        SnackbarManager::Get()->showCritical(message)->setClosable(true);
        SWUPDATELOG_WARNING()<<message<<process->errorString();
        m_updateProcess = nullptr;
        process->deleteLater();
    });

    connect(process, &QProcess::finished, this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if(m_updateProcess != process)
            return;

        m_updateProcess = nullptr;
        if(exitStatus != QProcess::NormalExit || exitCode != 0) {
            QString message = QString::fromUtf8(process->readAllStandardError()).trimmed();
            if(message.isEmpty())
                message = tr("swupdate-client a échoué");
            setStatus(message);
            SnackbarManager::Get()->showCritical(message)->setClosable(true);
            SWUPDATELOG_WARNING()<<message;
        }
        process->deleteLater();
    });

    process->start("swupdate-client", {"-q", "-p", updateFile.absoluteFilePath()});
    return true;
#else
    Q_UNUSED(file)
    return false;
#endif
}

bool Swupdate::restart()
{
#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    if(m_restartProcess) {
        const QString message = tr("Le redémarrage de SWUpdate est déjà en cours");
        SnackbarManager::Get()->showCritical(message)->setClosable(true);
        return false;
    }

    QProcess* process = new QProcess(this);
    m_restartProcess = process;

    connect(process, &QProcess::errorOccurred, this, [this, process](QProcess::ProcessError error) {
        if(error != QProcess::FailedToStart || m_restartProcess != process)
            return;

        const QString message = tr("Impossible de démarrer systemctl");
        setStatus(message);
        SnackbarManager::Get()->showCritical(message)->setClosable(true);
        SWUPDATELOG_WARNING()<<message<<process->errorString();
        m_restartProcess = nullptr;
        process->deleteLater();
    });

    connect(process, &QProcess::finished, this, [this, process](int exitCode, QProcess::ExitStatus exitStatus) {
        if(m_restartProcess != process)
            return;

        m_restartProcess = nullptr;
        if(exitStatus != QProcess::NormalExit || exitCode != 0) {
            QString message = QString::fromUtf8(process->readAllStandardError()).trimmed();
            if(message.isEmpty())
                message = tr("Le redémarrage de SWUpdate a échoué");
            setStatus(message);
            SnackbarManager::Get()->showCritical(message)->setClosable(true);
            SWUPDATELOG_WARNING()<<message;
        }
        else {
            closeProgressChannel(true);
            open();
        }
        process->deleteLater();
    });

    process->start("systemctl", {"restart", "swupdate.service"});
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
        if(m_socketNotifier) {
            m_socketNotifier->setEnabled(false);
            m_socketNotifier->deleteLater();
            m_socketNotifier = nullptr;
        }
        setProgressChannelFd(fd);
        scheduleOpen();
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
            setStatus(QString("%1, step %2/%3").arg(SwupdateRecoveryStatuses::asString(msg.status)).arg(msg.currentStep).arg(msg.nbSteps));
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
            setStatus(QString("%1, step %2/%3").arg(SwupdateRecoveryStatuses::asString(msg.status)).arg(msg.currentStep).arg(msg.nbSteps));
        }
        break;
    case SwupdateRecoveryStatuses::Download:
        setProgress(msg.downloadPercent*100.0);
        setStatus(QString("%1, %2  %3%").arg(SwupdateRecoveryStatuses::asString(msg.status), ::bytes(msg.downloadBytes)).arg(msg.downloadPercent));
        break;
    case SwupdateRecoveryStatuses::Progress:
        setProgress(msg.currentStepPercent*100.0);
        setStatus(QString("%1, step %2/%3  %4%").arg(SwupdateRecoveryStatuses::asString(msg.status)).arg(msg.currentStep).arg(msg.nbSteps).arg(msg.currentStepPercent));
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

void Swupdate::closeProgressChannel(bool closeFd)
{
    if(m_socketNotifier) {
        m_socketNotifier->setEnabled(false);
        m_socketNotifier->deleteLater();
        m_socketNotifier = nullptr;
    }

#if defined(SWUPDATE_FOUND) && defined(Q_OS_LINUX)
    if(closeFd && m_progressFd>=0)
        close(m_progressFd);
#else
    Q_UNUSED(closeFd)
#endif

    setProgressChannelFd(-1);
}

void Swupdate::scheduleOpen()
{
    if(!m_reconnectTimer.isActive())
        m_reconnectTimer.start();
}

void Swupdate::setProgressChannelFd(int fd)
{
    const bool wasAvailable = available();
    setProgressFd(fd);
    if(wasAvailable != available())
        emit availableChanged();
}
