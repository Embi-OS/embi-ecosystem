#include "socketportvalidator.h"

SocketPortValidator::SocketPortValidator(QObject* parent) :
    AbstractValidator(parent),
    m_intValidator(new QIntValidator(this))
{
    m_intValidator->setLocale(locale());
    m_intValidator->setBottom(0);
    m_intValidator->setTop(65535);

    QString summary = tr("%1 à %2").arg(0).arg(65535);
    setSummary(summary);
}

QValidator::State SocketPortValidator::validate(QString &input, int &pos) const
{
    if(!isReady())
        return QValidator::Acceptable;

    return m_intValidator->validate(input, pos);
}
