#ifndef QCBORVARIANTREADER_H
#define QCBORVARIANTREADER_H

#include "qvariantreader.h"
#include <QCborStreamReader>
#include <QCborParserError>

class QCborVariantReader: public QVariantReader
{
public:
    explicit QCborVariantReader(QIODevice *device);
    explicit QCborVariantReader(const QByteArray &data);
    virtual ~QCborVariantReader();
    Q_DISABLE_COPY(QCborVariantReader)

    QCborStreamReader* device() const;

    qint64 currentOffset() const final override { return m_device ? m_device->currentOffset() : 0; }
    qint64 totalSize() const final override { return m_size; }

    bool hasError() final override { return lastError() != QCborError::NoError; };
    bool hasNext() const final override;
    bool next() final override;
    bool atEnd() final override;

    QVariantReader::Type type() const final override;

    bool isLengthKnown() const final override { return m_device && m_device->isLengthKnown(); }
    quint64 length() const final override { return m_device ? m_device->length() : 0; }

    bool enterContainer() final override;
    bool leaveContainer() final override;

    QString readString() final override;
    QVariant readValue() final override;

    QCborError lastError() const { return error().error; }
    QCborParserError error() const;
    int errorCode() final override { return error().error; }
    QString errorString() final override { return error().errorString(); }

    static QVariant fromCbor(const QByteArray& cbor, QCborParserError* error = nullptr);
    static QVariant fromCbor(QIODevice* device, QCborParserError* error = nullptr);

private:
    bool hasStoredError() const { return m_error.error != QCborError::NoError; }
    void setError(QCborError error, qint64 offset = -1);

    QCborStreamReader *m_device;
    QCborParserError m_error;

    const qint64 m_size;
};

#endif // QCBORVARIANTREADER_H
