#include "formobject.h"
#include "form_log.h"

FormObject::FormObject(QObject *parent) :
    QStandardObject(parent)
{
    connect(this, &FormObject::targetAboutToChange, this, [this](){
        if(!m_target)
            return;
        disconnect(this, nullptr, m_target, nullptr);
        disconnect(m_target, nullptr, this, nullptr);
    });
    connect(this, &FormObject::targetChanged, this, [this](){
        if(!m_target)
            return;
        connect(m_target, &QObject::destroyed, this, &FormObject::onTargetDestroyed);
    });

    connect(this, &FormObject::targetChanged, this, &FormObject::targetValidityChanged);
    connect(this, &FormObject::targetPropertyChanged, this, &FormObject::targetValidityChanged);
    connect(this, &FormObject::targetPropertySeparatorChanged, this, &FormObject::targetValidityChanged);

    connect(this, &FormObject::targetValidityChanged, this, &FormObject::updateMetaProperty);

    connect(this, &FormObject::formTypeChanged, this, &FormObject::queueSelect);
    connect(this, &FormObject::defaultValueChanged, this, &FormObject::queueSelect);
    connect(this, &FormObject::valueTypeChanged, this, &FormObject::queueSelect);

    connect(this, &FormObject::valueChanged, this, &FormObject::queueSelect);
    connect(this, &FormObject::targetPropertyValueChanged, this, &FormObject::queueSelect);

    connect(this, &FormObject::selectWhenChanged, this, &FormObject::queueSelect);
    connect(this, &FormObject::submitWhenChanged, this, &FormObject::queueSubmit);
    connect(this, &FormObject::revertWhenChanged, this, &FormObject::queueRevert);

    connect(this, &FormObject::currentValueChanged, this, &FormObject::validate);
    connect(this, &FormObject::validatorAboutToChange, this, &FormObject::onValidatorAboutToChange);
    connect(this, &FormObject::validatorChanged, this, &FormObject::onValidatorChanged);
}

void FormObject::classBegin()
{

}

void FormObject::componentComplete()
{
    QStandardObject::componentComplete();

    if(m_selectPolicy!=FormEditPolicies::Manual)
        select();
}

void FormObject::onTargetDestroyed(QObject* target)
{
    if(!target)
        return;
    setTarget(nullptr);
}

bool FormObject::isTargetValid() const
{
    if(!m_target)
        return false;

    QString propertyName = m_targetProperty;
    if(!m_targetPropertySeparator.isEmpty() && m_targetProperty.contains(m_targetPropertySeparator))
    {
        QStringList properties = m_targetProperty.split(m_targetPropertySeparator);
        propertyName = properties.takeFirst();
    }

    if(m_visible && m_target && !m_targetProperty.isEmpty() && m_target->metaObject()->indexOfProperty(propertyName.toUtf8())<0) {
        FORMLOG_CRITICAL()<<"Target"<<m_target<<"does not contains property"<<propertyName;
    }

    return m_target && !m_targetProperty.isEmpty() && m_target->metaObject()->indexOfProperty(propertyName.toUtf8())>=0;
}

bool FormObject::isVariantValid(const QVariant& variant) const
{
    if(variant.userType()==QMetaType::UnknownType)
    {
        if(variant.isNull() || !variant.isValid())
            return false;
        return true;
    }

    switch (variant.userType()) {
    case QMetaType::Int:
        return variant.toInt()!=0;
    case QMetaType::UInt:
        return variant.toUInt()!=0;
    case QMetaType::LongLong:
        return variant.toLongLong()!=0;
    case QMetaType::ULongLong:
        return variant.toULongLong()!=0;
    case QMetaType::Float:
        return !qFuzzyIsNull(variant.toFloat());
    case QMetaType::Double:
        return !qFuzzyIsNull(variant.toDouble());
    case QMetaType::QChar:
        return !variant.toChar().isNull();
    case QMetaType::QDate:
        return !(variant.toDate().isNull() || variant.toDate().isValid());
    case QMetaType::QTime:
        return !(variant.toTime().isNull() || variant.toTime().isValid());
    case QMetaType::QDateTime:
        return !(variant.toDateTime().isNull() || variant.toDateTime().isValid());
    case QMetaType::QVariantList:
        return !variant.toList().isEmpty();
    case QMetaType::QVariantMap:
        return !variant.toMap().isEmpty();
    case QMetaType::QVariantHash:
        return !variant.toHash().isEmpty();
    case QMetaType::QString:
    default:
        return !variant.toString().isEmpty();
    }
}

bool FormObject::isTextValid(const QString& text) const
{
    if(!m_validator)
        return true;

    QString valueToValidate = text;
    int cursorPos=valueToValidate.size();

    return m_validator->validate(valueToValidate,cursorPos)==QValidator::Acceptable;
}

bool FormObject::isValueValid(const QVariant& value) const
{
    switch (m_formType) {
    case FormTypes::TextFieldUnit:
        return true;
    default:
        const QString text = valueToText(value);
        return isTextValid(text);
        break;
    }
}

void FormObject::queueSelect()
{
    if(!m_completed || !m_selectWhen)
        return;

    if (m_selectPolicy==FormEditPolicies::Direct) {
        select();
    }
    else if (m_selectPolicy==FormEditPolicies::Delayed) {
        if(!m_selectQueued) {
            m_selectQueued = true;
            QMetaObject::invokeMethod(this, &FormObject::select, Qt::QueuedConnection);
        }
    }
}

bool FormObject::select()
{
    if(!m_completed)
    {
        m_completed = true;
        emit this->completedChanged(true);
    }

    m_selectQueued = false;

    if(!m_selectWhen)
        return false;

    if (m_selectPolicy==FormEditPolicies::Disabled) {
        FORMLOG_WARNING()<<this<<"Select policy is disabled"<<m_formType;
        return false;
    }

    if(m_submitPolicy<FormEditPolicies::Manual && m_isDirty) {
        submit();
    }

    setLoading(true);
    setSelecting(true);

    bool result = doSelect();

    setIsDirty(false);
    setSelecting(false);
    setLoading(false);

    return result;
}

void FormObject::queueSubmit()
{
    if(!m_completed || !m_submitWhen)
        return;

    if (m_submitPolicy==FormEditPolicies::Direct) {
        submit();
    }
    else if (m_submitPolicy==FormEditPolicies::Delayed) {
        if(!m_submitQueued) {
            m_submitQueued = true;
            QMetaObject::invokeMethod(this, &FormObject::submit, Qt::QueuedConnection);
        }
    }
}

bool FormObject::submit()
{
    m_submitQueued = false;

    if(!m_submitWhen)
        return false;

    if (m_submitPolicy==FormEditPolicies::Disabled) {
        FORMLOG_WARNING()<<this<<"Submit policy is disabled"<<m_formType;
        return false;
    }

    if(!m_isDirty)
        return true;

    setLoading(true);
    setSubmiting(true);

    bool result = doSubmit();

    setIsDirty(false);
    setSubmiting(false);
    setLoading(false);

    return result;
}

void FormObject::queueRevert()
{
    if(!m_completed || !m_revertWhen)
        return;

    if (m_revertPolicy==FormEditPolicies::Direct) {
        revert();
    }
    else if (m_revertPolicy==FormEditPolicies::Delayed) {
        if(!m_revertQueued) {
            m_revertQueued = true;
            QMetaObject::invokeMethod(this, &FormObject::revert, Qt::QueuedConnection);
        }
    }
}

void FormObject::revert()
{
    m_revertQueued = false;

    if(!m_revertWhen)
        return;

    if (m_revertPolicy==FormEditPolicies::Disabled) {
        FORMLOG_WARNING()<<this<<"Revert policy is disabled"<<m_formType;
        return;
    }

    setLoading(true);
    setReverting(true);

    bool result = doRevert();

    setIsDirty(false);
    setReverting(false);
    setLoading(false);

    Q_UNUSED(result)
}

bool FormObject::doSelect()
{
    bool result = false;

    if(m_value.isValid())
        result = loadCurrentValue();
    else if(isTargetValid())
        result = loadTargetValue();
    else
        result = loadDefaultValue();

    return result;
}

bool FormObject::doSubmit()
{
    if(!m_enabled || !m_visible)
        return true;

    if(m_warning)
    {
        FORMLOG_WARNING()<<"Cannot submit"<<this<<"if warning is true";
        return false;
    }

    const QVariant formattedValue = formatValue(m_currentValue);

    bool result = true;
    if(m_target && !m_targetProperty.isEmpty())
    {
        QStringList properties;
        QString propertyName = m_targetProperty;
        if(!m_targetPropertySeparator.isEmpty() && m_targetProperty.contains(m_targetPropertySeparator))
        {
            properties = m_targetProperty.split(m_targetPropertySeparator);
            propertyName = properties.takeFirst();
        }

        if(properties.isEmpty())
        {
            result &= m_target->setProperty(propertyName.toUtf8(), formattedValue);
        }
        else
        {
            QVariant propertyValue = m_targetMetaProperty.read(m_target);
            if (propertyValue.userType() == qMetaTypeId<QJSValue>())
                propertyValue = propertyValue.value<QJSValue>().toVariant();
            result &= qVariantSetNestedValue(propertyValue, properties, formattedValue);
            result &= m_target->setProperty(propertyName.toUtf8(), propertyValue);
        }

        if(!result) {
            FORMLOG_WARNING()<<"Unable to set property"<<m_targetProperty<<"on target"<<m_target;
        }
    }

    emit this->accepted(formattedValue);

    return result;
}

bool FormObject::doRevert()
{
    bool result = doSelect();

    return result;
}

bool FormObject::changeValue(const QVariant& value)
{
    QVariant formattedValue = formatValue(value);
    if(!setCurrentValue(formattedValue))
        return false;

    if(isTargetValid())
    {
        setIsDirty(m_currentValue!=m_targetPropertyValue);
    }
    else
    {
        setIsDirty(m_currentValue!=m_value);
    }

    queueSubmit();

    return true;
}

bool FormObject::loadDefaultValue()
{
    QVariant formattedValue = formatValue(m_defaultValue);
    return setCurrentValue(formattedValue);
}

bool FormObject::loadCurrentValue()
{
    QVariant formattedValue = formatValue(m_value);
    return setCurrentValue(formattedValue);
}

bool FormObject::loadTargetValue()
{
    QVariant formattedValue = formatValue(m_targetPropertyValue);
    return setCurrentValue(formattedValue);
}

void FormObject::validate()
{
    validateValue(m_currentValue);
}

void FormObject::validateValue(const QVariant& value)
{
    if(!m_completed)
        return;

    bool acceptableInput = isValueValid(value);
    setAcceptableInput(acceptableInput);
}

void FormObject::onValidatorAboutToChange(QValidator* oldValidator, QValidator* newValidator)
{
    if(oldValidator)
    {
        disconnect(oldValidator, nullptr,this, nullptr);
        disconnect(this, nullptr,oldValidator, nullptr);
    }
}

void FormObject::onValidatorChanged(QValidator* validator)
{
    if (validator) {
        connect(validator, &QValidator::changed, this, &FormObject::validate);
    }

    validate();
}

void FormObject::updateMetaProperty()
{
    if(!isTargetValid())
        return;

    QStringList properties;
    QString propertyName = m_targetProperty;
    if(!m_targetPropertySeparator.isEmpty() && m_targetProperty.contains(m_targetPropertySeparator))
    {
        properties = m_targetProperty.split(m_targetPropertySeparator);
        propertyName = properties.takeFirst();
    }

    const QMetaObject *mo = m_target->metaObject();
    const int index = mo->indexOfProperty(propertyName.toUtf8());
    if(index<0)
        return;

    m_targetMetaProperty = mo->property(index);
    if (m_targetMetaProperty.hasNotifySignal())
    {
        static const char* HANDLER = "onTargetMetaPropertyChanged()";
        const QMetaMethod notifySignal = m_targetMetaProperty.notifySignal();
        const QMetaMethod updateSlot = metaObject()->method(metaObject()->indexOfSlot(HANDLER));
        connect(m_target,notifySignal,this,updateSlot);
    }

    const QVariant propertyValue = m_targetMetaProperty.read(m_target);
    const QVariant value = properties.isEmpty() ? propertyValue : qVariantGetNestedValue(propertyValue, properties);
    setTargetPropertyValue(value);
}

void FormObject::onTargetMetaPropertyChanged()
{
    if(!m_target)
        return;

    const int signalIndex = senderSignalIndex();
    if(signalIndex!=m_targetMetaProperty.notifySignalIndex())
        return;

    QStringList properties;
    if(!m_targetPropertySeparator.isEmpty() && m_targetProperty.contains(m_targetPropertySeparator))
    {
        properties = m_targetProperty.split(m_targetPropertySeparator);
        properties.removeFirst();
    }

    const QVariant propertyValue = m_targetMetaProperty.read(m_target);
    const QVariant value = properties.isEmpty() ? propertyValue : qVariantGetNestedValue(propertyValue, properties);
    setTargetPropertyValue(value);
}

QVariant FormObject::formatValue(const QVariant& value) const
{
    switch (m_formType) {
    case FormTypes::Color:
        return value.value<QColor>();
    case FormTypes::Slider:
        return  value.toDouble();
    case FormTypes::Switch:
        return value.toBool();
    case FormTypes::TextFieldUnit:
        return QVariantMap({{"value", value.toMap().value("value").toDouble()},
                            {"type", value.toMap().value("type").toInt()}});
    case FormTypes::TextFieldDate:
        switch (value.userType()) {
        case QMetaType::QDate:
            return value.toDate();
        case QMetaType::QTime:
        case QMetaType::QDateTime:
            return value.toDateTime().date();
        default:
            return QDate::fromString(value.toString());
        }
        break;
    case FormTypes::TextFieldTime:
        switch (value.userType()) {
        case QMetaType::QTime:
            return value.toTime();
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            return value.toDateTime().time();
        default:
            return QTime::fromString(value.toString());
        }
        break;
    case FormTypes::TextField:
        switch (m_valueType) {
        case FormValueTypes::Integer:
            return value.toInt();
        case FormValueTypes::Double:
            return value.toDouble();
        case FormValueTypes::Bool:
            return value.toBool();
        case FormValueTypes::Color:
            return value.value<QColor>();
        default:
            return value.toString();
        }
    case FormTypes::TextFieldPath:
    case FormTypes::TextFieldPassword:
    case FormTypes::TextArea:
        return value.toString();
    case FormTypes::TextFieldHex:
        return value.toInt();
    case FormTypes::ComboCheckList:
        return value.toList();
    case FormTypes::ComboBox:
    default:
        switch (m_valueType) {
        case FormValueTypes::Integer:
            return value.toInt();
        case FormValueTypes::Double:
            return value.toDouble();
        case FormValueTypes::Bool:
            return value.toBool();
        case FormValueTypes::Color:
            return value.value<QColor>();
        case FormValueTypes::Date:
            return value.toDate();
        case FormValueTypes::Time:
            return value.toTime();
        case FormValueTypes::DateTime:
            return value.toDateTime();
        case FormValueTypes::List:
            return value.toList();
        case FormValueTypes::Map:
            return value.toMap();
        default:
            return value;
        }
    }
}

QString FormObject::valueToText(const QVariant& value) const
{
    QString text;

    switch (m_formType) {
    case FormTypes::TextFieldHex:
        text = hex(value.toInt());
        break;
    case FormTypes::TextFieldUnit:
        text = QString::number(value.toMap().value("value").toDouble(), 'g', QLocale::FloatingPointShortest);
        break;
    case FormTypes::TextFieldDate:
        switch (value.userType()) {
        case QMetaType::QDate:
            text = value.toDate().toString("dd/MM/yyyy");
            break;
        case QMetaType::QTime:
        case QMetaType::QDateTime:
            text = value.toDateTime().date().toString("dd/MM/yyyy");
            break;
        default:
            text = value.toString();
        }
        break;
    case FormTypes::TextFieldTime:
        switch (value.userType()) {
        case QMetaType::QTime:
            text = value.toTime().toString("hh:mm");
            break;
        case QMetaType::QDate:
        case QMetaType::QDateTime:
            text = value.toDateTime().time().toString("hh:mm");
            break;
        default:
            text = value.toString();
        }
        break;
    case FormTypes::ComboCheckList: {
        const QVariantList values = value.toList();
        QStringList ret;
        ret.reserve(values.size());
        for(const QVariant& val: values)
            ret.append(val.toString());
        return ret.join("; ");
    }
    case FormTypes::Color:
    case FormTypes::Slider:
    case FormTypes::Switch:
    case FormTypes::TextField:
    case FormTypes::TextFieldPath:
    case FormTypes::TextFieldPassword:
    case FormTypes::TextArea:
    case FormTypes::ComboBox:
    default:
        switch (m_valueType) {
        case FormValueTypes::Date:
            text = QLocale().toString(value.toDate());
            break;
        case FormValueTypes::Time:
            text = QLocale().toString(value.toTime());
            break;
        case FormValueTypes::DateTime:
            text = QLocale().toString(value.toDateTime());
            break;
        case FormValueTypes::List:
            text = QString::fromUtf8(QUtils::Log::variantToJson(value));
            break;
        case FormValueTypes::Map:
            text = QString::fromUtf8(QUtils::Log::variantToJson(value));
            break;
        case FormValueTypes::Integer:
            text = QLocale().c().toString(value.toLongLong());
            break;
        case FormValueTypes::Double:
            text = QLocale().c().toString(value.toDouble());
            break;
        default:
            text = value.toString();
            break;
        }
    }

    return text;
}
