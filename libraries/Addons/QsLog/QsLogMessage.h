#ifndef QSLOGMESSAGE_H
#define QSLOGMESSAGE_H

#include "QsLogLevel.h"

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMetaType>
#include <QQmlEngine>

namespace QsLogging
{

class Message
{
    Q_GADGET

    Q_PROPERTY(QString message MEMBER message FINAL)
    Q_PROPERTY(QDateTime time MEMBER time FINAL)
    Q_PROPERTY(Level level MEMBER level FINAL)
    Q_PROPERTY(QString file MEMBER file FINAL)
    Q_PROPERTY(QString function MEMBER function FINAL)
    Q_PROPERTY(QString category MEMBER category FINAL)
    Q_PROPERTY(int line MEMBER line FINAL)
    Q_PROPERTY(QColor color READ color FINAL)
    Q_PROPERTY(QString display MEMBER display FINAL)

public:
    explicit Message() = default;
    ~Message() = default;
    Message(const QString& m, const QDateTime& t, const Level lev, const QString& f, const QString& func, const QString& cat, int l);
    Message(const QsLogging::Message &other);

    static QString formatTime(const QDateTime& time);
    static QString formatLevel(Level level);
    static QString formatFile(const QString& file);
    static QString formatFunction(const QString& function);
    static QString formatCategory(const QString& category);
    static QString formatLine(int line);

    const QColor& color() const;

    QsLogging::Message& operator=(const QsLogging::Message &other);
    inline bool operator==(const QsLogging::Message& rhs) const {
        return message==rhs.message && time==rhs.time && level==rhs.level && file==rhs.file && function==rhs.function && category==rhs.category && line==rhs.line;
    }
    inline bool operator!=(const QsLogging::Message& rhs) const {
        return !(*this==rhs);
    }
    inline bool operator<(const QsLogging::Message& rhs) const {
        return time<rhs.time;
    }
    inline bool operator>(const QsLogging::Message& rhs) const {
        return time>rhs.time;
    }
    inline bool operator<=(const QsLogging::Message& rhs) const {
        return time<=rhs.time;
    }
    inline bool operator>=(const QsLogging::Message& rhs) const {
        return time>=rhs.time;
    }

    QString message;
    QDateTime time;
    Level level;
    QString file;
    QString function;
    QString category;
    int line;

    mutable QString display;
};

}

Q_DECLARE_METATYPE(QsLogging::Message)

QDebug operator<<(QDebug dbg, const QsLogging::Message &message);

#endif // QSLOGMESSAGE_H
