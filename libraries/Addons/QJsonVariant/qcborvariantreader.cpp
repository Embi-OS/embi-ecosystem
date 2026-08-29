#include "qcborvariantreader.h"
#include <QCborValue>
#include <QCborStreamReader>

QCborVariantReader::QCborVariantReader(QIODevice *device):
    QCborVariantReader(device ? device->readAll() : QByteArray())
{
    if (!device)
        setError(QCborError{ QCborError::InputOutputError });
}

QCborVariantReader::QCborVariantReader(const QByteArray &data):
    m_device(new QCborStreamReader(data)),
    m_size(data.size())
{
    m_error.error = QCborError{ QCborError::NoError };
    m_error.offset = 0;
}

QCborVariantReader::~QCborVariantReader()
{
    delete m_device;
}

QCborStreamReader* QCborVariantReader::device() const
{
    return m_device;
}

bool QCborVariantReader::atEnd()
{
    return currentOffset() >= m_size;
}

bool QCborVariantReader::hasNext() const
{
    return !hasStoredError() && m_device && m_device->hasNext();
}

bool QCborVariantReader::next()
{
    if (hasStoredError() || !m_device)
        return false;

    const bool result = m_device->next();
    if (!result)
        setError(m_device->lastError());
    return result;
}

QVariantReader::Type QCborVariantReader::type() const
{
    if (hasStoredError() || !m_device)
        return QVariantReader::Invalid;

    switch (m_device->type()) {
    case QCborStreamReader::Array:
        return QVariantReader::List;
    case QCborStreamReader::Map:
        return QVariantReader::Map;
    case QCborStreamReader::Invalid:
        return QVariantReader::Invalid;
    default:
        return QVariantReader::Value;
        break;
    }
}

bool QCborVariantReader::enterContainer()
{
    if (hasStoredError() || !m_device)
        return false;

    const bool result = m_device->enterContainer();
    if (!result)
        setError(m_device->lastError());
    return result;
}

bool QCborVariantReader::leaveContainer()
{
    if (hasStoredError() || !m_device)
        return false;

    const bool result = m_device->leaveContainer();
    if (!result)
        setError(m_device->lastError());
    return result;
}

QString QCborVariantReader::readString()
{
    if (hasStoredError() || !m_device)
        return QString();

    if (!m_device->isString()) {
        setError(QCborError{ QCborError::IllegalType });
        return QString();
    }

    QString key = m_device->readAllString();
    if (m_device->lastError() != QCborError::NoError)
        setError(m_device->lastError());
    return key;
}

QVariant QCborVariantReader::readValue()
{
    if (hasStoredError() || !m_device)
        return QVariant();

    switch (m_device->type()) {
    case QCborStreamReader::ByteArray: {
        QByteArray data = m_device->readAllByteArray();
        if (m_device->lastError() != QCborError::NoError)
            setError(m_device->lastError());
        return data;
    }
    case QCborStreamReader::String: {
        QString data = m_device->readAllString();
        if (m_device->lastError() != QCborError::NoError)
            setError(m_device->lastError());
        return data;
    }
    case QCborStreamReader::Invalid:
        setError(m_device->lastError());
        return QVariant();
    default: {
        QVariant value = QCborValue::fromCbor(*m_device).toVariant();
        if (m_device->lastError() != QCborError::NoError)
            setError(m_device->lastError());
        return value;
    }
    }
}

QCborParserError QCborVariantReader::error() const
{
    if (hasStoredError())
        return m_error;

    QCborParserError error;
    error.error = m_device ? m_device->lastError() : QCborError{ QCborError::InputOutputError };
    error.offset = currentOffset();
    return error;
}

void QCborVariantReader::setError(QCborError error, qint64 offset)
{
    if (hasStoredError() || error == QCborError::NoError)
        return;

    m_error.error = error;
    m_error.offset = offset >= 0 ? offset : currentOffset();
}

QVariant QCborVariantReader::fromCbor(const QByteArray& cbor, QCborParserError* error)
{
    QCborVariantReader reader(cbor);
    QVariant variant = reader.read();
    if (!reader.hasError() && !reader.atEnd())
        reader.setError(QCborError{ QCborError::GarbageAtEnd });
    if(error)
        *error = reader.error();
    return variant;
}

QVariant QCborVariantReader::fromCbor(QIODevice* device, QCborParserError* error)
{
    QCborVariantReader reader(device);
    QVariant variant = reader.read();
    if (!reader.hasError() && !reader.atEnd())
        reader.setError(QCborError{ QCborError::GarbageAtEnd });
    if(error)
        *error = reader.error();
    return variant;
}
