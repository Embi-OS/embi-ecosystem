#include "applicationcontroller.h"
#include "solid_log.h"

#include <QUtils>
#include <QtConcurrentRun>
#include <QDeferred>
#include <Axion>

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
    m_hasAppController(false)
{
    QString program="which";
    QStringList arguments = QStringList()<<APPCONTROLLER_CMD;

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();

    process.waitForFinished(1000);

    QString processOutput = process.readAllStandardOutput();

    if(processOutput.contains(APPCONTROLLER_CMD))
        m_hasAppController = true;
}

void ApplicationController::init(bool makeDefault)
{
    const QString newVersionLocalFile = Paths::local(Paths::applicationName()+"_new");
    const QString oldVersionLocalFile = Paths::local(Paths::applicationName()+"_old");
    if(QFile::exists(newVersionLocalFile))
    {
        QUtils::Filesystem::remove(newVersionLocalFile);
    }
    if(QFile::exists(oldVersionLocalFile))
    {
        QUtils::Filesystem::remove(oldVersionLocalFile);
    }

#ifdef Q_OS_BOOT2QT
    if(makeDefault) {
        QString localFile = Paths::local(Paths::applicationName());
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
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isFile())
    {
        exitWithError(tr("Le fichier %1 n'existe pas").arg(path));
        return;
    }
    if(fileInfo.fileName()!=Paths::applicationName())
    {
        exitWithError(tr("La mise à jour n'est pas compatible").arg(path));
        return;
    }

    QString localFile = Paths::local(Paths::applicationName());
    QString newVersionLocalFile = Paths::local(Paths::applicationName()+"_new");

    auto importStep = [path, newVersionLocalFile]() {
        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Chargement de la mise à jour!"));

        QDefer defer;
        auto future = QtConcurrent::run([path, newVersionLocalFile](){
            return QUtils::Filesystem::copy(path, newVersionLocalFile, true);
        });

        future.then([defer](bool result) mutable {
            defer.end(result);
        });

        defer.complete([dialog](bool){ dialog->hide(); });

        return defer;
    };

    auto controlStep = [newVersionLocalFile, localFile]() {
        QDeferred<QString> defer;

        QFile newVersion(newVersionLocalFile);
        QFile oldVersion(localFile);

        if(!(newVersion.exists() && oldVersion.exists()))
        {
            SOLIDLOG_WARNING()<<"newVersion or oldVersion does not exits";
            defer.reject(tr("Il semble manquer un fichier"));
        }
        else if(!newVersion.setPermissions(QFileDevice::Permission(0x0755)))
        {
            SOLIDLOG_WARNING()<<"Can't set permissions to newVersion";
            defer.reject(tr("Impossible de lire le fichier de mise à jour"));
        }
        else if(!oldVersion.rename(localFile+"_old"))
        {
            SOLIDLOG_WARNING()<<"Can't rename oldVersion to"<<localFile+"_old";
            defer.reject(tr("Impossible de remplacer la version actuelle"));
        }
        else if(!newVersion.rename(localFile))
        {
            SOLIDLOG_WARNING()<<"Can't rename newVersion to"<<localFile;
            defer.reject(tr("Impossible d'importer la nouvelle version"));
        }
        else
        {
            defer.resolve("");
        }

        return defer;
    };

    auto endStep = []() {
        AxionHelper::criticalRestart(tr("Import terminé"), tr("Ne mettez pas le système hors tension"));
    };

    QVariantMap settings;
    settings["message"] = tr("Etes-vous certain de vouloir effectuer une mise à jour?");
    settings["infos"] = tr("Le processus est irréversible");
    settings["buttonAccept"] = tr("Mettre à jour");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([importStep, controlStep, endStep]() mutable {
        importStep()
        .fail([](){ exitWithError(tr("Erreur lors de l'import de la mise à jour")); })
        .then<QString>([controlStep](){ return controlStep(); })
        .fail([](const QString& message){ exitWithError(message); })
        .done([endStep](const QString&){ endStep(); });
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
    QDeferred<int, QString, QString> defer;

    DialogObject* dialog = DialogManager::Get()->showBusy(tr("Installation en cours!"));

    auto future = QtConcurrent::run([defer, path]() mutable {

        const QFileInfo fileInfo = QFileInfo(path);
        if(fileInfo.fileName()==Paths::applicationName())
        {
            const QString infos = tr("Impossible de réinstaller l'applicaiton actuelle");
            const QString traces = tr("Il est préférable d'utiliser la fonction de mise à jour");
            defer.reject(0, infos, traces);
            return;
        }

        const QFileInfo currentpath = QFileInfo(QCoreApplication::applicationFilePath());
        QDir installDir = QDir(currentpath.absolutePath());
        installDir.cdUp();

        QString applicationName = fileInfo.baseName();
        static QRegularExpression pattern("^A\\d{2}_.+");
        if (applicationName.contains(pattern)) {
            applicationName.removeFirst();
        }
        installDir.mkpath(applicationName);
        installDir.cd(applicationName);

        const QString installFilePath = installDir.absoluteFilePath(fileInfo.fileName());
        bool result = QUtils::Filesystem::copy(path, installFilePath, true);
        if(!result)
        {
            defer.reject(0, tr("Erreur lors de l'import de l'application"), QString());
            return;
        }

        QFile installFile = QFile(installFilePath);
        if(!installFile.setPermissions(QFileDevice::Permission(0x0755)))
        {
            defer.reject(0, tr("Impossible de changer les permissions de l'application"), QString());
            return;
        }

        defer.resolve(0, tr("Application %1 installé avec succès").arg(fileInfo.fileName()), installFilePath);

    });

    defer.complete([this, dialog](bool result, int, const QString& infos, const QString& traces) {
        dialog->hide();

        QVariantMap settings;
        settings["title"] = result ? tr("Installation terminée") : tr("Erreur lors de l'installation");
        settings["message"] = result ? tr("Voulez-vous lancer l'application?") : "";
        settings["infos"] = infos;
        settings["traces"] = traces;
        settings["severity"] = result ? DialogSeverities::Message : DialogSeverities::Error;
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
    QFileInfo fileInfo(path);
    if (fileInfo.isExecutable())
    {
        SOLIDLOG_INFO()<<"Launching application"<<path;
        QMetaObject::invokeMethod(qApp, [path](){
            qApp->quit();
            QProcess::startDetached("systemd-run", {APPCONTROLLER_CMD, path});
        }, Qt::QueuedConnection);
    }
    else
    {
        SOLIDLOG_WARNING() << "Path does not have execution permission:" << path;
        emit this->errorOccurred(QString("Path does not have execution permission: %1").arg(path));
    }
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
}

void ApplicationController::removeDefault()
{
    SOLIDLOG_INFO()<<"Removing default";
    QProcess *proc = new QProcess(this);
    connect(proc, &QProcess::finished, this, [this, proc](int exitCode, QProcess::ExitStatus exitStatus) {
        qTrace()<<exitCode<<exitStatus;
        if(exitCode != 0)
            emit this->errorOccurred(proc->readAllStandardError());
        emit this->currentDefaultChanged();
        proc->deleteLater();
    });
    proc->start(APPCONTROLLER_CMD, {"--remove-default"});
    SOLIDLOG_INFO()<<"Removing default"<<proc;
}
