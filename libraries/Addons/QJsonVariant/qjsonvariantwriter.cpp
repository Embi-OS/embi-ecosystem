#include "qjsonvariantwriter.h"
#include <QBuffer>
#include <QByteArrayView>
#include <QIODevice>
#include <QLocale>
#include <QJsonValue>
#include <limits>

#include "qutf8.h"

static bool writeJson(QIODevice *d, QByteArrayView data)
{
    return d->write(data.data(), data.size()) == data.size();
}

static bool variantToJson(const QVariant &value, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType);

static inline bool stringToJson(QStringView string, QIODevice *d)
{
    return writeJson(d, "\"")
            && writeJson(d, QUtf8::escapedString(string))
            && writeJson(d, "\"");
}
static inline bool stringToJson(QLatin1StringView string, QIODevice *d)
{
    return stringToJson(QString::fromLatin1(string.data(), string.size()), d);
}
static inline bool stringToJson(QUtf8StringView string, QIODevice *d)
{
    return stringToJson(QString::fromUtf8(string.data(), string.size()), d);
}

static inline bool startArray(QIODevice *d, int& indent, bool compact)
{
    indent = indent + (compact ? 0 : 1);
    return writeJson(d, compact ? "[" : "[\n");
}
static inline bool endArray(QIODevice *d, int& indent, bool compact)
{
    indent = indent - (compact ? 0 : 1);
    return writeJson(d, QByteArray(4*indent, ' '))
            && writeJson(d, (compact || indent) ? "]" : "]\n");
}
template<typename T>
static inline bool variantListToJson(const QList<T>& array, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType)
{
    QByteArray indentString(4*indent, ' ');
    qsizetype i = 0;
    for(const T& variant: array) {
        if (!writeJson(d, indentString)
                || !variantToJson(variant, d, indent, compact, doublePrecision, showType))
            return false;
        if (++i == array.size())
            return compact || writeJson(d, "\n");
        if (!writeJson(d, compact ? "," : ",\n"))
            return false;
    }
    return true;
}

static inline bool startMap(QIODevice *d, int& indent, bool compact)
{
    indent = indent + (compact ? 0 : 1);
    return writeJson(d, compact ? "{" : "{\n");
}
static inline bool endMap(QIODevice *d, int& indent, bool compact)
{
    indent = indent - (compact ? 0 : 1);
    return writeJson(d, QByteArray(4*indent, ' '))
            && writeJson(d, (compact || indent) ? "}" : "}\n");
}
template<typename T>
static inline bool variantObjectToJson(const T& object, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType)
{
    QByteArray indentString(4*indent, ' ');
    qsizetype i = 0;
    auto it = object.begin();
    auto end = object.end();
    for ( ; it != end; ++it) {
        if (!writeJson(d, indentString)
                || !stringToJson(it.key(), d)
                || !writeJson(d, compact ? ":" : ": ")
                || !variantToJson(it.value(), d, indent, compact, doublePrecision, showType))
            return false;
        if (++i == object.size())
            return compact || writeJson(d, "\n");
        if (!writeJson(d, compact ? "," : ",\n"))
            return false;
    }
    return true;
}
static inline bool variantValueToJson(const QVariant &value, QIODevice *d, int doublePrecision)
{
    switch (value.metaType().id()) {
    case QMetaType::Bool:
        return writeJson(d, value.toBool() ? "true" : "false");
    case QMetaType::Short:
    case QMetaType::UShort:
    case QMetaType::Int:
    case QMetaType::LongLong:
    case QMetaType::Long:
    case QMetaType::UInt:
        return writeJson(d, QByteArray::number(value.toLongLong()));
    case QMetaType::ULong:
    case QMetaType::ULongLong:
        if (value.toULongLong() <= static_cast<uint64_t>((std::numeric_limits<qint64>::max)()))
            return writeJson(d, QByteArray::number(value.toULongLong()));
        Q_FALLTHROUGH();
    case QMetaType::Float16:
    case QMetaType::Float:
    case QMetaType::Double: {
        const double val = value.toDouble();
        if (qIsFinite(val))
            return writeJson(d, QByteArray::number(val, 'g', doublePrecision));
        return writeJson(d, "null"); // +INF || -INF || NaN (see RFC4627#section2.4)
    }
    case QMetaType::QDateTime: {
        const QDateTime val = value.toDateTime().toUTC();
        return stringToJson(val.toString(Qt::ISODate), d);
    }
    case QMetaType::Nullptr:
    case QMetaType::QString:
    default:
        if(value.isNull() || !value.isValid())
            return writeJson(d, "null");
        return stringToJson(value.toString(), d);
    }
}
bool variantToJson(const QVariant &value, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType)
{
    bool success;
    // The exact metatype makes constData safe and avoids temporary shared containers.
    switch (value.metaType().id()) {
    case QMetaType::QStringList:
        success = startArray(d, indent, compact)
                && variantListToJson(*static_cast<const QStringList *>(value.constData()), d, indent, compact, doublePrecision, showType)
                && endArray(d, indent, compact);
        break;
    case QMetaType::QVariantList:
        success = startArray(d, indent, compact)
                && variantListToJson(*static_cast<const QVariantList *>(value.constData()), d, indent, compact, doublePrecision, showType)
                && endArray(d, indent, compact);
        break;
    case QMetaType::QVariantMap:
        success = startMap(d, indent, compact)
                && variantObjectToJson(*static_cast<const QVariantMap *>(value.constData()), d, indent, compact, doublePrecision, showType)
                && endMap(d, indent, compact);
        break;
    case QMetaType::QVariantHash:
        success = startMap(d, indent, compact)
                && variantObjectToJson(*static_cast<const QVariantHash *>(value.constData()), d, indent, compact, doublePrecision, showType)
                && endMap(d, indent, compact);
        break;
    default:
        success = variantValueToJson(value, d, doublePrecision);
        break;
    }

    if (!success)
        return false;
    if(showType) {
        return writeJson(d, compact ? "" : " ")
                && writeJson(d, QString("(%1)").arg(value.metaType().name()).toUtf8());
    }
    return true;
}

QJsonVariantWriter::QJsonVariantWriter(QIODevice *device, bool compact, int doublePrecision):
    m_device(device),
    m_deleteDevice(false),
    m_showType(false),
    m_writeError(true),
    m_compact(compact),
    m_doublePrecision(doublePrecision),
    m_indent(0),
    m_nestLevel(0)
{
}

QJsonVariantWriter::QJsonVariantWriter(QByteArray *data, bool compact, int doublePrecision):
    QJsonVariantWriter(new QBuffer(data), compact, doublePrecision)
{
    m_device->open(QIODevice::WriteOnly);
    m_deleteDevice = true;
}

QJsonVariantWriter::~QJsonVariantWriter()
{
    if (m_deleteDevice)
        delete m_device;
}

void QJsonVariantWriter::start()
{
    m_showType = false;
    m_writeError = false;
    if (!m_device) {
        m_writeError = true;
    } else if (m_device->isOpen()) {
        m_writeError = !m_device->isWritable();
    } else {
        m_writeError = !m_device->open(QIODevice::WriteOnly);
    }
    m_indent = 0;
}
void QJsonVariantWriter::startArray()
{
    if (m_writeError)
        return;
    m_writeError = !::startArray(m_device, m_indent, m_compact);
}
void QJsonVariantWriter::endArray()
{
    if (m_writeError)
        return;
    m_writeError = !::endArray(m_device, m_indent, m_compact);
}
void QJsonVariantWriter::startMap()
{
    if (m_writeError)
        return;
    m_writeError = !::startMap(m_device, m_indent, m_compact);
}
void QJsonVariantWriter::endMap()
{
    if (m_writeError)
        return;
    m_writeError = !::endMap(m_device, m_indent, m_compact);
}

void QJsonVariantWriter::writeKeyValue(QLatin1StringView key, const QVariant& value)
{
    // TODO: if is not first key/value pair -> writeValueSeparator
    writeString(key);
    writeNameSeparator();
    writeVariant(value);
}
void QJsonVariantWriter::writeKeyValue(QStringView key, const QVariant& value)
{
    // TODO: if is not first key/value pair -> writeValueSeparator
    writeString(key);
    writeNameSeparator();
    writeVariant(value);
}
void QJsonVariantWriter::writeKeyValue(QUtf8StringView key, const QVariant& value)
{
    // TODO: if is not first key/value pair -> writeValueSeparator
    writeString(key);
    writeNameSeparator();
    writeVariant(value);
}
void QJsonVariantWriter::writeNameSeparator()
{
    if (m_writeError)
        return;
    writeRaw(m_compact ? ":" : ": ");
}
void QJsonVariantWriter::writeValueSeparator()
{
    if (m_writeError)
        return;
    writeRaw(m_compact ? "," : ",\n");
}

void QJsonVariantWriter::writeString(QLatin1StringView s)
{
    if (m_writeError)
        return;
    m_writeError = !::stringToJson(s, m_device);
}
void QJsonVariantWriter::writeString(QStringView s)
{
    if (m_writeError)
        return;
    m_writeError = !::stringToJson(s, m_device);
}
void QJsonVariantWriter::writeString(QUtf8StringView s)
{
    if (m_writeError)
        return;
    m_writeError = !::stringToJson(s, m_device);
}
void QJsonVariantWriter::writeRaw(const char *data, qint64 len)
{
    if (!data || m_writeError)
        return;
    if (m_device->write(data, len) != len)
        m_writeError = true;
}
void QJsonVariantWriter::writeRaw(const char *data)
{
    if (!data || m_writeError)
        return;
    writeRaw(data, qstrlen(data));
}
void QJsonVariantWriter::writeRaw(const QByteArray &data)
{
    if (m_writeError)
        return;
    if (m_device->write(data) != data.size())
        m_writeError = true;
}
void QJsonVariantWriter::writeVariant(const QVariant &v)
{
    // TODO: if is not first value in list -> writeValueSeparator
    if (m_writeError)
        return;
    m_writeError = !::variantToJson(v, m_device, m_indent, m_compact, m_doublePrecision, m_showType);
}

QByteArray QJsonVariantWriter::fromVariant(const QVariant& variant, bool compact, int doublePrecision)
{
    QByteArray json;
    QBuffer buffer(&json);
    QJsonVariantWriter writer(&buffer, compact, doublePrecision);

    writer.start();
    writer.writeVariant(variant);

    return json;
}

bool QJsonVariantWriter::fromVariant(const QVariant& variant, QIODevice* device, bool compact, int doublePrecision)
{
    QJsonVariantWriter writer(device, compact, doublePrecision);

    writer.start();
    writer.writeVariant(variant);
    return !writer.hasError();
}

QByteArray QJsonVariantWriter::escapedString(QStringView s)
{
    return QUtf8::escapedString(s);
}

QByteArray QJsonVariantWriter::fromVariantDebug(const QVariant& variant, bool compact)
{
    QByteArray json;
    QJsonVariantWriter writer(&json, compact);

    writer.start();
    writer.m_showType = true;
    writer.writeRaw("\n");
    writer.writeRaw(QByteArray(100,'-'));
    writer.writeRaw("\n");
    writer.writeVariant(variant);
    writer.writeRaw("\n");
    writer.writeRaw(QByteArray(100,'-'));

    json.squeeze();

    return json;
}
