#include "regexpfilter.h"
#include <QVariant>

namespace QQsfpm {

/*!
    \qmltype RegExpFilter
    \inherits RoleFilter
    \inqmlmodule SortFilterProxyModel
    \ingroup Filters
    \brief  Filters rows matching a regular expression.

    A RegExpFilter is a \l RoleFilter that accepts rows matching a regular rexpression.

    In the following example, only rows with their \c lastName role beggining with the content of textfield the will be accepted:
    \code
    TextField {
       id: nameTextField
    }

    SortFilterProxyModel {
       sourceModel: contactModel
       filters: RegExpFilter {
           roleName: "lastName"
           pattern: "^" + nameTextField.displayText
       }
    }
    \endcode
*/

RegExpFilter::RegExpFilter(QObject *parent) : RoleFilter(parent)
{
    connect(this, &RegExpFilter::patternChanged, this, &RegExpFilter::updatePattern);
    connect(this, &RegExpFilter::caseSensitivityChanged, this, &RegExpFilter::updateCaseSensitivity);
}

void RegExpFilter::updatePattern()
{
    m_regExp.setPattern(m_pattern);
    invalidate();
}

void RegExpFilter::updateCaseSensitivity()
{
    auto patternOptions = m_regExp.patternOptions();
    patternOptions.setFlag(QRegularExpression::CaseInsensitiveOption, m_caseSensitivity == Qt::CaseInsensitive);
    m_regExp.setPatternOptions(patternOptions);
    invalidate();
}

bool RegExpFilter::filterRow(const QModelIndex& sourceIndex, const QQmlSortFilterProxyModel& proxyModel) const
{
    QString string = sourceData(sourceIndex, proxyModel).toString();
    return m_regExp.match(string).hasMatch();
}

}
