#include "formatutils.h"

FormatUtils::FormatUtils(QObject *parent) :
    QObject(parent)
{

}

QString FormatUtils::hex(qint64 num, int digits) const
{
    return ::hex(num, digits);
}

QString FormatUtils::hex(const QByteArray& byteArray) const
{
    return ::hex(byteArray);
}

QString FormatUtils::bin(qint64 num, int digits) const
{
    return ::bin(num, digits);
}

QString FormatUtils::bin(const QByteArray& byteArray) const
{
    return ::bin(byteArray);
}

QByteArray FormatUtils::fromHex(const QString& byteArray) const
{
    return ::fromHex(byteArray);
}

qint32 FormatUtils::intFromHex(const QString& byteArray) const
{
    return ::intFromHex(byteArray);
}

QString FormatUtils::bytes(qint64 num, int decimals) const
{
    return ::bytes(num, decimals);
}

QString FormatUtils::lsqueeze(const QString &str, int maxlen) const
{
    return ::lsqueeze(str, maxlen);
}

QString FormatUtils::csqueeze(const QString &str, int maxlen) const
{
    return ::csqueeze(str, maxlen);
}

QString FormatUtils::rsqueeze(const QString &str, int maxlen) const
{
    return ::rsqueeze(str, maxlen);
}

QString FormatUtils::obscure(const QString &str) const
{
    return ::obscure(str);
}

QString FormatUtils::realToString(double n, int precision, char format) const
{
    return QLocale::c().toString(n, format, precision);
}

QString FormatUtils::intToString(qint64 n, int size, char fill) const
{
    QString string = QLocale::c().toString(n);
    return string.rightJustified(size, fill, false);
}
