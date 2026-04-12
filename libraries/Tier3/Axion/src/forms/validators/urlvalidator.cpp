#include "urlvalidator.h"

UrlValidator::UrlValidator(QObject *parent) :
    AbstractValidator(parent)
{
    connect(this, &UrlValidator::schemesChanged, this, &AbstractValidator::queueInvalidate);
}

void UrlValidator::invalidate()
{
    AbstractValidator::invalidate();
}

QValidator::State UrlValidator::validate(QString &input, int &pos) const
{
    Q_UNUSED(pos);

    if (!isReady())
        return QValidator::Acceptable;

    const QString trimmed = input.trimmed();
    if (trimmed.isEmpty())
        return QValidator::Intermediate;

    const QUrl url = QUrl::fromUserInput(trimmed);

    if (!url.isValid() || url.scheme().isEmpty())
        return QValidator::Intermediate;

    if (!m_schemes.isEmpty()) {
        const QString scheme = url.scheme();

        bool allowed = std::any_of(m_schemes.cbegin(), m_schemes.cend(),
                                   [&scheme](const QString &s) {
                                       return s.compare(scheme, Qt::CaseInsensitive) == 0;
                                   });

        if (!allowed)
            return QValidator::Invalid;
    }

    return QValidator::Acceptable;
}

void UrlValidator::fixup(QString &text) const
{
    text = QUrl::fromUserInput(text).toString();
}
