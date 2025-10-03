#include "methodvalidator.h"
#include "form_log.h"

#include <QQmlContext>
#include <QQmlExpression>

MethodValidator::MethodValidator(QObject *parent) :
    AbstractValidator(parent)
{

}

void MethodValidator::componentComplete()
{
    updateMethod();
}

QValidator::State MethodValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    if(!m_method.isValid())
        return QValidator::Acceptable;

    int ret;
    void *argv[] = {&ret, &input};
    QMetaObject::metacall(const_cast<MethodValidator*>(this), QMetaObject::InvokeMetaMethod, m_method.methodIndex(), argv);

    if(ret<0 || ret>2) {
        FORMLOG_WARNING()<<"Can't convert result to QValidator::State";
        return QValidator::Invalid;
    }

    return (QValidator::State)ret;
}

void MethodValidator::updateMethod()
{
    const auto *metaObj = metaObject();
    for (int idx = metaObj->methodOffset(); idx < metaObj->methodCount(); idx++) {
        // Once we find the method signature, break the loop
        QMetaMethod method = metaObj->method(idx);
        if (method.name() == "method") {
            m_method = method;
            break;
        }
    }

    if (!m_method.isValid())
        return;

    if (m_method.parameterCount() != 1) {
        FORMLOG_WARNING()<<"MethodValidator method requires a single parameter";
        return;
    }

    if (m_method.parameterMetaType(0).id()!=QMetaType::QString) {
        FORMLOG_WARNING()<<"MethodValidator method parameter must be QString";
        return;
    }

    if (m_method.returnMetaType().id()!=QMetaType::Int) {
        FORMLOG_WARNING()<<"MethodValidator method return must be integer";
        return;
    }

    invalidate();
}
