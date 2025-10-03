#include "backupexporter.h"
#include "solid_log.h"

#include <QUtils>
#include <QtConcurrentRun>
#include <QDeferred>
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
    m_exportPath = path;
    QFileInfo fileInfo = QFileInfo(path);
    if(!fileInfo.exists() || !fileInfo.isDir())
    {
        exitWithError(tr("Le chemin %1 n'existe pas").arg(path));
        return;
    }

    auto exportStep = [this](const QString& path) {
        QVariantMap settings;
        settings["message"] = tr("Sauvegarde en cours!");
        settings["infos"] = path;
        DialogObject* dialog = DialogManager::Get()->showBusy(settings);

        QDefer defer;
        QDir dir(path);
        if(!dir.exists())
        {
            SOLIDLOG_WARNING()<<path<<"does not exist";
            defer.reject();
            return defer;
        }

        QString folder=dir.dirName();
        QString dst=m_exportPath+"/"+Paths::applicationName()+"_Backup/"+folder;

        auto future = QtConcurrent::run([path, dst](){
            return QUtils::Filesystem::copy(path, dst, true);
        });

        future.then(this, [defer](bool result) mutable {
            defer.end(result);
        });

        defer.complete([dialog](bool){ dialog->hide(); });

        return defer;
    };

    auto endStep = []() {
        SOLIDLOG_INFO()<<"Process ended successfully";
        QVariantMap settings;
        settings["message"] = tr("Une sauvegarde a été effectuée");
        settings["infos"] = tr("Ejecter le périphérique avant de le retirer !")+"\n"+
                            tr("En cliquant sur le logo USB en haut à droite de la fenêtre");
        DialogManager::Get()->showMessage(settings);
    };

    QVariantMap settings;
    settings["message"] = tr("Le système va exporter toutes les données!");
    settings["infos"] = tr("Le processus ne pourra pas être arrété");
    settings["buttonAccept"] = tr("Exporter");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([exportStep, endStep]() mutable {
        exportStep(Paths::database())
        .fail([](){ exitWithError(Paths::database()); })
        .then<>([exportStep](){ return exportStep(Paths::setting()); })
        .fail([](){ exitWithError(Paths::setting()); })
        .then<>([exportStep](){ return exportStep(Paths::capture()); })
        .fail([](){ exitWithError(Paths::capture()); })
        .then<>([exportStep](){ return exportStep(Paths::log()); })
        .fail([](){ exitWithError(Paths::log()); })
        .done([endStep](){ endStep(); });
    }, Qt::QueuedConnection);
}

void BackupExporter::exitWithError(const QString& msg)
{
    QVariantMap settings;
    settings["message"] = tr("Erreur lors de la sauvegarde");
    settings["infos"] = msg;
    DialogManager::Get()->showError(settings);
}
