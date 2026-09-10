#include "qcborvariantwriter.h"
#include <QCborStreamWriter>
#include <QCborValue>
#include <QCborArray>
#include <QCborMap>
#include <QIODevice>
#include <limits>
#include <cstring>

// QCborStreamWriter's append functions do not report device write failures.
// Latch short writes here and suppress all subsequent writes to the target.
class QCborVariantWriteDevice final : public QIODevice
{
public:
    explicit QCborVariantWriteDevice(QIODevice *target): m_target(target) {}

    bool start()
    {
        m_failed = !m_target || (m_target->isOpen() ? !m_target->isWritable()
                                                   : !m_target->open(WriteOnly));
        if (!isOpen())
            open(WriteOnly | Unbuffered);
        return !m_failed;
    }
    bool hasError() const { return m_failed; }
    bool isSequential() const override { return true; }

protected:
    qint64 readData(char *, qint64) override { return -1; }
    qint64 writeData(const char *data, qint64 size) override
    {
        if (m_failed)
            return -1;
        const qint64 written = m_target->write(data, size);
        m_failed = written != size;
        return written;
    }

private:
    QIODevice *m_target;
    bool m_failed = true;
};

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
    case QMetaType::ULong:
    case QMetaType::ULongLong:
        writer.append(value.toULongLong());
        break;
    case QMetaType::Float:
    case QMetaType::Double:
        if (opt == QCborValue::NoTransformation)
            writer.append(value.toDouble());
        else
            QCborValue(value.toDouble()).toCbor(writer, QCborValue::EncodingOptions(opt));
        break;
    default:
        QCborValue::fromVariant(value).toCbor(writer, (QCborValue::EncodingOptions)opt);
        break;
    }
}
void variantToCbor(const QVariant &value, QCborStreamWriter &writer, int opt)
{
    // The exact metatype makes constData safe and avoids temporary shared containers.
    switch (value.metaType().id()) {
    case QMetaType::QStringList: {
        variantListToCbor(*static_cast<const QStringList *>(value.constData()), writer, opt);
        break;
    }
    case QMetaType::QVariantList: {
        variantListToCbor(*static_cast<const QVariantList *>(value.constData()), writer, opt);
        break;
    }
    case QMetaType::QVariantMap: {
        variantObjectToCbor(*static_cast<const QVariantMap *>(value.constData()), writer, opt);
        break;
    }
    case QMetaType::QVariantHash: {
        variantObjectToCbor(*static_cast<const QVariantHash *>(value.constData()), writer, opt);
        break;
    }
    default: {
        variantValueToCbor(value, writer, opt);
        break;
    }
    }
}

QCborVariantWriter::QCborVariantWriter(QIODevice *device, int options):
    m_output(std::make_unique<QCborVariantWriteDevice>(device)),
    m_device(new QCborStreamWriter(m_output.get())),
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

bool QCborVariantWriter::hasError() const
{
    return m_writeError || (m_output && m_output->hasError());
}

void QCborVariantWriter::start()
{
    if (m_output) {
        m_writeError = !m_output->start();
        return;
    }
    m_writeError = false;
    QIODevice *device = m_device ? m_device->device() : nullptr;
    if (!device) {
        m_writeError = true;
    } else if (device->isOpen()) {
        m_writeError = !device->isWritable();
    } else {
        m_writeError = !device->open(QIODevice::WriteOnly);
    }
}
void QCborVariantWriter::startArray()
{
    if (hasError())
        return;
    m_device->startArray();
}
void QCborVariantWriter::startArray(quint64 count)
{
    if (hasError())
        return;
    m_device->startArray(count);
}
void QCborVariantWriter::endArray()
{
    if (hasError())
        return;
    if (!m_device->endArray())
        m_writeError = true;
}
void QCborVariantWriter::startMap()
{
    if (hasError())
        return;
    m_device->startMap();
}
void QCborVariantWriter::startMap(quint64 count)
{
    if (hasError())
        return;
    m_device->startMap(count);
}
void QCborVariantWriter::endMap()
{
    if (hasError())
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
    if (hasError())
        return;
    m_device->append(s);
}
void QCborVariantWriter::writeString(QStringView s)
{
    if (hasError())
        return;
    m_device->append(s);
}
void QCborVariantWriter::writeString(QUtf8StringView s)
{
    if (hasError())
        return;
    m_device->appendTextString(s.data(), s.size());
}
void QCborVariantWriter::writeRaw(const char *data, qint64 len)
{
    if (!data || hasError())
        return;
    QIODevice *device = m_device->device();
    if (!device || device->write(data, len) != len)
        m_writeError = true;
}
void QCborVariantWriter::writeRaw(const char *data)
{
    if (data)
        writeRaw(data, qint64(std::strlen(data)));
}
void QCborVariantWriter::writeRaw(const QByteArray &data)
{
    if (hasError())
        return;
    QIODevice *device = m_device->device();
    if (!device || device->write(data) != data.size())
        m_writeError = true;
}
void QCborVariantWriter::writeVariant(const QVariant &v)
{
    if (hasError())
        return;
    ::variantToCbor(v, *m_device, m_options);
}

QByteArray QCborVariantWriter::fromVariant(const QVariant& variant, int options)
{
    QByteArray cbor;
    QCborStreamWriter writer(&cbor);
    variantToCbor(variant, writer, options);

    return cbor;
}

bool QCborVariantWriter::fromVariant(const QVariant& variant, QIODevice* device, int options)
{
    QCborVariantWriter writer(device, options);

    writer.start();
    writer.writeVariant(variant);
    return !writer.hasError();
}
