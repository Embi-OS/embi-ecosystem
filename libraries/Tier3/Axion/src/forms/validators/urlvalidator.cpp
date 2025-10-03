#include "urlvalidator.h"

UrlValidator::UrlValidator(QObject *parent) :
    AbstractValidator(parent),
    m_schemeValidator(new QRegularExpressionValidator(this))
{
    m_schemeValidator->setLocale(locale());

    connect(this, &UrlValidator::schemesChanged, this, &AbstractValidator::queueInvalidate);
}

void UrlValidator::invalidate()
{
    m_schemeValidator->setLocale(locale());
    m_schemeValidator->setRegularExpression(QRegularExpression(QStringLiteral(R"__(^(?:%1)\:\/\/.*)__")
                                                             .arg(m_schemes.join(QLatin1Char('|')))));

    AbstractValidator::invalidate();
}

QValidator::State UrlValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    if(!m_schemes.isEmpty()) {
        QValidator::State res = m_schemeValidator->validate(input, pos);
        if(res != QValidator::Acceptable)
            return res;
    }

    auto url = QUrl::fromUserInput(input);
    if(url.isValid())
        return QValidator::Acceptable;
    else
        return QValidator::Intermediate;
}

void UrlValidator::fixup(QString &text) const
{
    text = QUrl::fromUserInput(text).toString();
}
