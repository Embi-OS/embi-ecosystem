#ifndef STEPELEMENT_H
#define STEPELEMENT_H

#include <QDefs>

Q_ENUM_CLASS(StepStates, StepState,
             None,
             Started,
             Ended,
             Skipped)

class StepElement : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_REF_PROPERTY(QString, label, Label, {})
    Q_WRITABLE_REF_PROPERTY(QString, instruction, Instruction, {})
    Q_WRITABLE_REF_PROPERTY(QString, icon, Icon, {})
    Q_WRITABLE_VAR_PROPERTY(bool, optional, Optional, false)
    Q_WRITABLE_PTR_PROPERTY(QQmlComponent, delegate, Delegate, nullptr)

    Q_READONLY_FUZ_PROPERTY(float, progress, Progress, 0)
    Q_READONLY_VAR_PROPERTY(StepStates::Enum, state, State, StepStates::None)

public:
    explicit StepElement(QObject* parent = nullptr);

public slots:
    void reset();

    void start();
    void skip();
    void end();

signals:
    void started();
    void skipped();
    void ended();
};

#endif // STEPELEMENT_H
