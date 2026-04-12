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

/*
(?=.*[a-z]): Positive lookahead for at least one lowercase letter.
(?=.*[A-Z]): Positive lookahead for at least one uppercase letter.
(?=.*\d): Positive lookahead for at least one digit.
(?=.*[!@#$%^&*]): Positive lookahead for at least one special character (adjust the set as needed).
.{8,}: Matches any character (except newline) 8 or more times.

How to Adapt for Different Rules
To add more special characters: Expand the character set within the lookahead, e.g., (?=.*[@$!%*?&^#()]).
To remove a rule: Simply remove its corresponding lookahead, e.g., remove (?=.*\d) to not require a number.
For simpler checks: Use separate regexes for each rule (e.g., /\d/, /[A-Z]/, /.{8,}/) for easier reading and maintenance, as recommended by some Stack Overflow users.
*/
Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, passwordRegExp, (R"(^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[!@#$%^&*]).{8,}$)"))

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

const QRegularExpression& RegExpUtils::passwordRegExp()
{
    return *::passwordRegExp;
}
