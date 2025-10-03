#ifndef QTIMEDEFS_H
#define QTIMEDEFS_H

#include <QElapsedTimer>
#include <QTimer>
#include <QDebug>
#include <QString>

#define timeInfo QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"TIME").info
#define timeDebug QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"TIME").debug
#define timeTrace QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC,"\x1B""TIME").debug

#define Q_MEASURE_TIME(method) \
    { \
        QElapsedTimer _timer; \
        _timer.start(); \
        method; \
        timeInfo()<<#method<<"took:"<<_timer.nsecsElapsed()/1000000.0<<"ms"; \
    } \

#define Q_DEBUG_TIME(method) \
    { \
        QElapsedTimer _timer; \
        _timer.start(); \
        method; \
        timeDebug()<<#method<<"took:"<<_timer.nsecsElapsed()/1000000.0<<"ms"; \
    } \

#define Q_TRACE_TIME(method) \
    { \
        QElapsedTimer _timer; \
        _timer.start(); \
        method; \
        timeTrace()<<#method<<"took:"<<_timer.nsecsElapsed()/1000000.0<<"ms"; \
    } \

#endif // QTIMEDEFS_H
