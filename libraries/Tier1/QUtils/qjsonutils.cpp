#include "qjsonutils.h"
#include <QJsonDocument>
#include <QCborStreamWriter>
#include <QCborStreamReader>
#include <QIODevice>
#include <QBuffer>
#include <QElapsedTimer>

#define JSON_VARIANT_WRITING
#define JSON_VARIANT_PARSING
#define CBOR_VARIANT_WRITING
#define CBOR_VARIANT_PARSING

#include <QJsonVariant>

QByteArray QUtils::Log::variantToLog(const QVariant& variant, bool compact)
{
    return QJsonVariantWriter::fromVariantDebug(variant, compact);
}

QByteArray QUtils::Log::variantToJson(const QVariant& variant, bool compact)
{
    return QJsonVariantWriter::fromVariant(variant, compact);
}

QByteArray QUtils::Log::variantToCbor(const QVariant& variant, int opt)
{
    return QCborVariantWriter::fromVariant(variant, opt);
}

QByteArray QUtils::Json::variantToJson(const QVariant& variant, bool compact)
{
#ifdef JSON_VARIANT_WRITING
    return QJsonVariantWriter::fromVariant(variant, compact);
#endif
    return QJsonDocument::fromVariant(variant).toJson(compact?QJsonDocument::Compact:QJsonDocument::Indented);
}

void QUtils::Json::variantToJsonStream(const QVariant& variant, QIODevice* device, bool compact)
{
#ifdef JSON_VARIANT_WRITING
    QJsonVariantWriter::fromVariant(variant, device, compact);
#endif
    device->write(QJsonDocument::fromVariant(variant).toJson(compact?QJsonDocument::Compact:QJsonDocument::Indented));
}

QVariant QUtils::Json::jsonToVariant(const QByteArray& json, QJsonParseError* error)
{
#ifdef JSON_VARIANT_PARSING
    return QJsonVariantReader::fromJson(json, error);
#endif
    return QJsonDocument::fromJson(json, error).toVariant();
}

QVariant QUtils::Json::jsonStreamToVariant(QIODevice* device, QJsonParseError* error)
{
#ifdef JSON_VARIANT_PARSING
    return QJsonVariantReader::fromJson(device, error);
#endif
    return QJsonDocument::fromJson(device->readAll(), error).toVariant();
}

QByteArray QUtils::Cbor::variantToCbor(const QVariant& variant, int opt)
{
#ifdef CBOR_VARIANT_WRITING
    return QCborVariantWriter::fromVariant(variant, opt);
#endif
    return QCborValue::fromVariant(variant).toCbor((QCborValue::EncodingOptions)opt);
}

void QUtils::Cbor::variantToCborStream(const QVariant& variant, QIODevice* device, int opt)
{
#ifdef CBOR_VARIANT_WRITING
    return QCborVariantWriter::fromVariant(variant, device, opt);
#endif
    QCborStreamWriter writer(device);
    QCborValue::fromVariant(variant).toCbor(writer, (QCborValue::EncodingOptions)opt);
}

QVariant QUtils::Cbor::cborToVariant(const QByteArray& cbor, QCborParserError* error)
{
#ifdef CBOR_VARIANT_PARSING
    return QCborVariantReader::fromCbor(cbor, error);
#endif
    return QCborValue::fromCbor(cbor).toVariant();
}

QVariant QUtils::Cbor::cborStreamToVariant(QIODevice* device, QCborParserError* error)
{
#ifdef CBOR_VARIANT_PARSING
    return QCborVariantReader::fromCbor(device, error);
#endif
    QCborStreamReader reader(device);
    return QCborValue::fromCbor(reader).toVariant();
}
