#include "system_helpertypes.h"

#include <Axion>

#include "Timedate/timedatesettings.h"

#include <QStandardPaths>
#include <QtConcurrentRun>

SystemHelper::SystemHelper(QObject *parent) :
    QObject(parent)
{

}

void SystemHelper::showTimedateCtl()
{
    QVariantMap settings;
    settings["message"] = "timedatectl";
    settings["infos"] = TimedateSettings::timedateCtl();
    settings["diagnose"] = false;
    DialogManager::Get()->showText(settings);
}

void SystemHelper::clearLogs()
{
    QVariantMap settings;
    settings["message"] = tr("Le système va nettoyer tous les fichiers logs!");
    settings["infos"] = tr("Le processus ne pourra pas être arrété");
    settings["buttonAccept"] = tr("Nettoyer");
    settings["buttonReject"] = tr("Annuler");
    settings["diagnose"] = false;
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([this](){

        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Suppression des logs en cours!"));
        auto future = QtConcurrent::run([](){
            bool result = true;
            result &= Log::Get()->clear();
            return result;
        });

        future.then(this, [dialog](bool result) {
            dialog->hide();
            if(!result)
                DialogManager::Get()->showError(tr("Erreur lors de la suppression"));
            else
                AxionHelper::criticalRestart(tr("Suppression terminée"));
        });
    });
}

void SystemHelper::exportLogs()
{
    QVariantMap settings;
    settings["message"] = tr("Ou voullez-vous exporter les logs ?");
    settings["selectionType"] = FolderTreeTypes::Dir;
    settings["buttonAccept"] = tr("Exporter");
    settings["buttonReject"] = tr("Annuler");
    DialogObject* dialog = DialogManager::Get()->showFileTree(settings);
    dialog->onPathSelected([this](const QString& path) mutable {

        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Suppression des logs en cours!"));
        auto future = QtConcurrent::run([path](){
            bool result = true;
            result &= Log::Get()->copy(path);
            return result;
        });

        future.then(this, [dialog](bool result) {
            dialog->hide();
            if(!result)
                DialogManager::Get()->showError(tr("Erreur lors de l'export"));
        });
    });
}

void SystemHelper::clearCache()
{
    QVariantMap settings;
    settings["message"] = tr("Le système va nettoyer tous les fichiers temporaires!");
    settings["infos"] = tr("Le processus ne pourra pas être arrété");
    settings["traces"] = tr("L'application va ensuite redémarrer !");
    settings["buttonAccept"] = tr("Nettoyer");
    settings["buttonReject"] = tr("Annuler");
    settings["diagnose"] = false;
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([this](){

        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Suppression des fichiers temporaires en cours!"));
        auto future = QtConcurrent::run([](){
            bool result = true;
            result &= QUtils::Filesystem::clear(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
            result &= QUtils::Filesystem::clear(Paths::cache());
            return result;
        });

        future.then(this, [dialog](bool result) {
            dialog->hide();
            if(!result)
                DialogManager::Get()->showError(tr("Erreur lors de la suppression"));
            else
                AxionHelper::criticalRestart(tr("Suppression terminée"));
        });
    });
}

void SystemHelper::clearSettings()
{
    QVariantMap settings;
    settings["message"] = tr("Le système va nettoyer tous les réglages!");
    settings["infos"] = tr("Le processus ne pourra pas être arrété");
    settings["traces"] = tr("L'application va ensuite redémarrer !");
    settings["buttonAccept"] = tr("Nettoyer");
    settings["buttonReject"] = tr("Annuler");
    settings["diagnose"] = false;
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([this](){

        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Suppression des réglages en cours!"));
        auto future = QtConcurrent::run([](){
            bool result = true;
            result &= QUtils::Filesystem::clear(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
            result &= QUtils::Filesystem::clear(Paths::setting());
            return result;
        });

        future.then(this, [dialog](bool result) {
            dialog->hide();
            if(!result)
                DialogManager::Get()->showError(tr("Erreur lors de la suppression"));
            else
                AxionHelper::criticalRestart(tr("Suppression terminée"));
        });
    });
}

void SystemHelper::clearAll()
{
    QVariantMap settings;
    settings["message"] = tr("Le système va tout remettre à zéro!");
    settings["infos"] = tr("Le processus ne pourra pas être arrété");
    settings["traces"] = tr("L'application va ensuite redémarrer !");
    settings["buttonAccept"] = tr("Nettoyer");
    settings["buttonReject"] = tr("Annuler");
    settings["diagnose"] = false;
    DialogObject* dialog = DialogManager::Get()->showMessage(settings);
    dialog->onAccepted([this](){

        DialogObject* dialog = DialogManager::Get()->showBusy(tr("Suppression des réglages en cours!"));
        auto future = QtConcurrent::run([](){
            bool result = true;
            QUtils::Filesystem::clear(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
            QUtils::Filesystem::clear(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
            result &= QUtils::Filesystem::clear(Paths::setting());
            result &= QUtils::Filesystem::clear(Paths::database());
            result &= QUtils::Filesystem::clear(Paths::log());
            result &= QUtils::Filesystem::clear(Paths::cache());
            return result;
        });

        future.then(this, [dialog](bool result) {
            dialog->hide();
            if(!result)
                DialogManager::Get()->showError(tr("Erreur lors de la suppression"));
            else
                AxionHelper::criticalRestart(tr("Suppression terminée"));
        });
    });
}
