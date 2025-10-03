#ifndef RANDOMUTILS_H
#define RANDOMUTILS_H

#include <QDefs>
#include <QColor>
#include "qsingleton.h"

class RandomUtils : public QObject,
                    public QQmlSingleton<RandomUtils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<RandomUtils>;
    explicit RandomUtils(QObject *parent = nullptr);

public:
    Q_INVOKABLE static QColor color();
    Q_INVOKABLE static QString uuid();
};

#endif // RANDOMUTILS_H
