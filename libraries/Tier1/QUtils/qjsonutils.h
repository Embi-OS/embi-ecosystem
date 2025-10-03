#ifndef QJSONUTILS_H
#define QJSONUTILS_H

#include <QVariant>
#include <QByteArray>

struct QJsonParseError;
struct QCborParserError;

namespace QUtils
{

namespace Log
{

QByteArray variantToLog(const QVariant& variant, bool compact = false);
QByteArray variantToJson(const QVariant& variant, bool compact = false);
QByteArray variantToCbor(const QVariant& variant, int opt = 0);

}

namespace Json
{

    QByteArray variantToJson(const QVariant& variant, bool compact = true);
    void variantToJsonStream(const QVariant& variant, QIODevice* device, bool compact = true);

    QVariant jsonToVariant(const QByteArray& json, QJsonParseError* error = nullptr);
    QVariant jsonStreamToVariant(QIODevice* device, QJsonParseError* error = nullptr);

}

namespace Cbor
{

    QByteArray variantToCbor(const QVariant& variant, int opt = 0);
    void variantToCborStream(const QVariant& variant, QIODevice* device, int opt = 0);

    QVariant cborToVariant(const QByteArray& cbor, QCborParserError* error = nullptr);
    QVariant cborStreamToVariant(QIODevice* device, QCborParserError* error = nullptr);

}

}

#endif // QUTILS_H
