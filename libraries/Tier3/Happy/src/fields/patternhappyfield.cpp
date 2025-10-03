#include "patternhappyfield.h"
#include "happy_log.h"

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpPattern, (R"(\?<(.*?)(?:,(\d+))?>)"))

PatternHappyField::PatternHappyField(QObject* parent):
    AbstractHappyField(parent)
{

}

void PatternHappyField::preRead(const HappyHttpParameters& parameters, const QSqlRecord& baseRec)
{
    QRegularExpressionMatchIterator i = regExpPattern->globalMatch(m_pattern);

    m_string = m_pattern;
    m_captures.clear();
    m_roleNames.clear();
    m_indexes.clear();
    m_precisions.clear();

    // Parcourir chaque correspondance
    int index = 1;  // Index de départ pour %1, %2, etc.
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString capture = match.captured(0);  // Capturer le terme dans la chaîne originale
        const QString roleName = match.captured(1);  // Capturer le texte entre "< >"
        const QString precision = match.captured(2);  // Capturer la précision (si elle existe)

        m_captures.append(capture);
        m_roleNames.append(roleName);
        m_indexes.append(baseRec.indexOf(roleName));
        m_precisions.append(precision.isEmpty()?-1:precision.toUInt());

        m_string.replace(capture, "%" % QString::number(index));
        index++;
    }
}

QVariant PatternHappyField::read(const QSqlRecord& record, bool* ok) const
{
    QString result = m_string;

    for (int i = 0; i < m_indexes.size(); ++i) {
        int index = m_indexes[i];
        if(index<0) {
            result = result.arg(m_captures[i]);
        }
        else {
            const QVariant value = record.value(index);
            int precision = m_precisions[i];
            if(precision<0)
                result = result.arg(value.toString());
            else
                result = result.arg(value.toDouble(), 0, 'f', precision);
        }
    }

    if(ok)
        *ok = true;

    return result;
}

QVariant PatternHappyField::write(const QVariantMap& values, bool creation, bool* ok) const
{
    if(ok)
        *ok = false;

    return QVariant();
}

