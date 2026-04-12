#include "extravalidatorattached.h"

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
