#include "applicationcontroller.h"
#include "solid_log.h"

#include <QUtils>
#include <QtConcurrentRun>
#include <Axion>
#include <QStandardPaths>

#include "core/paths.h"
#include "dialogs/dialogmanager.h"

#ifndef APPCONTROLLER_CMD
#define APPCONTROLLER_CMD "appcontroller"
#endif

#ifndef B2QT_PREFIX
#define B2QT_PREFIX "/usr/bin/b2qt"
#endif

ApplicationController::ApplicationController(QObject *parent) :
    QObject(parent),
#ifdef Q_OS_BOOT2QT
    m_hasAppController(!QStandardPaths::findExecutable(APPCONTROLLER_CMD).isEmpty())
#else
    m_hasAppController(false)
#endif
{

}

void ApplicationController::init(bool makeDefault)
{
    const QString newVersionLocalFile = Paths::applicationFilePath()+"_new";
    const QString oldVersionLocalFile = Paths::applicationFilePath()+"_old";
    if(QFile::exists(newVersionLocalFile))
    {
        QUtils::Filesystem::remove(newVersionLocalFile);
    }
    if(QFile::exists(oldVersionLocalFile))
    {
        QUtils::Filesystem::remove(oldVersionLocalFile);
    }

#ifdef Q_OS_BOOT2QT
    const QString applicationDirPath = QCoreApplication::applicationDirPath();
    const QStorageInfo storageInfo(applicationDirPath);
    if(makeDefault && storageInfo.isRoot()) {
        QString localFile = Paths::applicationFilePath();
        SOLIDLOG_INFO()<<"Making default"<<localFile;
        QProcess::startDetached(APPCONTROLLER_CMD, {"--make-default", localFile});
    }
#endif
}

bool ApplicationController::hasAppController()
{
    return m_hasAppController;
}

QString ApplicationController::currentDefault() const
{
    if (!m_hasAppController)
        return QString("Application controller unavailable on this platform");

    QFileInfo fileInfo = QFileInfo(B2QT_PREFIX);
    if(!fileInfo.exists())
        return QString("%1 does not exists").arg(B2QT_PREFIX);
    if(!fileInfo.isSymLink())
        return QString("%1 is not a symlink").arg(B2QT_PREFIX);
    return fileInfo.symLinkTarget();
}

void ApplicationController::update()
{
    QVariantMap settings;
    settings["title"] = tr("Mettre à jour");
    settings["message"] = tr("Sélectionner le fichier de mise à jour");
    settings["selectionType"] = FolderTreeTypes::File;
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {
        update(path);
    }, Qt::QueuedConnection);
}

void ApplicationController::update(const QString& path)
{
    const QFileInfo fileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isFile())
    {
        exitWithError(tr("Le fichier %1 n'existe pas").arg(path));
        return;
    }
    if(fileInfo.fileName() != Paths::applicationFileName())
    {
        exitWithError(tr("La mise à jour n'est pas compatible").arg(path));
        return;
    }

    auto importStep = [this, path]() {
        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Chargement de la mise à jour!"));

        const QString localFile = Paths::applicationFilePath();
        const QString newVersionLocalFile = Paths::applicationFilePath() + "_new";

        QtConcurrent::run([path, newVersionLocalFile]() {
            return QUtils::Filesystem::copy(path, newVersionLocalFile, true);
        }).then(this, [dialog, newVersionLocalFile, localFile](bool copied) {
            dialog->hide();

            if(!copied) {
                exitWithError(tr("Erreur lors de l'import de la mise à jour"));
                return;
            }

            QFile newVersion(newVersionLocalFile);
            QFile oldVersion(localFile);

            QString errorMessage;
            if(!(newVersion.exists() && oldVersion.exists()))
            {
                SOLIDLOG_WARNING() << "newVersion or oldVersion does not exits";
                errorMessage = tr("Il semble manquer un fichier");
            }
            else if(!newVersion.setPermissions(QFileDevice::Permissions(0x0755)))
            {
                SOLIDLOG_WARNING() << "Can't set permissions to newVersion";
                errorMessage = tr("Impossible de lire le fichier de mise à jour");
            }
            else if(!oldVersion.rename(localFile + "_old"))
            {
                SOLIDLOG_WARNING() << "Can't rename oldVersion to" << localFile + "_old";
                errorMessage = tr("Impossible de remplacer la version actuelle");
            }
            else if(!newVersion.rename(localFile))
            {
                SOLIDLOG_WARNING() << "Can't rename newVersion to" << localFile;
                errorMessage = tr("Impossible d'importer la nouvelle version");
            }

            if(!errorMessage.isEmpty()) {
                exitWithError(errorMessage);
                return;
            }

            AxionHelper::criticalRestart(tr("Import terminé"), tr("Ne mettez pas le système hors tension"));
        });
    };

    QVariantMap settings;
    settings["message"] = tr("Etes-vous certain de vouloir effectuer une mise à jour?");
    settings["infos"] = tr("Le processus est irréversible");
    settings["buttonAccept"] = tr("Mettre à jour");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);

    dialog->onAccepted([importStep]() {
        importStep();
    });
}

void ApplicationController::exitWithError(const QString& msg)
{
    AxionHelper::criticalRestart(tr("La mise à jour a échoué"), msg);
}

void ApplicationController::install()
{
    QVariantMap settings;
    settings["title"] = tr("Installer");
    settings["message"] = tr("Sélectionner l'application à installer");
    settings["selectionType"] = FolderTreeTypes::File;
    settings["showRootDrives"] = true;
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {
        install(path);
    }, Qt::QueuedConnection);
}

void ApplicationController::install(const QString& path)
{
    DialogObject* dialog = DialogManager::Get()->showBusy(tr("Installation en cours!"));

    auto future = QtConcurrent::run([path]() -> std::tuple<bool, QString, QString> {

        const QFileInfo fileInfo = QFileInfo(path);
        if(fileInfo.fileName()==Paths::applicationName())
        {
            const QString infos = tr("Impossible de réinstaller l'applicaiton actuelle");
            const QString traces = tr("Il est préférable d'utiliser la fonction de mise à jour");
            return std::tuple<bool, QString, QString>(false, infos, traces);
        }

        const QString applicationDirPath = QCoreApplication::applicationDirPath();
        QDir installDir = QDir(applicationDirPath);
        if (applicationDirPath.endsWith(QCoreApplication::applicationName())) {
            installDir.cdUp();
            QString applicationName = fileInfo.baseName();
            static const QRegularExpression pattern("^A\\d{2}_.+");
            if (applicationName.contains(pattern)) {
                applicationName.removeFirst();
            }
            installDir.mkpath(applicationName);
            installDir.cd(applicationName);
        }

        const QString installFilePath = installDir.absoluteFilePath(fileInfo.fileName());
        bool result = QUtils::Filesystem::copy(path, installFilePath, true);
        if(!result)
        {
            return std::tuple<bool, QString, QString>(false, tr("Erreur lors de l'import de l'application"), QString());
        }

        QFile installFile = QFile(installFilePath);
        if(!installFile.setPermissions(QFileDevice::Permissions(0x0755)))
        {
            return std::tuple<bool, QString, QString>(false, tr("Impossible de changer les permissions de l'application"), QString());
        }

        return std::tuple<bool, QString, QString>(true, tr("Application %1 installé avec succès").arg(fileInfo.fileName()), installFilePath);
    });

    future.then(this, [this, dialog](const std::tuple<bool, QString, QString>& tuple) {
        dialog->hide();

        const bool result = std::get<0>(tuple);
        const QString infos = std::get<1>(tuple);
        const QString traces = std::get<2>(tuple);

        QVariantMap settings;
        settings["title"] = result ? tr("Installation terminée") : tr("Erreur lors de l'installation");
        settings["message"] = result ? tr("Voulez-vous lancer l'application?") : "";
        settings["infos"] = infos;
        settings["traces"] = traces;
        settings["severity"] = result ? DialogSeverities::Message : DialogSeverities::Critical;
        settings["buttonAccept"] = result ? tr("Lancer") : tr("Fermer");
        settings["buttonReject"] = result ? tr("Fermer") : "";
        settings["diagnose"] = false;
        DialogObject* action = DialogManager::Get()->showAction(settings);
        if(result) {
            action->onAccepted([this, traces](){
                launch(traces);
            });
        }
    });
}

void ApplicationController::launch()
{
    QVariantMap settings;
    settings["title"] = tr("Lancer");
    settings["message"] = tr("Sélectionner l'application à lancer");
    settings["selectionType"] = FolderTreeTypes::File;
    settings["showRootDrives"] = true;
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {
        launch(path);
    }, Qt::QueuedConnection);
}

void ApplicationController::launch(const QString& path)
{
#ifdef Q_OS_BOOT2QT
    QFileInfo fileInfo(path);
    if (fileInfo.isExecutable())
    {
        SOLIDLOG_INFO()<<"Launching application"<<path;
        QMetaObject::invokeMethod(qApp, [path](){
            qApp->quit();
            QString uid = QString::number(getuid());
            QString gid = QString::number(getgid());
            QProcess::startDetached("systemd-run", {"--system", "--uid="+uid, "--gid="+gid, APPCONTROLLER_CMD, path});
        }, Qt::QueuedConnection);
    }
    else
    {
        SOLIDLOG_WARNING() << "Path does not have execution permission:" << path;
        emit this->errorOccurred(QString("Path does not have execution permission: %1").arg(path));
    }
#else
    SOLIDLOG_WARNING() << "Platform is not able to launch:" << path;
    emit this->errorOccurred(QString("Platform is not able to launch: %1").arg(path));
#endif
}

void ApplicationController::makeDefault()
{
    QVariantMap settings;
    settings["title"] = tr("Application par défaut");
    settings["message"] = tr("Sélectionner la nouvelle application par défaut");
    settings["selectionType"] = FolderTreeTypes::File;
    settings["showRootDrives"] = true;
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {
        makeDefault(path);
    }, Qt::QueuedConnection);
}

void ApplicationController::makeDefault(const QString& path)
{
#ifdef Q_OS_BOOT2QT
    QFileInfo fileInfo(path);
    if (fileInfo.isExecutable())
    {
        SOLIDLOG_INFO()<<"Making default"<<path;
        QProcess *proc = new QProcess(this);
        connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus) {
            if(exitCode != 0)
                emit this->errorOccurred(proc->readAllStandardError());
            emit this->currentDefaultChanged();
            proc->deleteLater();
        });
        proc->start(APPCONTROLLER_CMD, {"--make-default", path});
    }
    else
    {
        SOLIDLOG_WARNING() << "Path does not have execution permission:" << path;
        emit this->errorOccurred(QString("Path does not have execution permission: %1").arg(path));
    }
#else
    SOLIDLOG_WARNING() << "Platform is not able to make default:" << path;
    emit this->errorOccurred(QString("Platform is not able to make default: %1").arg(path));
#endif
}

void ApplicationController::removeDefault()
{
#ifdef Q_OS_BOOT2QT
    SOLIDLOG_INFO()<<"Removing default";
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus exitStatus) {
        Q_UNUSED(exitStatus)
        if(exitCode != 0)
            emit this->errorOccurred(proc->readAllStandardError());
        emit this->currentDefaultChanged();
        proc->deleteLater();
    });
    proc->start(APPCONTROLLER_CMD, {"--remove-default"});
    SOLIDLOG_INFO()<<"Removing default"<<proc;
#else
    SOLIDLOG_WARNING() << "Platform is not able to remove default";
    emit this->errorOccurred(QString("Platform is not able to remove default"));
#endif
}
