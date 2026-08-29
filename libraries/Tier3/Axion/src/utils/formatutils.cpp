#include "formatutils.h"
#include <QUtils>
#include <QRegularExpression>

static QString cleanQmlTypeName(QString typeName)
{
    const int addressIndex = typeName.indexOf('(');
    if(addressIndex>=0)
        typeName.truncate(addressIndex);

    typeName = typeName.trimmed();
    typeName.remove(QRegularExpression(QStringLiteral("_(QMLTYPE|QML)_\\d+$")));
    return typeName;
}

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

QString FormatUtils::qmlTypeName(const QVariant& variant) const
{
    const QVariant value = qVariantFromJSVariant(variant);
    if(QObject* object = value.value<QObject*>())
        return cleanQmlTypeName(QString::fromLatin1(object->metaObject()->className()));

    return cleanQmlTypeName(value.toString());
}

QByteArray FormatUtils::variantToLog(const QVariant& variant, bool compact) const
{
    return QUtils::Log::variantToLog(qVariantFromJSVariant(variant), compact);
}

QByteArray FormatUtils::variantToJson(const QVariant& variant, bool compact, int doublePrecision) const
{
    return QUtils::Json::variantToJson(qVariantFromJSVariant(variant), compact, doublePrecision);
}

QVariant FormatUtils::jsonToVariant(const QByteArray& json) const
{
    return QUtils::Json::jsonToVariant(json);
}

QByteArray FormatUtils::variantToCbor(const QVariant& variant, int opt) const
{
    return QUtils::Cbor::variantToCbor(qVariantFromJSVariant(variant), opt);
}

QVariant FormatUtils::cborToVariant(const QByteArray& cbor) const
{
    return QUtils::Cbor::cborToVariant(cbor);
}

QVariantMap FormatUtils::objectToVariant(const QObject* object) const
{
    return QUtils::Object::objectToVariant(object);
}

bool FormatUtils::variantToObject(const QVariantMap& variant, QObject* object) const
{
    return QUtils::Object::variantToObject(variant, object);
}
