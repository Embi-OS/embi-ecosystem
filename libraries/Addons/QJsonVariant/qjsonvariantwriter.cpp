#include "qjsonvariantwriter.h"
#include <QBuffer>
#include <QIODevice>
#include <QLocale>
#include <QJsonValue>
#include <limits>

#include "qutf8.h"

static void variantToJson(const QVariant &value, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType);

static inline void stringToJson(QLatin1StringView string, QIODevice *d)
{
    d->write("\"");
    d->write(string.data(), string.size());
    d->write("\"");
}
static inline void stringToJson(QStringView string, QIODevice *d)
{
    d->write("\"");
    d->write(QUtf8::escapedString(string));
    d->write("\"");
}
static inline void stringToJson(QUtf8StringView string, QIODevice *d)
{
    d->write("\"");
    d->write(string.data(), string.size());
    d->write("\"");
}

static inline void startArray(QIODevice *d, int& indent, bool compact)
{
    d->write(compact ? "[" : "[\n");
    indent = indent + (compact ? 0 : 1);
}
static inline void endArray(QIODevice *d, int& indent, bool compact)
{
    indent = indent - (compact ? 0 : 1);
    d->write(QByteArray(4*indent, ' '));
    d->write((compact || indent) ? "]" : "]\n");
}
template<typename T>
static inline void variantListToJson(const QList<T>& array, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType)
{
    QByteArray indentString(4*indent, ' ');
    qsizetype i = 0;
    for(const T& variant: array) {
        d->write(indentString);
        variantToJson(variant, d, indent, compact, doublePrecision, showType);
        if (++i == array.size()) {
            if (!compact)
                d->write("\n");
            break;
        }
        d->write(compact ? "," : ",\n");
    }
}

static inline void startMap(QIODevice *d, int& indent, bool compact)
{
    d->write(compact ? "{" : "{\n");
    indent = indent + (compact ? 0 : 1);
}
static inline void endMap(QIODevice *d, int& indent, bool compact)
{
    indent = indent - (compact ? 0 : 1);
    d->write(QByteArray(4*indent, ' '));
    d->write((compact || indent) ? "}" : "}\n");
}
template<typename T>
static inline void variantObjectToJson(const T& object, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType)
{
    QByteArray indentString(4*indent, ' ');
    qsizetype i = 0;
    auto it = object.begin();
    auto end = object.end();
    for ( ; it != end; ++it) {
        d->write(indentString);
        stringToJson(it.key(), d);
        d->write(compact ? ":" : ": ");
        variantToJson(it.value(), d, indent, compact, doublePrecision, showType);
        if (++i == object.size()) {
            if (!compact)
                d->write("\n");
            break;
        }
        d->write(compact ? "," : ",\n");
    }
}
static inline void variantValueToJson(const QVariant &value, QIODevice *d, int doublePrecision)
{
    switch (value.metaType().id()) {
    case QMetaType::Bool:
        if(value.toBool())
            d->write("true");
        else
            d->write("false");
        break;
    case QMetaType::Short:
    case QMetaType::UShort:
    case QMetaType::Int:
    case QMetaType::LongLong:
    case QMetaType::Long:
    case QMetaType::UInt:
        d->write(QByteArray::number(value.toLongLong()));
        break;
    case QMetaType::ULong:
    case QMetaType::ULongLong:
        if (value.toULongLong() <= static_cast<uint64_t>((std::numeric_limits<qint64>::max)())) {
            d->write(QByteArray::number(value.toULongLong()));
            break;
        }
        Q_FALLTHROUGH();
    case QMetaType::Float16:
    case QMetaType::Float:
    case QMetaType::Double: {
        const double val = value.toDouble();
        if (qIsFinite(val))
            d->write(QByteArray::number(val, 'g', doublePrecision));
        else
            d->write("null"); // +INF || -INF || NaN (see RFC4627#section2.4)
        break;
    }
    case QMetaType::QDateTime: {
        const QDateTime val = value.toDateTime().toUTC();
        stringToJson(val.toString(Qt::ISODate), d);
        break;
    }
    case QMetaType::Nullptr:
    case QMetaType::QString:
    default:
        if(value.isNull() || !value.isValid()) {
            d->write("null");
            break;
        }
        stringToJson(value.toString(), d);
        break;
    }
}
void variantToJson(const QVariant &value, QIODevice *d, int indent, bool compact, int doublePrecision, bool showType)
{
    switch (value.metaType().id()) {
    case QMetaType::QStringList: {
        startArray(d, indent, compact);
        variantListToJson(value.toStringList(), d, indent, compact, doublePrecision, showType);
        endArray(d, indent, compact);
        break;
    }
    case QMetaType::QVariantList: {
        startArray(d, indent, compact);
        variantListToJson(value.toList(), d, indent, compact, doublePrecision, showType);
        endArray(d, indent, compact);
        break;
    }
    case QMetaType::QVariantMap: {
        startMap(d, indent, compact);
        variantObjectToJson(value.toMap(), d, indent, compact, doublePrecision, showType);
        endMap(d, indent, compact);
        break;
    }
    case QMetaType::QVariantHash: {
        startMap(d, indent, compact);
        variantObjectToJson(value.toHash(), d, indent, compact, doublePrecision, showType);
        endMap(d, indent, compact);
        break;
    }
    default: {
        variantValueToJson(value, d, doublePrecision);
        break;
    }
    }

    if(showType) {
        d->write(compact ? "" : " ");
        d->write(QString("(%1)").arg(value.metaType().name()).toUtf8());
    }
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
    m_device->open(QIODevice::WriteOnly | QIODevice::Unbuffered);
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
        m_writeError = !m_device->open(QIODevice::WriteOnly | QIODevice::Unbuffered);
    }
    m_indent = 0;
}
void QJsonVariantWriter::startArray()
{
    if (m_writeError)
        return;
    ::startArray(m_device, m_indent, m_compact);
}
void QJsonVariantWriter::endArray()
{
    if (m_writeError)
        return;
    ::endArray(m_device, m_indent, m_compact);
}
void QJsonVariantWriter::startMap()
{
    if (m_writeError)
        return;
    ::startMap(m_device, m_indent, m_compact);
}
void QJsonVariantWriter::endMap()
{
    if (m_writeError)
        return;
    ::endMap(m_device, m_indent, m_compact);
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
    m_device->write(m_compact ? ":" : ": ");
}
void QJsonVariantWriter::writeValueSeparator()
{
    if (m_writeError)
        return;
    m_device->write(m_compact ? "," : ",\n");
}

void QJsonVariantWriter::writeString(QLatin1StringView s)
{
    if (m_writeError)
        return;
    ::stringToJson(s, m_device);
}
void QJsonVariantWriter::writeString(QStringView s)
{
    if (m_writeError)
        return;
    ::stringToJson(s, m_device);
}
void QJsonVariantWriter::writeString(QUtf8StringView s)
{
    if (m_writeError)
        return;
    ::stringToJson(s, m_device);
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
    if (m_device->write(data) < 0)
        m_writeError = true;
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
    ::variantToJson(v, m_device, m_indent, m_compact, m_doublePrecision, m_showType);
}

QByteArray QJsonVariantWriter::fromVariant(const QVariant& variant, bool compact, int doublePrecision)
{
    QByteArray json;
    QJsonVariantWriter writer(&json, compact, doublePrecision);

    writer.start();
    writer.writeVariant(variant);

    json.squeeze();

    return json;
}

void QJsonVariantWriter::fromVariant(const QVariant& variant, QIODevice* device, bool compact, int doublePrecision)
{
    QJsonVariantWriter writer(device, compact, doublePrecision);

    writer.start();
    writer.writeVariant(variant);
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
