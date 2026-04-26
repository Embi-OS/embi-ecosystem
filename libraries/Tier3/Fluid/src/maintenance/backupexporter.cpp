#include "backupexporter.h"
#include "fluid_log.h"

#include <QUtils>
#include <QtConcurrentRun>
#include <Axion>

#include "core/paths.h"
#include "dialogs/dialogmanager.h"

BackupExporter::BackupExporter(QObject *parent) :
    QObject(parent)
{

}

void BackupExporter::run()
{
    QVariantMap settings;
    settings["title"] = tr("Sauvegarder");
    settings["message"] = tr("Où voulez-vous effectuer la sauvegarde?");
    settings["selectionType"] = FolderTreeTypes::Dir;
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {
        run(path);
    }, Qt::QueuedConnection);
}

void BackupExporter::run(const QString& path)
{
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isDir())
    {
        exitWithError(tr("Le chemin %1 n'existe pas").arg(path));
        return;
    }

    QVariantMap settings;
    settings["message"] = tr("Le système va exporter toutes les données!");
    settings["infos"] = tr("Le processus ne pourra pas être arrété");
    settings["buttonAccept"] = tr("Exporter");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([this, path]() mutable {
        doRun(path);
    }, Qt::QueuedConnection);
}

bool BackupExporter::doRun(const QString& path)
{
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isDir())
    {
        exitWithError(tr("Le chemin %1 n'existe pas").arg(path));
        return false;
    }

    const QString exportPath = path+"/"+Paths::applicationFileName()+"_Backup/";

    auto doExport = [exportPath](const QString& path) {
        QDir dir(path);
        if(!dir.exists())
        {
            FLUIDLOG_DEBUG()<<path<<"does not exist, skipping export";
            return true;
        }

        const QString folder=dir.dirName();
        const QString dst=exportPath+folder;

        QElapsedTimer timer;
        timer.start();

        bool result = QUtils::Filesystem::copy(path, dst, true);

        FLUIDLOG_DEBUG()<<path<<"exported in"<<timer.nsecsElapsed()/1000000.0<<"ms";

        return result;
    };

    QVariantMap settings;
    settings["message"] = tr("Export en cours!");
    settings["infos"] = exportPath;
    DialogObject* dialog = DialogManager::Get()->showBusy(settings);

    auto future = QtConcurrent::run([doExport]() -> std::tuple<bool, QString> {
        if(!doExport(Paths::database()))
            return std::tuple<bool, QString>(false, Paths::database());
        if(!doExport(Paths::setting()))
            return std::tuple<bool, QString>(false, Paths::setting());
        if(!doExport(Paths::capture()))
            return std::tuple<bool, QString>(false, Paths::capture());
        if(!doExport(Paths::log()))
            return std::tuple<bool, QString>(false, Paths::log());
        if(!doExport(Paths::cache()))
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
        QVariantMap settings;
        settings["message"] = tr("Une sauvegarde a été effectuée");
        settings["infos"] = tr("Ejecter le périphérique avant de le retirer !")+"\n"+
                            tr("En cliquant sur le logo USB en haut à droite de la fenêtre");
        DialogManager::Get()->showMessage(settings);
    });

    return true;
}

void BackupExporter::exitWithError(const QString& msg)
{
    QVariantMap settings;
    settings["message"] = tr("Erreur lors de la sauvegarde");
    settings["infos"] = msg;
    DialogManager::Get()->showCritical(settings);
}
