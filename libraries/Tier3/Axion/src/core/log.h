#ifndef LOG_H
#define LOG_H

#include <QDefs>
#include "qsingleton.h"

class Log : public QObject,
            public QQmlSingleton<Log>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_WRITABLE_VAR_PROPERTY(bool, canOverlay, CanOverlay, false)
    Q_WRITABLE_VAR_PROPERTY(bool, overlay, Overlay, false)

protected:
    friend QQmlSingleton<Log>;
    explicit Log(QObject *parent = nullptr);

public:
    Q_INVOKABLE void startElapsed() const;
    Q_INVOKABLE double elapsed() const;

    Q_INVOKABLE void trace(const QString& message) const;
    Q_INVOKABLE void debug(const QString& message) const;
    Q_INVOKABLE void info(const QString& message) const;
    Q_INVOKABLE void warning(const QString& message) const;
    Q_INVOKABLE void critical(const QString& message) const;
    Q_INVOKABLE void fatal(const QString& message) const;

public slots:
    static void init();
    static void unInit();
    bool clear();
    bool copy(const QString& path);

private:
    static bool m_ready;
};

#endif // LOG_H
