#include "snackbarloader.h"
#include "snackbarmanager.h"

SnackbarLoader::SnackbarLoader(QObject *parent) :
    QObject(parent)
{
    connect(this, &SnackbarLoader::activeChanged, this, &SnackbarLoader::onActiveChanged);
}

SnackbarLoader::~SnackbarLoader()
{
    clearSnackbar();
}

void SnackbarLoader::clearSnackbar()
{
    if(m_snackbar.isNull())
        return;
    m_snackbar->close();
    m_snackbar.clear();
    m_active = false;
}

void SnackbarLoader::onActiveChanged(bool active)
{
    if(active)
    {
        if(!m_snackbar.isNull())
        {
            m_snackbar->close();
            m_snackbar.clear();
        }

        SnackbarObject* dialog = SnackbarManager::Get()->show(m_settings);
        dialog->setSeverity(m_severity);
        dialog->setTitle(m_title);
        dialog->setCaption(m_caption);
        dialog->setDetails(m_details);
        dialog->setButton(m_button);
        dialog->setClosable(m_closable);
        dialog->setTimeout(0);
        dialog->setProgress(m_progress);
        dialog->setDiagnose(m_diagnose);

        connect(this, &SnackbarLoader::settingsChanged, dialog, &SnackbarObject::setSettings);
        connect(this, &SnackbarLoader::severityChanged, dialog, &SnackbarObject::setSeverity);
        connect(this, &SnackbarLoader::titleChanged, dialog, &SnackbarObject::setTitle);
        connect(this, &SnackbarLoader::captionChanged, dialog, &SnackbarObject::setCaption);
        connect(this, &SnackbarLoader::detailsChanged, dialog, &SnackbarObject::setDetails);
        connect(this, &SnackbarLoader::buttonChanged, dialog, &SnackbarObject::setButton);
        connect(this, &SnackbarLoader::closableChanged, dialog, &SnackbarObject::setClosable);
        connect(this, &SnackbarLoader::progressChanged, dialog, &SnackbarObject::setProgress);
        connect(this, &SnackbarLoader::diagnoseChanged, dialog, &SnackbarObject::setDiagnose);

        connect(dialog, &SnackbarObject::closed, this, &SnackbarLoader::closed);
        connect(dialog, &SnackbarObject::accepted, this, &SnackbarLoader::accepted);

        m_snackbar = dialog;
    }
    else
    {
        clearSnackbar();
    }
}
