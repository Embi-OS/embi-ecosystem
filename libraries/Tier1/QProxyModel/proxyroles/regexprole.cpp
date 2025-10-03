#include "regexprole.h"
#include "qqmlsortfilterproxymodel.h"
#include <QDebug>

namespace QQsfpm {

/*!
    \qmltype RegExpRole
    \inherits ProxyRole
    \inqmlmodule SortFilterProxyModel
    \ingroup ProxyRoles
    \brief A ProxyRole extracting data from a source role via a regular expression.

    A RegExpRole is a \l ProxyRole that provides a role for each named capture group of its regular expression \l pattern.

    In the following example, the \c date role of the source model will be extracted in 3 roles in the proxy moodel: \c year, \c month and \c day.
    \code
    SortFilterProxyModel {
        sourceModel: eventModel
        proxyRoles: RegExpRole {
            roleName: "date"
            pattern: "(?<year>\\d{4})-(?<month>\\d{2})-(?<day>\\d{2})"
        }
    }
    \endcode
*/

RegExpRole::RegExpRole(QObject *parent) : ProxyRole(parent)
{
    connect(this, &RegExpRole::roleNameChanged, this, &RegExpRole::roleNamesInvalidated);
    connect(this, &RegExpRole::roleChanged, this, &RegExpRole::invalidate);

    connect(this, &RegExpRole::patternChanged, this, &RegExpRole::updatePattern);
    connect(this, &RegExpRole::caseSensitivityChanged, this, &RegExpRole::updateCaseSensitivity);

    connect(this, &RegExpRole::patternChanged, this, &RegExpRole::namesChanged);
}

void RegExpRole::updatePattern()
{
    m_regExp.setPattern(m_pattern);
    invalidate();
}

void RegExpRole::updateCaseSensitivity()
{
    auto patternOptions = m_regExp.patternOptions();
    patternOptions.setFlag(QRegularExpression::CaseInsensitiveOption, m_caseSensitivity == Qt::CaseInsensitive);
    m_regExp.setPatternOptions(patternOptions);
    invalidate();
}

QStringList RegExpRole::names()
{
    QStringList nameCaptureGroups = m_regExp.namedCaptureGroups();
    nameCaptureGroups.removeAll("");
    return nameCaptureGroups;
}

void RegExpRole::updateRoles(const QQmlSortFilterProxyModel& proxyModel)
{
    int role = proxyModel.roleForName(m_roleName);
    setRole(role);

    ProxyRole::updateRoles(proxyModel);
}

QVariant RegExpRole::data(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel, const QString &name)
{
    QString text = proxyModel.sourceData(sourceIndex, m_role).toString();
    QRegularExpressionMatch match = m_regExp.match(text);
    return match.hasMatch() ? (match.captured(name)) : QVariant{};
}

}
