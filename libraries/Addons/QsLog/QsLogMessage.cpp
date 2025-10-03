#include "QsLogMessage.h"
#include "QsLogLevel.h"

#include <QString>
#include <QRegularExpression>
#include <QFileInfo>
#include <QDebug>

namespace QsLogging
{

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, regExpCpp, ("(([a-zA-Z_][a-zA-Z0-9_]*)::([a-zA-Z_][a-zA-Z0-9_]*)\\()"))

Message::Message(const QString& m, const QDateTime& t, const Level lev, const QString& f, const QString& func, const QString& cat, int l) :
    message(m),
    time(t),
    level(lev),
    file(f),
    function(func),
    category(cat),
    line(l)
{

}

Message::Message(const QsLogging::Message &other) :
    Message(other.message, other.time, other.level, other.file, other.function, other.category, other.line)
{
    display = other.display;
}

QString Message::formatTime(const QDateTime& time)
{
    return time.toString(Qt::ISODateWithMs);
}

QString Message::formatLevel(QsLogging::Level level)
{
    return QString("[%1]").arg(QsLogging::levelToText(level));
}

QString Message::formatFile(const QString& file)
{
    return QFileInfo(file).fileName();
}

QString Message::formatFunction(const QString& function)
{
    QString prefix;
    QRegularExpressionMatchIterator rxIterator;
    if(function.endsWith(".qml"))
    {
        prefix = QUrl(function).fileName();
    }
    else
    {
        rxIterator = regExpCpp->globalMatch(function);
        if(rxIterator.hasNext())
        {
            QRegularExpressionMatch match = rxIterator.next();
            prefix = match.captured(1);
            prefix.removeLast();
        }
    }

    if(prefix.isEmpty())
        prefix = function;

    prefix.resize(50,' ');

    return prefix;
}

QString Message::formatCategory(const QString& category)
{
    QString retCategory(category.toUpper());
    if(retCategory.contains("DEFAULT"))
        retCategory.clear();

    retCategory.resize(10,' ');

    return retCategory;
}

QString Message::formatLine(int line)
{
    if(line<0)
        line=0;
    return QString("%1").arg(line, 4);
}

const QColor& Message::color() const
{
    return levelToColor(level);
}

}

QsLogging::Message& QsLogging::Message::operator=(const QsLogging::Message& other)
{
    message=other.message;
    time=other.time;
    level=other.level;
    file=other.file;
    function=other.function;
    category=other.category;
    line=other.line;
    display=other.display;

    return *this;
}

QDebug operator<<(QDebug dbg, const QsLogging::Message &message)
{
    dbg.noquote().nospace()
                 <<"QsLogging::Message("
                 <<"(message: "<<message.message.trimmed()
                 <<"); (time: "<<QsLogging::Message::formatTime(message.time).trimmed()
                 <<"); (level: "<<QsLogging::Message::formatLevel(message.level).trimmed()
                 <<"); (file: "<<QsLogging::Message::formatFile(message.file).trimmed()
                 <<"); (function: "<<QsLogging::Message::formatFunction(message.function).trimmed()
                 <<"); (category: "<<QsLogging::Message::formatCategory(message.category).trimmed()
                 <<"); (line: "<<QsLogging::Message::formatLine(message.line).trimmed()
                 <<"))";
    return dbg.quote().space();
}
