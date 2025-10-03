#include "dialogmanager.h"
#include "popup_log.h"

#include "foldertreemodel.h"
#include <QFileDialog>

DialogManager::DialogManager(QObject *parent) :
    QObject(parent),
    m_children(new QObjectListModel(this, &DialogObject::staticMetaObject))
{
    m_children->onInserted<DialogObject>([this](DialogObject* object){
        this->onObjectInserted(object);
    });
}

void DialogManager::saveFileContent(const QByteArray &fileContent, const QString &fileNameHint)
{
    QFileDialog::saveFileContent(fileContent, fileNameHint);
}

void DialogManager::getOpenFileContent(const QString &nameFilter, const std::function<void(const QString&, const QByteArray&)>& fileContentReady)
{
    QFileDialog::getOpenFileContent(nameFilter,  fileContentReady);
}

DialogObject* DialogManager::show(const QVariant& settings)
{
    DialogObject* dialog = new DialogObject(settings, this);

    QMetaObject::invokeMethod(this, [this, dialog](){
        if(dialog->isMarkedForDeletion())
            return;
        m_children->prepend(dialog);
        dialog->show();
    }, Qt:: QueuedConnection);

    return dialog;
}

DialogObject* DialogManager::showError(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);
    dialog->setSeverity(DialogSeverities::Error);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Erreur"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Fermer"));

    return dialog;
}

DialogObject* DialogManager::showWarning(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);
    dialog->setSeverity(DialogSeverities::Warning);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Attention"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Fermer"));

    return dialog;
}

DialogObject* DialogManager::showMessage(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Information"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Fermer"));

    return dialog;
}

DialogObject* DialogManager::showAction(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Information"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Fermer"));

    return dialog;
}

DialogObject* DialogManager::showQuit(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Attention"));
    if(dialog->getMessage().isEmpty())
        dialog->setMessage(tr("Êtes-vous sûr de vouloir quitter la page?"));
    if(dialog->getInfos().isEmpty())
        dialog->setInfos(tr("Les modifications ne seront pas sauvegardées!"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Quitter"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showCancel(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Attention"));
    if(dialog->getMessage().isEmpty())
        dialog->setMessage(tr("Êtes-vous sûr de vouloir quitter le mode édition?"));
    if(dialog->getInfos().isEmpty())
        dialog->setInfos(tr("Les modifications ne seront pas sauvegardées!"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Quitter"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showValidate(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Attention"));
    if(dialog->getMessage().isEmpty())
        dialog->setMessage(tr("Êtes-vous sûr de vouloir sauvegarder les changements ?"));
    if(dialog->getInfos().isEmpty())
        dialog->setInfos(tr("Les modifications sont irreversibles!"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Sauvegarder"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showDelete(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Action);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Attention"));
    if(dialog->getMessage().isEmpty())
        dialog->setMessage(tr("Êtes-vous sûr de vouloir supprimer les éléments sélectionnés ?"));
    if(dialog->getInfos().isEmpty())
        dialog->setInfos(tr("Les modifications sont irreversibles!"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Supprimer"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showText(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Text);
    dialog->setSeverity(DialogSeverities::None);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Information"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Fermer"));

    return dialog;
}

DialogObject* DialogManager::showTree(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Tree);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Fermer"));

    return dialog;
}

DialogObject* DialogManager::showBusy(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Busy);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Patienter"));
    if(dialog->getMessage().isEmpty())
        dialog->setMessage(tr("Une tâche est en cours"));

    return dialog;
}

DialogObject* DialogManager::showDate(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Date);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Valider"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showTime(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Time);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Valider"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showFileTree(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::FileTree);
    dialog->setSeverity(DialogSeverities::Message);

    const QVariantMap map = settings.toMap();

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Fichiers"));
    if(dialog->getMessage().isEmpty() && map.value("selectionType").toInt()==FolderTreeTypes::File)
        dialog->setMessage(tr("Sélectionner un fichier"));
    if(dialog->getMessage().isEmpty() && map.value("selectionType").toInt()==FolderTreeTypes::Dir)
        dialog->setMessage(tr("Sélectionner un dossier"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Valider"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showInput(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Input);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Formulaire"));
    if(dialog->getMessage().isEmpty())
        dialog->setMessage(tr("Remplisser le formulaire"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Valider"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showForm(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Form);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Formulaire"));
    if(dialog->getMessage().isEmpty())
        dialog->setMessage(tr("Remplisser le formulaire"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Valider"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

DialogObject* DialogManager::showAlarm(const QVariant& settings)
{
    DialogObject* dialog = show(settings);

    dialog->setType(DialogTypes::Alarm);
    dialog->setSeverity(DialogSeverities::Message);

    if(dialog->getTitle().isEmpty())
        dialog->setTitle(tr("Planification"));
    if(dialog->getButtonAccept().isEmpty())
        dialog->setButtonAccept(tr("Valider"));
    if(dialog->getButtonReject().isEmpty())
        dialog->setButtonReject(tr("Annuler"));

    return dialog;
}

void DialogManager::onObjectInserted(DialogObject* object) const
{
    if(!object->getDiagnose())
        return;

    diagnose(object->getSeverity(), object->getTitle());
    diagnose(object->getSeverity(), object->getMessage());
    diagnose(object->getSeverity(), object->getInfos());
    diagnose(object->getSeverity(), object->getTraces());
}

void DialogManager::diagnose(DialogSeverities::Enum severity, QString log) const
{
    if(!log.isEmpty())
    {
        log.replace("\n","; ");

        switch(severity) {
        case DialogSeverities::Fatal:
        case DialogSeverities::Error:
            POPUPLOG_CRITICAL()<<severity<<log;
            break;
        case DialogSeverities::Warning:
            POPUPLOG_WARNING()<<severity<<log;
            break;
        case DialogSeverities::Info:
            POPUPLOG_INFO()<<severity<<log;
            break;
        case DialogSeverities::Message:
            POPUPLOG_DEBUG()<<severity<<log;
            break;
        default:
            POPUPLOG_TRACE()<<severity<<log;
            break;
        }
    }
}
