#include "stepelement.h"
#include "step_log.h"

StepElement::StepElement(QObject *parent) :
    QObject(parent)
{

}

void StepElement::reset()
{
    resetState();
}

void StepElement::start()
{
    setProgress(0);
    setState(StepStates::Started);

    emit started();
}

void StepElement::skip()
{
    Q_ASSERT_X(m_state == StepStates::Started, "StepElement", "Cannot skip a step that is not started");
    if (m_state != StepStates::Started)
    {
        STEPLOG_CRITICAL() << "Cannot skip a step that is not started - state:"<<m_state;
        return;
    }

    Q_ASSERT_X(m_optional, "StepElement", "Cannot skip a step that is not optional");
    if(!m_optional)
    {
        STEPLOG_WARNING() << "Cannot skip a step that is not optionnal";
        return;
    }

    setProgress(100);
    setState(StepStates::Skipped);

    emit skipped();
}

void StepElement::end()
{
    Q_ASSERT_X(m_state == StepStates::Started, "StepElement", "Cannot end a step that is not started");
    if (m_state != StepStates::Started)
    {
        STEPLOG_CRITICAL() << "Cannot end a step that is not started - state:"<<m_state;
        return;
    }

    setProgress(100);
    setState(StepStates::Ended);

    emit ended();
}

