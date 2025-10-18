#include "backupimporter.h"
#include "solid_log.h"

#include <QUtils>
#include <QtConcurrentRun>
#include <QDeferred>
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
    m_importPath = path;
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

    auto importStep = [this](const QString& path) {
        QVariantMap settings;
        settings["message"] = tr("Import en cours!");
        settings["infos"] = path;
        DialogObject* dialog = DialogManager::Get()->showBusy(settings);

        QDefer defer;
        QDir dirSrc(path);
        QString folder=dirSrc.dirName();
        QString src=m_importPath+"/"+folder;

        QDir dir(src);
        if(!dir.exists())
        {
            SOLIDLOG_DEBUG()<<src<<"does not exist";
            defer.resolve();
            return defer;
        }

        auto future = QtConcurrent::run([src, path](){
            return QUtils::Filesystem::copy(src, path, true);
        });

        future.then(this, [defer](bool result) mutable {
            defer.end(result);
        });

        defer.complete([dialog](bool){ dialog->hide(); });

        return defer;
    };

    auto endStep = []() {
        AxionHelper::criticalRestart(tr("Import terminé"));
    };

    QVariantMap settings;
    settings["message"] = tr("Le système va importer toutes les données!");
    settings["infos"] = tr("Le processus est irréversible");
    settings["buttonAccept"] = tr("Importer");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([importStep, endStep]() mutable {
        importStep(Paths::log())
        .fail([](){ exitWithError(Paths::log()); })
        .then<>([importStep](){ return importStep(Paths::capture()); })
        .fail([](){ exitWithError(Paths::capture()); })
        .then<>([importStep](){ return importStep(Paths::setting()); })
        .fail([](){ exitWithError(Paths::setting()); })
        .then<>([importStep](){ return importStep(Paths::database()); })
        .fail([](){ exitWithError(Paths::database()); })
        .then<>([importStep](){ return importStep(Paths::cache()); })
        .fail([](){ exitWithError(Paths::cache()); })
        .done([endStep](){ endStep(); });
    }, Qt::QueuedConnection);
}

void BackupImporter::exitWithError(const QString& msg)
{
    QVariantMap settings;
    settings["message"] = tr("Erreur lors de de l'import");
    settings["infos"] = msg;
    DialogManager::Get()->showError(settings);
}

