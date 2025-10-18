#ifndef PATHS_H
#define PATHS_H

#include <QDefs>
#include "qsingleton.h"

class Paths : public QObject,
              public QQmlSingleton<Paths>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<Paths>;
    explicit Paths(QObject *parent = nullptr);

public:
    Q_INVOKABLE static QString applicationFilePath();
    Q_INVOKABLE static QString applicationFileName();
    Q_INVOKABLE static QString applicationName();
    Q_INVOKABLE static QString local(const QString& file = QString());
    Q_INVOKABLE static QString log(const QString& file = QString());
    Q_INVOKABLE static QString capture(const QString& file = QString());
    Q_INVOKABLE static QString setting(const QString& file = QString());
    Q_INVOKABLE static QString database(const QString& file = QString());
    Q_INVOKABLE static QString cache(const QString& file = QString());

    Q_INVOKABLE static QString specificLocation(const QString& folder, const QString& file = QString());

public slots:
    static void init();

private:
    static bool m_ready;
};

#endif // PATHS_H
