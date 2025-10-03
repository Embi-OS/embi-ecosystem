#ifndef FORMOBJECT_H
#define FORMOBJECT_H

#include <QModels>
#include <QDefs>
#include <QUtils>
#include <QValidator>

Q_ENUM_CLASS(FormTypes, FormType,
             None,
             Subtitle,
             TextField,
             TextArea,
             Switch,
             Slider,
             Color,
             ComboBox,
             ComboCheckList,
             TextFieldUnit,
             TextFieldDate,
             TextFieldTime,
             TextFieldPath,
             TextFieldPassword,
             TextFieldHex,)

Q_ENUM_CLASS(FormValueTypes, FormValueType,
             Auto,
             String,
             Integer,
             Double,
             Bool,
             Color,
             Date,
             Time,
             DateTime,
             List,
             Map,)

Q_ENUM_CLASS(FormTextTypes, FormTextType,
             String,
             Number,
             Hex,
             Scan,)

Q_ENUM_CLASS(FormEditPolicies, FormEditPolicy,
             Direct,
             Delayed,
             Manual,
             Disabled)

class FormObject : public QStandardObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_REQUIRED_VAR_PROPERTY(FormTypes::Enum, formType, FormType, FormTypes::None)
    Q_REQUIRED_VAR_PROPERTY(QVariant, defaultValue, DefaultValue, {})
    Q_WRITABLE_VAR_PROPERTY(FormValueTypes::Enum, valueType, ValueType, FormValueTypes::Auto)

    Q_WRITABLE_REF_PROPERTY(QString, label, Label, "")
    Q_WRITABLE_REF_PROPERTY(QString, placeholder, Placeholder, "")
    Q_WRITABLE_REF_PROPERTY(QString, infos, Infos, "")
    Q_WRITABLE_PTR_PROPERTY(QValidator, validator, Validator, nullptr)

    Q_READONLY_REF_PROPERTY(QVariant, currentValue, CurrentValue, {})
    Q_READONLY_VAR_PROPERTY(bool, acceptableInput, AcceptableInput, true)

    // Can be forwarded by model
    Q_WRITABLE_VAR_PROPERTY(bool, fitLabel, FitLabel, false)
    Q_WRITABLE_FUZ_PROPERTY(double, labelWidthRatio, LabelWidthRatio, 0.5)
    Q_WRITABLE_VAR_PROPERTY(FormEditPolicies::Enum, selectPolicy, SelectPolicy, FormEditPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(FormEditPolicies::Enum, submitPolicy, SubmitPolicy, FormEditPolicies::Direct)
    Q_WRITABLE_VAR_PROPERTY(FormEditPolicies::Enum, revertPolicy, RevertPolicy, FormEditPolicies::Direct)
    Q_WRITABLE_PTR_PROPERTY(QObject, target, Target, nullptr)
    Q_WRITABLE_REF_PROPERTY(QString, targetProperty, TargetProperty, "")
    Q_WRITABLE_REF_PROPERTY(QString, targetPropertySeparator, TargetPropertySeparator, ".")
    Q_READONLY_REF_PROPERTY(QVariant, targetPropertyValue, TargetPropertyValue, {})

    Q_WRITABLE_VAR_PROPERTY(bool, selectWhen, SelectWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, submitWhen, SubmitWhen, true)
    Q_WRITABLE_VAR_PROPERTY(bool, revertWhen, RevertWhen, true)

    Q_READONLY_VAR_PROPERTY(bool, selecting, Selecting, false)
    Q_READONLY_VAR_PROPERTY(bool, submiting, Submiting, false)
    Q_READONLY_VAR_PROPERTY(bool, reverting, Reverting, false)
    Q_READONLY_VAR_PROPERTY(bool, loading, Loading, false)

    Q_PROPERTY(bool isTargetValid READ isTargetValid NOTIFY targetValidityChanged)

public:
    explicit FormObject(QObject* parent = nullptr);

    void classBegin() override final;
    void componentComplete() override final;

    bool isTargetValid() const;
    Q_INVOKABLE bool isVariantValid(const QVariant& variant) const;
    Q_INVOKABLE bool isTextValid(const QString& text) const;
    Q_INVOKABLE bool isValueValid(const QVariant& value) const;

    Q_INVOKABLE QVariant formatValue(const QVariant& value) const;
    Q_INVOKABLE QString valueToText(const QVariant& value) const;

public slots:
    void queueSelect();
    bool select();

    void queueSubmit();
    bool submit();

    void queueRevert();
    void revert();

    bool changeValue(const QVariant& value);

    void updateMetaProperty();
    void onTargetMetaPropertyChanged();

    bool loadDefaultValue();
    bool loadCurrentValue();
    bool loadTargetValue();

    void validate();
    void validateValue(const QVariant& value);
    void onValidatorAboutToChange(QValidator* oldValidator, QValidator* newValidator);
    void onValidatorChanged(QValidator* validator);

signals:
    void targetValidityChanged();

    void accepted(const QVariant& value);

protected slots:
    void onTargetDestroyed(QObject* target);

protected:
    bool doSelect();
    bool doSubmit();
    bool doRevert();

private:
    bool m_completed=false;
    QMetaProperty m_targetMetaProperty;

    bool m_selectQueued = false;
    bool m_submitQueued = false;
    bool m_revertQueued = false;
};

#endif // FORMOBJECT_H
