#include "snackbarmanager.h"
#include "popup_log.h"

SnackbarManager::SnackbarManager(QObject *parent) :
    QObject(parent),
    m_children(new QObjectListModel(this, &SnackbarObject::staticMetaObject))
{
    m_children->onInserted<SnackbarObject>([this](SnackbarObject* object){
        this->onObjectInserted(object);
    });
}

SnackbarObject* SnackbarManager::show(const QVariant& settings)
{
    SnackbarObject* snackbar = new SnackbarObject(settings, this);

    QMetaObject::invokeMethod(this, [this, snackbar](){
        if(snackbar->isMarkedForDeletion())
            return;
        m_children->prepend(snackbar);
    }, Qt::QueuedConnection);

    return snackbar;
}

SnackbarObject* SnackbarManager::showInfo(const QVariant& settings)
{
    SnackbarObject* snackbar = show(settings);

    snackbar->setSeverity(SnackbarSeverities::Info);

    if(snackbar->getTitle().isEmpty())
        snackbar->setTitle(tr("Information"));

    return snackbar;
}

SnackbarObject* SnackbarManager::showWarning(const QVariant& settings)
{
    SnackbarObject* snackbar = show(settings);

    snackbar->setSeverity(SnackbarSeverities::Warning);

    if(snackbar->getTitle().isEmpty())
        snackbar->setTitle(tr("Attention"));

    return snackbar;
}

SnackbarObject* SnackbarManager::showError(const QVariant& settings)
{
    SnackbarObject* snackbar = show(settings);

    snackbar->setSeverity(SnackbarSeverities::Error);

    if(snackbar->getTitle().isEmpty())
        snackbar->setTitle(tr("Erreur"));

    return snackbar;
}

SnackbarObject* SnackbarManager::showFatal(const QVariant& settings)
{
    SnackbarObject* snackbar = show(settings);

    snackbar->setSeverity(SnackbarSeverities::Fatal);
    snackbar->setClosable(true);

    if(snackbar->getTitle().isEmpty())
        snackbar->setTitle(tr("Alerte"));

    return snackbar;
}

SnackbarObject* SnackbarManager::showSuccess(const QVariant& settings)
{
    SnackbarObject* snackbar = show(settings);

    snackbar->setSeverity(SnackbarSeverities::Success);

    if(snackbar->getTitle().isEmpty())
        snackbar->setTitle(tr("Succès"));

    return snackbar;
}

void SnackbarManager::onObjectInserted(SnackbarObject* object) const
{
    if(!object->getDiagnose())
        return;

    diagnose(object->getSeverity(), object->getTitle());
    diagnose(object->getSeverity(), object->getCaption());
}

void SnackbarManager::diagnose(SnackbarSeverity severity, QString log) const
{
    if(!log.isEmpty())
    {
        log.replace("\n","; ");

        switch(severity) {
        case SnackbarSeverities::None:
            POPUPLOG_TRACE()<<severity<<log;
            break;
        case SnackbarSeverities::Fatal:
        case SnackbarSeverities::Error:
            POPUPLOG_CRITICAL()<<severity<<log;
            break;
        case SnackbarSeverities::Warning:
            POPUPLOG_WARNING()<<severity<<log;
            break;
        case SnackbarSeverities::Info:
            POPUPLOG_INFO()<<severity<<log;
            break;
        default:
            POPUPLOG_DEBUG()<<severity<<log;
            break;
        }
    }
}
