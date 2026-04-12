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

        SnackbarObject* snackbar = SnackbarManager::Get()->show(m_settings);
        snackbar->setSeverity(m_severity);
        snackbar->setTitle(m_title);
        snackbar->setCaption(m_caption);
        snackbar->setDetails(m_details);
        snackbar->setButton(m_button);
        snackbar->setClosable(m_closable);
        snackbar->setTimeout(0);
        snackbar->setProgress(m_progress);
        snackbar->setDiagnose(m_diagnose);

        connect(this, &SnackbarLoader::settingsChanged, snackbar, &SnackbarObject::setSettings);
        connect(this, &SnackbarLoader::severityChanged, snackbar, &SnackbarObject::setSeverity);
        connect(this, &SnackbarLoader::titleChanged, snackbar, &SnackbarObject::setTitle);
        connect(this, &SnackbarLoader::captionChanged, snackbar, &SnackbarObject::setCaption);
        connect(this, &SnackbarLoader::detailsChanged, snackbar, &SnackbarObject::setDetails);
        connect(this, &SnackbarLoader::buttonChanged, snackbar, &SnackbarObject::setButton);
        connect(this, &SnackbarLoader::closableChanged, snackbar, &SnackbarObject::setClosable);
        connect(this, &SnackbarLoader::progressChanged, snackbar, &SnackbarObject::setProgress);
        connect(this, &SnackbarLoader::diagnoseChanged, snackbar, &SnackbarObject::setDiagnose);

        connect(snackbar, &SnackbarObject::closed, this, &SnackbarLoader::closed);
        connect(snackbar, &SnackbarObject::accepted, this, &SnackbarLoader::accepted);

        connect(snackbar, &SnackbarObject::closed, this, &SnackbarLoader::resetActive);
        connect(snackbar, &SnackbarObject::accepted, this, &SnackbarLoader::resetActive);

        m_snackbar = snackbar;
    }
    else
    {
        clearSnackbar();
    }
}
