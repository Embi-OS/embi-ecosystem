#include "regexputils.h"
#include <QRegularExpression>

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, qrcRegExp, ("qrc:/+"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, markdownRegExp, (R"((^#{1,6} .+)|(^- .+)|(\[.+\]\(.+\)))", QRegularExpression::MultilineOption))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, htmlRegExp, (R"(<[^>]+>)"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, jsonRegExp, (R"(^\s*(\{.*\}|\[.*\])\s*$)", QRegularExpression::DotMatchesEverythingOption))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, xmlRegExp, (R"(<\?xml[^>]*\?>|<[^>]+>)"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, charRegExp, (R"(^[ -~]*$)"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, alphanumericsRegExp, (R"(^[a-zA-Z0-9._-]+$)"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, numberRegExp, (R"(^[0-9]*$)"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, uncPathRegExp, (R"(^\/\/[^\s/]+(?:\/[^\s/]+)+$)"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, pathRegExp, (R"(^(/\S+)?$)"))
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, nonSpaceRegExp, (R"(^\S*$)"))

RegExpUtils::RegExpUtils(QObject *parent) :
    QObject(parent)
{

}

const QRegularExpression& RegExpUtils::qrcRegExp()
{
    return *::qrcRegExp;
}

const QRegularExpression& RegExpUtils::markdownRegExp()
{
    return *::markdownRegExp;
}

const QRegularExpression& RegExpUtils::htmlRegExp()
{
    return *::htmlRegExp;
}

const QRegularExpression& RegExpUtils::jsonRegExp()
{
    return *::jsonRegExp;
}

const QRegularExpression& RegExpUtils::xmlRegExp()
{
    return *::xmlRegExp;
}

const QRegularExpression& RegExpUtils::charRegExp()
{
    return *::charRegExp;
}

const QRegularExpression& RegExpUtils::alphanumericsRegExp()
{
    return *::alphanumericsRegExp;
}

const QRegularExpression& RegExpUtils::numberRegExp()
{
    return *::numberRegExp;
}

const QRegularExpression& RegExpUtils::uncPathRegExp()
{
    return *::uncPathRegExp;
}

const QRegularExpression& RegExpUtils::pathRegExp()
{
    return *::pathRegExp;
}

const QRegularExpression& RegExpUtils::nonSpaceRegExp()
{
    return *::nonSpaceRegExp;
}
