#include "backupimporter.h"
#include "fluid_log.h"

#include <QUtils>
#include <QtConcurrentRun>
#include <Axion>

#include "core/paths.h"
#include "dialogs/dialogmanager.h"

BackupImporter::BackupImporter(QObject *parent) :
    QObject(parent)
{

}

void BackupImporter::run()
{
    QVariantMap settings;
    settings["title"] = tr("Importer");
    settings["message"] = tr("Sélectionner la sauvegarde à importer");
    settings["selectionType"] = FolderTreeTypes::Dir;
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {
        run(path);
    }, Qt::QueuedConnection);
}

void BackupImporter::run(const QString& path)
{
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isDir())
    {
        exitWithError(tr("Le chemin %1 n'existe pas").arg(path));
        return;
    }
    if(fileInfo.baseName()!=Paths::applicationFileName()+"_Backup")
    {
        exitWithError(tr("Le chemin n'est pas compatible").arg(path));
        return;
    }

    QVariantMap settings;
    settings["message"] = tr("Le système va importer toutes les données!");
    settings["infos"] = tr("Le processus est irréversible");
    settings["buttonAccept"] = tr("Importer");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([this, path]() mutable {
        doRun(path);
    }, Qt::QueuedConnection);
}

bool BackupImporter::doRun(const QString& path)
{
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isDir())
    {
        exitWithError(tr("Le chemin %1 n'existe pas").arg(path));
        return false;
    }
    if(fileInfo.baseName()!=Paths::applicationFileName()+"_Backup")
    {
        exitWithError(tr("Le chemin n'est pas compatible").arg(path));
        return false;
    }

    const QString importPath = path;

    auto doImport = [importPath](const QString& path) {
        QDir dirSrc(path);
        QString folder=dirSrc.dirName();
        QString src=importPath+"/"+folder;

        QDir dir(src);
        if(!dir.exists())
        {
            FLUIDLOG_DEBUG()<<src<<"does not exist, skipping import";
            return true;
        }

        QElapsedTimer timer;
        timer.start();

        bool result = QUtils::Filesystem::copy(src, path, true);

        FLUIDLOG_DEBUG()<<src<<"imported in"<<timer.nsecsElapsed()/1000000.0<<"ms";

        return result;
    };

    QVariantMap settings;
    settings["message"] = tr("Import en cours!");
    settings["infos"] = importPath;
    DialogObject* dialog = DialogManager::Get()->showBusy(settings);

    auto future = QtConcurrent::run([doImport]() -> std::tuple<bool, QString> {
        if(!doImport(Paths::database()))
            return std::tuple<bool, QString>(false, Paths::database());
        if(!doImport(Paths::setting()))
            return std::tuple<bool, QString>(false, Paths::setting());
        if(!doImport(Paths::capture()))
            return std::tuple<bool, QString>(false, Paths::capture());
        if(!doImport(Paths::log()))
            return std::tuple<bool, QString>(false, Paths::log());
        if(!doImport(Paths::cache()))
            return std::tuple<bool, QString>(false, Paths::cache());
        return std::tuple<bool, QString>(true, QString());
    });

    future.then(this, [dialog](const std::tuple<bool, QString>& tuple) {
        dialog->hide();
        const bool result = std::get<0>(tuple);
        if(!result) {
            const QString path = std::get<1>(tuple);
            exitWithError(path);
            return;
        }

        FLUIDLOG_INFO()<<"Process ended successfully";
        AxionHelper::criticalRestart(tr("Import terminé"));
    });

    return true;
}

void BackupImporter::exitWithError(const QString& msg)
{
    QVariantMap settings;
    settings["message"] = tr("Erreur lors de de l'import");
    settings["infos"] = msg;
    DialogManager::Get()->showCritical(settings);
}

