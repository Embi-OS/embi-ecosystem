#ifndef STEPMACHINE_H
#define STEPMACHINE_H

#include <QModels>

Q_ENUM_CLASS(StepMachineStates, StepMachineState,
             None,
             Started,
             Finished)

#include "stepelement.h"
class StepMachine : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT

    Q_READONLY_FUZ_PROPERTY(float, progress, Progress, 0)
    Q_READONLY_VAR_PROPERTY(StepMachineStates::Enum, state, State, StepMachineStates::None)
    Q_READONLY_PTR_PROPERTY(StepElement, currentStep, CurrentStep, nullptr)

public:
    explicit StepMachine(QObject* parent = nullptr);

    void classBegin() override;
    void componentComplete() override;

    Q_INVOKABLE StepElement* firstStep() const;
    Q_INVOKABLE StepElement* lastStep() const;
    Q_INVOKABLE StepElement* nextStep() const;
    Q_INVOKABLE StepElement* previousStep() const;

public slots:
    void reset();

signals:
    void entered(StepElement* step);
    void skipped(StepElement* step);
    void ended(StepElement* step);

    void started();
    void finished();

private slots:
    void onCurrentStepAboutToChange(StepElement* step, StepElement* newStep);

    void onCurrentStepProgressChanged(float stepProgress);
    void onCurrentStepStarted();
    void onCurrentStepSkipped();
    void onCurrentStepEnded();
};
#endif // STEPMACHINE_H
