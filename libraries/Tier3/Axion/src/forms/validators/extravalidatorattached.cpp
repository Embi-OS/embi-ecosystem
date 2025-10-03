#include "extravalidatorattached.h"
#include "form_log.h"

ExtraValidatorAttached::ExtraValidatorAttached(QValidator* parent):
    QObject(parent),
    m_validator(parent)
{

}

QString ExtraValidatorAttached::localeName() const
{
    return m_validator->locale().name();
}

void ExtraValidatorAttached::setLocaleName(const QString &name)
{
    if (m_validator->locale().name() != name) {
        m_validator->setLocale(QLocale(name));
        emit localeNameChanged();
    }
}

void ExtraValidatorAttached::resetLocaleName()
{
    QLocale defaultLocale;
    if (m_validator->locale() != defaultLocale) {
        m_validator->setLocale(defaultLocale);
        emit localeNameChanged();
    }
}

ExtraValidatorAttached* ExtraValidatorAttached::wrap(const QObject* object)
{
    return qobject_cast<ExtraValidatorAttached*>(qmlAttachedPropertiesObject<ExtraValidatorAttached>(object, true));
}

ExtraValidatorAttached* ExtraValidatorAttached::qmlAttachedProperties(QObject * object)
{
    const QValidator* validator = qobject_cast<const QValidator*>(object);
    if (!validator)
    {
        FORMLOG_CRITICAL()<<object<<validator;
        FORMLOG_FATAL("ExtraValidatorAttached must be attached to a QValidator*");
        return nullptr;
    }

    ExtraValidatorAttached* helper = new ExtraValidatorAttached(const_cast<QValidator*>(validator));
    QQmlEngine::setObjectOwnership(helper, QQmlEngine::CppOwnership);

    return helper;
}
