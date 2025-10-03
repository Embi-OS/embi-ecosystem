#include "formobjectmodel.h"
#include "qstandardobjectgroup.h"
#include "form_log.h"

FormObjectModel::FormObjectModel(QObject* parent) :
    QStandardObjectModel(parent)
{
    QStandardObjectGroup* group = new QStandardObjectGroup(this);
    group->setDelayed(true);
    group->setEnabledWarning(true);
    group->setEnabledDirty(true);
    connect(group, &QStandardObjectGroup::warningChanged, this, &FormObjectModel::setWarning);
    connect(group, &QStandardObjectGroup::isDirtyChanged, this, &FormObjectModel::setIsDirty);
    connect(this, &FormObjectModel::completedChanged, group, &QStandardObjectGroup::invalidate, Qt::SingleShotConnection);

    onInserted<QStandardObject>([group](QStandardObject* object) {
        group->addObject(object);
    });
    onRemoved<QStandardObject>([group](QStandardObject* object) {
        group->removeObject(object);
    });

    connect(this, &FormObjectModel::targetChanged, this, [this](){
        if(!m_target)
            return;
        connect(m_target, &QObject::destroyed, this, &FormObjectModel::onTargetDestroyed);
    });
    connect(this, &FormObjectModel::targetChanged, this, &FormObjectModel::queueSelect, Qt::QueuedConnection);

    connect(this, &FormObjectModel::selectWhenChanged, this, &FormObjectModel::queueSelect);
    connect(this, &FormObjectModel::submitWhenChanged, this, &FormObjectModel::queueSubmit);
    connect(this, &FormObjectModel::revertWhenChanged, this, &FormObjectModel::queueRevert);
}

void FormObjectModel::classBegin()
{

}

void FormObjectModel::componentComplete()
{
    QStandardObjectModel::componentComplete();

    forEach([&](QObject* object) mutable {
        if(FormObject* formObject = qobject_cast<FormObject*>(object))
            connectFormObjectProperties(formObject);
        return true;
    });

    if(m_selectPolicy<FormEditPolicies::Manual && m_selectWhen)
        select();
}

void FormObjectModel::onTargetDestroyed(QObject* target)
{
    if(!target)
        return;
    setTarget(nullptr);
}

void FormObjectModel::queueSelect()
{
    if(!m_completed || !m_selectWhen)
        return;

    if (m_selectPolicy==FormEditPolicies::Direct) {
        select();
    }
    else if (m_selectPolicy==FormEditPolicies::Delayed) {
        if(!m_selectQueued) {
            m_selectQueued = true;
            QMetaObject::invokeMethod(this, &FormObjectModel::select, Qt::QueuedConnection);
        }
    }
}

bool FormObjectModel::select()
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
        FORMLOG_WARNING()<<this<<"Select policy is disabled";
        return false;
    }

    if(m_submitPolicy<FormEditPolicies::Manual && m_isDirty) {
        submit();
    }

    setLoading(true);
    setSelecting(true);

    bool result = doSelect();

    setSelecting(false);
    setLoading(false);

    emit this->selectDone(result);

    return result;
}

void FormObjectModel::queueSubmit()
{
    if(!m_completed || !m_submitWhen)
        return;

    if (m_submitPolicy==FormEditPolicies::Direct) {
        submit();
    }
    else if (m_submitPolicy==FormEditPolicies::Delayed) {
        if(!m_submitQueued) {
            m_submitQueued = true;
            QMetaObject::invokeMethod(this, &FormObjectModel::submit, Qt::QueuedConnection);
        }
    }
}

bool FormObjectModel::submit()
{
    m_submitQueued = false;

    if(!m_submitWhen)
        return false;

    if (m_submitPolicy==FormEditPolicies::Disabled) {
        FORMLOG_WARNING()<<this<<"Submit policy is disabled";
        return false;
    }

    if(!m_isDirty)
        return true;

    setLoading(true);
    setSubmiting(true);

    bool result = doSubmit();

    setSubmiting(false);
    setLoading(false);

    emit this->submitDone(result);

    return result;
}

void FormObjectModel::queueRevert()
{
    if(!m_completed || !m_revertWhen)
        return;

    if (m_revertPolicy==FormEditPolicies::Direct) {
        revert();
    }
    else if (m_revertPolicy==FormEditPolicies::Delayed) {
        if(!m_revertQueued) {
            m_revertQueued = true;
            QMetaObject::invokeMethod(this, &FormObjectModel::revert, Qt::QueuedConnection);
        }
    }
}

void FormObjectModel::revert()
{
    m_revertQueued = false;

    if(!m_revertWhen)
        return;

    if (m_revertPolicy==FormEditPolicies::Disabled) {
        FORMLOG_WARNING()<<this<<"Revert policy is disabled";
        return;
    }

    setLoading(true);
    setReverting(true);

    bool result = doRevert();

    setReverting(false);
    setLoading(false);

    emit this->revertDone(result);
}

bool FormObjectModel::doSelect()
{
    bool result = true;
    forEach([&](QObject* object) mutable {
        if(FormObject* formObject = qobject_cast<FormObject*>(object))
            result &= formObject->select();
        return true;
    });
    return result;
}

bool FormObjectModel::doSubmit()
{
    if(m_warning)
    {
        FORMLOG_WARNING()<<"Cannot submit"<<this<<"if warning is true";
        return false;
    }

    bool result = true;
    forEach([&](QObject* object) mutable {
        if(FormObject* formObject = qobject_cast<FormObject*>(object))
            result &= formObject->submit();
        return true;
    });
    return result;
}

bool FormObjectModel::doRevert()
{
    bool result = true;
    forEach([&](QObject* object) mutable {
        if(FormObject* formObject = qobject_cast<FormObject*>(object))
            formObject->revert();
        return true;
    });
    return result;
}

QVariantMap FormObjectModel::currentValues() const
{
    QVariant map = QVariantMap();
    forEach([&](QObject* object) mutable {
        if(FormObject* formObject = qobject_cast<FormObject*>(object)) {
            const QString key = formObject->getTargetProperty();
            if(key.isEmpty())
                return true;
            const QVariant value = formObject->getCurrentValue();
            qVariantMergeNestedValue(map, key, value, formObject->getTargetPropertySeparator());
        }
        return true;
    });

    return map.toMap();
}

void FormObjectModel::fillValues(const QVariantMap& values)
{
    if(values.isEmpty())
        return;

    forEach([&](QObject* object) mutable {
        if(FormObject* formObject = qobject_cast<FormObject*>(object)) {
            const QString key = formObject->getTargetProperty();
            if(key.isEmpty())
                return true;
            const QVariant value = qVariantGetNestedValue(values, key, formObject->getTargetPropertySeparator());
            formObject->setValue(value);
            if(formObject->getSelectPolicy()!=FormEditPolicies::Direct)
                formObject->select();
        }
        return true;
    });
}

void FormObjectModel::connectFormObjectProperties(FormObject* object)
{
    // formObject->setEnabled(m_enabled);
    object->setEditable(m_editable);
    object->setFitLabel(m_fitLabel);
    object->setLabelWidthRatio(m_labelWidthRatio);
    object->setSelectPolicy(m_selectPolicy);
    object->setSubmitPolicy(m_submitPolicy);
    object->setRevertPolicy(m_revertPolicy);
    object->setSelectWhen(m_selectWhen);
    object->setSubmitWhen(m_submitWhen);
    object->setRevertWhen(m_revertWhen);
    if(m_target)
        object->setTarget(m_target);

    // connect(this, &FormObjectModel::enabledChanged, object, &FormObject::setEnabled);
    connect(this, &FormObjectModel::editableChanged, object, &FormObject::setEditable);
    connect(this, &FormObjectModel::fitLabelChanged, object, &FormObject::setFitLabel);
    connect(this, &FormObjectModel::labelWidthRatioChanged, object, &FormObject::setLabelWidthRatio);
    connect(this, &FormObjectModel::selectPolicyChanged, object, &FormObject::setSelectPolicy);
    connect(this, &FormObjectModel::submitPolicyChanged, object, &FormObject::setSubmitPolicy);
    connect(this, &FormObjectModel::revertPolicyChanged, object, &FormObject::setRevertPolicy);
    connect(this, &FormObjectModel::selectWhenChanged, object, &FormObject::setSelectWhen);
    connect(this, &FormObjectModel::submitWhenChanged, object, &FormObject::setSubmitWhen);
    connect(this, &FormObjectModel::revertWhenChanged, object, &FormObject::setRevertWhen);
    connect(this, &FormObjectModel::targetChanged, object, &FormObject::setTarget);
}
