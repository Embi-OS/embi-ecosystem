#ifndef FORMOBJECTMODEL_H
#define FORMOBJECTMODEL_H

#include <QModels>
#include <QDefs>

#include "formobject.h"

class FormObjectModel : public QStandardObjectModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_CLASSINFO("DefaultProperty", "content")

    Q_WRITABLE_VAR_PROPERTY(bool, enabled, Enabled, true)
    Q_WRITABLE_VAR_PROPERTY(bool, editable, Editable, true)
    Q_WRITABLE_VAR_PROPERTY(bool, fitLabel, FitLabel, false)
    Q_WRITABLE_FUZ_PROPERTY(double, labelWidthRatio, LabelWidthRatio, 0.5)
    Q_WRITABLE_VAR_PROPERTY(FormEditPolicies::Enum, selectPolicy, SelectPolicy, FormEditPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(FormEditPolicies::Enum, submitPolicy, SubmitPolicy, FormEditPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(FormEditPolicies::Enum, revertPolicy, RevertPolicy, FormEditPolicies::Direct)
    Q_WRITABLE_PTR_PROPERTY(QObject, target, Target, nullptr)

    Q_WRITABLE_VAR_PROPERTY(bool, selectWhen, SelectWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, submitWhen, SubmitWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, revertWhen, RevertWhen, true)

    Q_READONLY_VAR_PROPERTY(bool, selecting, Selecting, false)
    Q_READONLY_VAR_PROPERTY(bool, submiting, Submiting, false)
    Q_READONLY_VAR_PROPERTY(bool, reverting, Reverting, false)
    Q_READONLY_VAR_PROPERTY(bool, loading, Loading, false)

    Q_READONLY_VAR_PROPERTY(bool, warning, Warning, false)
    Q_READONLY_VAR_PROPERTY(bool, isDirty, IsDirty, false)

public:
    explicit FormObjectModel(QObject* parent = nullptr);

    void classBegin() override final;
    void componentComplete() override final;

    Q_INVOKABLE QVariantMap currentValues() const;
    Q_INVOKABLE void fillValues(const QVariantMap& values);

public slots:
    void queueSelect();
    bool select();

    void queueSubmit();
    bool submit() override final;

    void queueRevert();
    void revert() override final;

signals:
    void selectDone(bool result);
    void submitDone(bool result);
    void revertDone(bool result);

protected:
    void connectFormObjectProperties(FormObject* object);

    bool doSelect();
    bool doSubmit();
    bool doRevert();

protected slots:
    void onTargetDestroyed(QObject* target);

private:
    bool m_completed=false;

    bool m_selectQueued = false;
    bool m_submitQueued = false;
    bool m_revertQueued = false;
};

#endif // FORMOBJECTMODEL_H
