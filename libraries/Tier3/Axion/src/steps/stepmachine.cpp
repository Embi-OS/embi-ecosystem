#include "stepmachine.h"

StepMachine::StepMachine(QObject *parent) :
    QObjectListModel(parent, &StepElement::staticMetaObject)
{
    connect(this, &StepMachine::currentStepAboutToChange, this, &StepMachine::onCurrentStepAboutToChange);
}

void StepMachine::classBegin()
{

}

void StepMachine::componentComplete()
{
    reset();
}

StepElement* StepMachine::firstStep() const
{
    if(isEmpty())
        return nullptr;

    StepElement* step = first<StepElement>();
    return step;
}

StepElement* StepMachine::lastStep() const
{
    if(isEmpty())
        return nullptr;

    StepElement* step = last<StepElement>();
    return step;
}

StepElement* StepMachine::nextStep() const
{
    if(isEmpty())
        return nullptr;

    if(!m_currentStep)
        return firstStep();

    int currentIndex = indexOf(m_currentStep);
    if(currentIndex<0 || currentIndex >= size()-1)
        return nullptr;

    return at<StepElement>(currentIndex+1);
}

StepElement* StepMachine::previousStep() const
{
    if(isEmpty())
        return nullptr;

    if(!m_currentStep)
        return lastStep();

    int currentIndex = indexOf(m_currentStep);
    if(currentIndex<=0)
        return nullptr;

    return at<StepElement>(currentIndex-1);
}

void StepMachine::reset()
{
    resetProgress();
    resetState();
    resetCurrentStep();

    for(StepElement* step: modelIterator<StepElement>())
    {
        step->reset();
    }

    setCurrentStep(firstStep());
    m_currentStep->start();
}

void StepMachine::onCurrentStepAboutToChange(StepElement* step, StepElement* newStep)
{
    if(step)
    {
        disconnect(step, nullptr, this, nullptr);
    }

    if(newStep)
    {
        connect(newStep, &StepElement::progressChanged, this, &StepMachine::onCurrentStepProgressChanged);
        connect(newStep, &StepElement::started, this, &StepMachine::onCurrentStepStarted, Qt::QueuedConnection);
        connect(newStep, &StepElement::skipped, this, &StepMachine::onCurrentStepSkipped, Qt::QueuedConnection);
        connect(newStep, &StepElement::ended, this, &StepMachine::onCurrentStepEnded, Qt::QueuedConnection);
    }
}

void StepMachine::onCurrentStepProgressChanged(float stepProgress)
{
    if(!m_currentStep)
        return;

    float progress = ((float)indexOf(m_currentStep)/(float)count())*100.0 + stepProgress/count();
    setProgress(progress);
}

void StepMachine::onCurrentStepStarted()
{
    if(!m_currentStep)
        return;

    emit this->entered(m_currentStep);

    if(isFirst(m_currentStep))
    {
        setState(StepMachineStates::Started);
        emit this->started();
    }
}

void StepMachine::onCurrentStepSkipped()
{
    if(!m_currentStep)
        return;

    emit this->skipped(m_currentStep);

    if(isLast(m_currentStep))
    {
        setState(StepMachineStates::Finished);
        emit this->finished();
        return;
    }

    setCurrentStep(nextStep());
    m_currentStep->start();
}

void StepMachine::onCurrentStepEnded()
{
    if(!m_currentStep)
        return;

    emit this->ended(m_currentStep);

    if(isLast(m_currentStep))
    {
        setState(StepMachineStates::Finished);
        emit this->finished();
        setCurrentStep(nullptr);
        return;
    }

    setCurrentStep(nextStep());
    m_currentStep->start();
}
