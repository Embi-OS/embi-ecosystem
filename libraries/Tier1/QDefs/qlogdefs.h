#ifndef QLOGDEFS_H
#define QLOGDEFS_H

#include <QDebug>
#include <QString>
#include <QStringLiteral>
#include <QVariant>

#define QDEBUG_NO_TRACE false
#define qTrace \
    if(QDEBUG_NO_TRACE) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1B").debug

#define QDEBUG_NO_NOTICE false
#define qNotice \
    if(QDEBUG_NO_TRACE) {} \
    else QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1C").debug

//──────────────────────────────────────────────────────────────────────
// Fonctions de debug
//──────────────────────────────────────────────────────────────────────

Q_GLOBAL_STATIC(QByteArray, qLogLineData)

inline const char* qLogLineMessage(const QString& title, const QChar& repeat='-')
{
    QString line(100,repeat);

    line.replace(line.size()/2-title.size()/2,title.size(),title);

    qLogLineData->clear();
    qLogLineData->append(line.toUtf8());

    return qLogLineData->constData();
}

inline const char* qLogLine(const QChar& repeat='-')
{
    return qLogLineMessage("",repeat);
}

#endif // QLOGDEFS_H
