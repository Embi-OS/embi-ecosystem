#include "patternrole.h"
#include "qqmlsortfilterproxymodel.h"
#include <QDebug>

namespace QQsfpm {

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpPattern, (R"(\?<(.*?)(?:,(\d+))?>)"))

PatternRole::PatternRole(QObject *parent) : SingleRole(parent)
{
    connect(this, &PatternRole::patternChanged, this, &PatternRole::updatePattern);

    connect(this, &PatternRole::patternChanged, this, &PatternRole::namesChanged);
}

void PatternRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    QList<int> roles;
    for (const QString& roleName : std::as_const(m_roleNames)) {
        int role = proxyModel.roleForName(roleName);
        roles << role;
    }
    m_roles = roles;

    invalidate();
}

void PatternRole::updatePattern()
{
    QRegularExpressionMatchIterator i = regExpPattern->globalMatch(m_pattern);

    m_string = m_pattern;
    m_captures.clear();
    m_roleNames.clear();
    m_roles.clear();
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
        m_precisions.append(precision.isEmpty()?-1:precision.toUInt());

        m_string.replace(capture, "%" % QString::number(index));
        index++;
    }

    emit this->roleNamesInvalidated();
}

QVariant PatternRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel)
{
    QString result = m_string;

    for (int i = 0; i < m_roles.size(); ++i) {
        int role = m_roles[i];
        if(role<0) {
            result = result.arg(m_captures[i]);
        }
        else {
            const QVariant value = proxyModel.sourceData(sourceIndex, role);
            int precision = m_precisions[i];
            if(precision<0)
                result = result.arg(value.toString());
            else
                result = result.arg(value.toDouble(), 0, 'f', precision);
        }
    }

    return result;
}

}
