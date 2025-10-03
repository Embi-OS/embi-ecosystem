#ifndef FORMATUTILS_H
#define FORMATUTILS_H

#include <QDefs>
#include "qsingleton.h"

class FormatUtils : public QObject,
                    public QQmlSingleton<FormatUtils>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

protected:
    friend QQmlSingleton<FormatUtils>;
    explicit FormatUtils(QObject *parent = nullptr);

public:
    Q_INVOKABLE QString hex(qint64 num, int digits=0) const;
    Q_INVOKABLE QString hex(const QByteArray& byteArray) const;
    Q_INVOKABLE QString bin(qint64 num, int digits=0) const;
    Q_INVOKABLE QString bin(const QByteArray& byteArray) const;
    Q_INVOKABLE QByteArray fromHex(const QString& byteArray) const;
    Q_INVOKABLE qint32 intFromHex(const QString& byteArray) const;
    Q_INVOKABLE QString bytes(qint64 num, int decimals=3) const;
    Q_INVOKABLE QString lsqueeze(const QString &str, int maxlen = 40) const;
    Q_INVOKABLE QString csqueeze(const QString &str, int maxlen = 40) const;
    Q_INVOKABLE QString rsqueeze(const QString &str, int maxlen = 40) const;
    Q_INVOKABLE QString obscure(const QString &str) const;
    Q_INVOKABLE QString realToString(double n, int precision=6, char format='F') const;
    Q_INVOKABLE QString intToString(qint64 n, int size=-1, char fill='0') const;
};

#endif // FORMATUTILS_H
