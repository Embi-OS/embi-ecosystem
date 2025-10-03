#include "QsLogLevel.h"

Q_GLOBAL_STATIC_WITH_ARGS(const QString, NoticeString,("NOTE"))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, TraceString, ("TRAC"))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, DebugString, ("DBUG"))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, InfoString,  ("INFO"))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, WarnString,  ("WARN"))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, CritString,  ("CRIT"))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, FatalString, ("FATL"))
Q_GLOBAL_STATIC_WITH_ARGS(const QString, OffString,   ("OFF"))

QsLogging::Level QsLogging::levelFromQtMsgType(const int type)
{
    switch (type) {
        case -2:
            return NoticeLevel;
        case -1:
            return TraceLevel;
        case QtDebugMsg:
            return DebugLevel;
        case QtInfoMsg:
            return InfoLevel;
        case QtWarningMsg:
            return WarnLevel;
        case QtCriticalMsg:
            return CritLevel;
        case QtFatalMsg:
            return FatalLevel;
        default:
            return OffLevel;
    }

    return OffLevel;
}

QsLogging::Level QsLogging::levelFromLogMessage(const QString& logMessage, bool* conversionSucceeded)
{
    if (conversionSucceeded)
        *conversionSucceeded = true;

    if (logMessage.contains(*NoticeString))
        return NoticeLevel;
    if (logMessage.contains(*TraceString))
        return TraceLevel;
    if (logMessage.contains(*DebugString))
        return DebugLevel;
    if (logMessage.contains(*InfoString))
        return InfoLevel;
    if (logMessage.contains(*WarnString))
        return WarnLevel;
    if (logMessage.contains(*CritString))
        return CritLevel;
    if (logMessage.contains(*FatalString))
        return FatalLevel;
    if (logMessage.contains(*OffString))
        return OffLevel;

    if (conversionSucceeded)
        *conversionSucceeded = false;
    return OffLevel;
}

const QString& QsLogging::levelToText(QsLogging::Level theLevel)
{
    switch (theLevel) {
        case NoticeLevel:
            return *NoticeString;
        case TraceLevel:
            return *TraceString;
        case DebugLevel:
            return *DebugString;
        case InfoLevel:
            return *InfoString;
        case WarnLevel:
            return *WarnString;
        case CritLevel:
            return *CritString;
        case FatalLevel:
            return *FatalString;
        case OffLevel:
            return *OffString;
        default: {
            return *OffString;
        }
    }
    return *OffString;
}

static const QColor NoticeColor(0xFF,0xFF,0xFF,0x00);
static const QColor TraceColor(0x40,0x40,0x40,0xFF);
static const QColor DebugColor(0xAA,0xAA,0xAA,0xFF);
static const QColor InfoColor (0x00,0xDD,0xFF,0xFF);
static const QColor WarnColor (0xF3,0xFF,0x00,0xFF);
static const QColor CritColor(0xFF,0x8F,0x00,0xFF);
static const QColor FatalColor(0xFF,0x00,0x20,0xFF);
static const QColor OffColor(0xFF,0xFF,0xFF,0xFF);

const QColor& QsLogging::levelToColor(QsLogging::Level theLevel)
{
    switch (theLevel) {
        case NoticeLevel:
            return NoticeColor;
        case TraceLevel:
            return TraceColor;
        case DebugLevel:
            return DebugColor;
        case InfoLevel:
            return InfoColor;
        case WarnLevel:
            return WarnColor;
        case CritLevel:
            return CritColor;
        case FatalLevel:
            return FatalColor;
        case OffLevel:
            return OffColor;
        default: {
            return OffColor;
        }
    }
}

QString QsLogging::colorStart(const QColor& color, QsLogging::Level theLevel)
{
    static int r, g, b;
    color.getRgb(&r, &g, &b);
    return QString("\x1b[38;2;%1;%2;%3m").arg(r).arg(g).arg(b);
}

QString QsLogging::colorEnd()
{
    return QString("\033[0m");
}
