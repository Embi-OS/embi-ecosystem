#include "dialogloader.h"
#include "dialogmanager.h"

DialogLoader::DialogLoader(QObject *parent) :
    QObject(parent)
{
    connect(this, &DialogLoader::activeChanged, this, &DialogLoader::onActiveChanged);
}

DialogLoader::~DialogLoader()
{
    clearDialog();
}

void DialogLoader::clearDialog()
{
    if(m_dialog.isNull())
        return;
    m_dialog->hide();
    m_dialog.clear();
    m_active = false;
}

void DialogLoader::onActiveChanged(bool active)
{
    if(active)
    {
        if(!m_dialog.isNull())
        {
            m_dialog->hide();
            m_dialog.clear();
        }

        DialogObject* dialog = DialogManager::Get()->show(m_settings);
        dialog->setType(m_type);
        dialog->setSeverity(m_severity);
        dialog->setTitle(m_title);
        dialog->setMessage(m_message);
        dialog->setInfos(m_infos);
        dialog->setTraces(m_traces);
        dialog->setProgress(m_progress);
        dialog->setButtonApply(m_buttonApply);
        dialog->setButtonAccept(m_buttonAccept);
        dialog->setButtonReject(m_buttonReject);
        dialog->setButtonDiscard(m_buttonDiscard);
        dialog->setButtonHelp(m_buttonHelp);
        dialog->setDiagnose(m_diagnose);

        connect(this, &DialogLoader::settingsChanged, dialog, &DialogObject::setSettings);
        connect(this, &DialogLoader::typeChanged, dialog, &DialogObject::setType);
        connect(this, &DialogLoader::severityChanged, dialog, &DialogObject::setSeverity);
        connect(this, &DialogLoader::titleChanged, dialog, &DialogObject::setTitle);
        connect(this, &DialogLoader::messageChanged, dialog, &DialogObject::setMessage);
        connect(this, &DialogLoader::infosChanged, dialog, &DialogObject::setInfos);
        connect(this, &DialogLoader::tracesChanged, dialog, &DialogObject::setTraces);
        connect(this, &DialogLoader::progressChanged, dialog, &DialogObject::setProgress);
        connect(this, &DialogLoader::buttonAcceptChanged, dialog, &DialogObject::setButtonAccept);
        connect(this, &DialogLoader::buttonRejectChanged, dialog, &DialogObject::setButtonReject);
        connect(this, &DialogLoader::buttonHelpChanged, dialog, &DialogObject::setButtonHelp);
        connect(this, &DialogLoader::diagnoseChanged, dialog, &DialogObject::setDiagnose);

        connect(dialog, &DialogObject::aboutToHide, this, &DialogLoader::aboutToHide);
        connect(dialog, &DialogObject::aboutToShow, this, &DialogLoader::aboutToShow);
        connect(dialog, &DialogObject::closed, this, &DialogLoader::closed);
        connect(dialog, &DialogObject::opened, this, &DialogLoader::opened);

        connect(dialog, &DialogObject::accepted, this, &DialogLoader::accepted);
        connect(dialog, &DialogObject::discarded, this, &DialogLoader::discarded);
        connect(dialog, &DialogObject::helpRequested, this, &DialogLoader::helpRequested);
        connect(dialog, &DialogObject::rejected, this, &DialogLoader::rejected);
        connect(dialog, &DialogObject::reset, this, &DialogLoader::reset);

        connect(dialog, &DialogObject::dateSelected, this, &DialogLoader::dateSelected);
        connect(dialog, &DialogObject::timeSelected, this, &DialogLoader::timeSelected);
        connect(dialog, &DialogObject::pathSelected, this, &DialogLoader::pathSelected);
        connect(dialog, &DialogObject::inputAccepted, this, &DialogLoader::inputAccepted);
        connect(dialog, &DialogObject::inputApplied, this, &DialogLoader::inputApplied);
        connect(dialog, &DialogObject::formValidated, this, &DialogLoader::formValidated);
        connect(dialog, &DialogObject::alarmValidated, this, &DialogLoader::alarmValidated);

        m_dialog = dialog;
    }
    else
    {
        clearDialog();
    }
}
