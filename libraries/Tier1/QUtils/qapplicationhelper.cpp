#include "qapplicationhelper.h"

#include <QUrl>
#include <QGuiApplication>
#include <QCursor>
#include <QQmlEngine>
#include <QIcon>
#include <QRegularExpression>

Q_GLOBAL_STATIC_WITH_ARGS(QRegularExpression, qrcRegExp, ("qrc:/+"))

QApplicationHelper::QApplicationHelper(QObject* parent):
    QObject(parent),
    m_completed(false)
{
    connect(this, &QApplicationHelper::windowIconChanged, this, &QApplicationHelper::refreshWindowIcon);
    connect(this, &QApplicationHelper::cursorShapeChanged, this, &QApplicationHelper::refreshCursor);
    connect(this, &QApplicationHelper::fontChanged, this, &QApplicationHelper::refreshFont);
}

void QApplicationHelper::classBegin()
{

}

void QApplicationHelper::componentComplete()
{
    m_completed = true;
    refreshWindowIcon();
    refreshCursor();
    refreshFont();
}

void QApplicationHelper::refreshWindowIcon()
{
    if(!m_completed)
        return;

    QString icon = m_windowIcon;
    if(icon.startsWith("file://"))
        icon = QUrl(icon).toLocalFile();
    else if(icon.startsWith("qrc:/"))
        icon.replace(*qrcRegExp, ":/");

    QGuiApplication::setWindowIcon(QIcon(icon));
}

void QApplicationHelper::refreshCursor()
{
    if(!m_completed)
        return;

    if(m_cursorShape==-1)
    {
        QGuiApplication::restoreOverrideCursor();
    }
    else
    {
        QCursor cursor((Qt::CursorShape)m_cursorShape);
        QGuiApplication::setOverrideCursor(cursor);
        QGuiApplication::changeOverrideCursor(cursor);
    }
}

void QApplicationHelper::refreshFont()
{
    if(!m_completed)
        return;

    QFont font = QGuiApplication::font();
    font.setFamily(m_font);
    QGuiApplication::setFont(font);
}
