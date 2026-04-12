#ifndef UTILS_H
#define UTILS_H

#include <QDefs>
#include "qsingleton.h"

class Utils : public QObject,
              public QQmlSingleton<Utils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<Utils>;
    explicit Utils(QObject *parent = nullptr);

public:
    Q_INVOKABLE static void await(int msecs, QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents);
    Q_INVOKABLE static void singleshotTimer(int interval, QJSValue callback);
    Q_INVOKABLE static void singleshotTimer(int interval, QObject* context, QJSValue callback);

    Q_INVOKABLE static QStringList fontFamilies();

    Q_INVOKABLE static bool pointIsValid(const QPointF& point);
};

#endif // UTILS_H
