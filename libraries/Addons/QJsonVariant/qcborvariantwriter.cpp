#include "qcborvariantwriter.h"
#include <QCborStreamWriter>
#include <QCborValue>
#include <QCborArray>
#include <QCborMap>
#include <QIODevice>
#include <limits>

static void variantToCbor(const QVariant &value, QCborStreamWriter &writer, int opt);

template<typename T>
static inline void variantListToCbor(const QList<T>& array, QCborStreamWriter &writer, int opt)
{
    writer.startArray(array.size());
    for(const T& variant: array) {
        variantToCbor(variant, writer, opt);
    }
    writer.endArray();
}
template<typename T>
static inline void variantObjectToCbor(const T& object, QCborStreamWriter &writer, int opt)
{
    writer.startMap(object.size());
    auto it = object.begin();
    auto end = object.end();
    for ( ; it != end; ++it) {
        writer.append(it.key());
        variantToCbor(it.value(), writer, opt);
    }
    writer.endMap();
}
static inline void variantValueToCbor(const QVariant &value, QCborStreamWriter &writer, int opt)
{
    switch (value.metaType().id()) {
    case QMetaType::QByteArray:
        writer.append(value.toByteArray());
        break;
    case QMetaType::QString:
        writer.append(value.toString());
        break;
    case QMetaType::UnknownType:
        writer.appendNull();
        break;
    case QMetaType::Nullptr:
        writer.appendUndefined();
        break;
    case QMetaType::Bool:
        writer.append(value.toBool());
        break;
    case QMetaType::Short:
    case QMetaType::UShort:
    case QMetaType::Int:
    case QMetaType::LongLong:
    case QMetaType::UInt:
        writer.append(value.toLongLong());
        break;
    case QMetaType::ULongLong:
        if (value.toULongLong() <= static_cast<uint64_t>((std::numeric_limits<qint64>::max)())) {
            writer.append(value.toLongLong());
            break;
        }
        Q_FALLTHROUGH();
    case QMetaType::Float:
    case QMetaType::Double:
        writer.append(value.toDouble());
        break;
    default:
        QCborValue::fromVariant(value).toCbor(writer, (QCborValue::EncodingOptions)opt);
        break;
    }
}
void variantToCbor(const QVariant &value, QCborStreamWriter &writer, int opt)
{
    switch (value.metaType().id()) {
    case QMetaType::QStringList: {
        variantListToCbor(value.toStringList(), writer, opt);
        break;
    }
    case QMetaType::QVariantList: {
        variantListToCbor(value.toList(), writer, opt);
        break;
    }
    case QMetaType::QVariantMap: {
        variantObjectToCbor(value.toMap(), writer, opt);
        break;
    }
    case QMetaType::QVariantHash: {
        variantObjectToCbor(value.toHash(), writer, opt);
        break;
    }
    default: {
        variantValueToCbor(value, writer, opt);
        break;
    }
    }
}

QCborVariantWriter::QCborVariantWriter(QIODevice *device, int options):
    m_device(new QCborStreamWriter(device)),
    m_writeError(true),
    m_options(options)
{

}

QCborVariantWriter::QCborVariantWriter(QByteArray *data, int options):
    m_device(new QCborStreamWriter(data)),
    m_writeError(true),
    m_options(options)
{

}

QCborVariantWriter::~QCborVariantWriter()
{
    delete m_device;
}

QCborStreamWriter* QCborVariantWriter::device() const
{
    return m_device;
}

void QCborVariantWriter::start()
{
    m_writeError = false;
    QIODevice *device = m_device ? m_device->device() : nullptr;
    if (!device) {
        m_writeError = true;
    } else if (device->isOpen()) {
        m_writeError = !device->isWritable();
    } else {
        m_writeError = !device->open(QIODevice::WriteOnly | QIODevice::Unbuffered);
    }
}
void QCborVariantWriter::startArray()
{
    if (m_writeError)
        return;
    m_device->startArray();
}
void QCborVariantWriter::startArray(quint64 count)
{
    if (m_writeError)
        return;
    m_device->startArray(count);
}
void QCborVariantWriter::endArray()
{
    if (m_writeError)
        return;
    if (!m_device->endArray())
        m_writeError = true;
}
void QCborVariantWriter::startMap()
{
    if (m_writeError)
        return;
    m_device->startMap();
}
void QCborVariantWriter::startMap(quint64 count)
{
    if (m_writeError)
        return;
    m_device->startMap(count);
}
void QCborVariantWriter::endMap()
{
    if (m_writeError)
        return;
    if (!m_device->endMap())
        m_writeError = true;
}

void QCborVariantWriter::writeKeyValue(QLatin1StringView key, const QVariant& value)
{
    writeString(key);
    writeVariant(value);
}
void QCborVariantWriter::writeKeyValue(QStringView key, const QVariant& value)
{
    writeString(key);
    writeVariant(value);
}
void QCborVariantWriter::writeKeyValue(QUtf8StringView key, const QVariant& value)
{
    writeString(key);
    writeVariant(value);
}

void QCborVariantWriter::writeString(QLatin1StringView s)
{
    if (m_writeError)
        return;
    m_device->append(s);
}
void QCborVariantWriter::writeString(QStringView s)
{
    if (m_writeError)
        return;
    m_device->append(s);
}
void QCborVariantWriter::writeString(QUtf8StringView s)
{
    if (m_writeError)
        return;
    m_device->appendTextString(s.data(), s.size());
}
void QCborVariantWriter::writeRaw(const char *data, qint64 len)
{
    if (!data || m_writeError)
        return;
    QIODevice *device = m_device->device();
    if (!device || device->write(data, len) != len)
        m_writeError = true;
}
void QCborVariantWriter::writeRaw(const char *data)
{
    if (!data || m_writeError)
        return;
    QIODevice *device = m_device->device();
    if (!device || device->write(data) < 0)
        m_writeError = true;
}
void QCborVariantWriter::writeRaw(const QByteArray &data)
{
    if (m_writeError)
        return;
    QIODevice *device = m_device->device();
    if (!device || device->write(data) != data.size())
        m_writeError = true;
}
void QCborVariantWriter::writeVariant(const QVariant &v)
{
    if (m_writeError)
        return;
    ::variantToCbor(v, *m_device, m_options);
}

QByteArray QCborVariantWriter::fromVariant(const QVariant& variant, int options)
{
    QByteArray cbor;
    QCborVariantWriter writer(&cbor, options);

    writer.start();
    writer.writeVariant(variant);

    cbor.squeeze();

    return cbor;
}

void QCborVariantWriter::fromVariant(const QVariant& variant, QIODevice* device, int options)
{
    QCborVariantWriter writer(device, options);

    writer.start();
    writer.writeVariant(variant);
}
