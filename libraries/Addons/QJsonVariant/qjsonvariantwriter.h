#ifndef QJSONVARIANTWRITER_H
#define QJSONVARIANTWRITER_H

#include <QVariant>
#include <QLocale>
#include <QByteArray>

class QIODevice;
class QJsonVariantWriter
{
public:
    explicit QJsonVariantWriter(QIODevice *device, bool compact=true, int doublePrecision=QLocale::FloatingPointShortest);
    explicit QJsonVariantWriter(QByteArray *data, bool compact=true, int doublePrecision=QLocale::FloatingPointShortest);
    ~QJsonVariantWriter();
    Q_DISABLE_COPY(QJsonVariantWriter)

    void start();
    void startArray();
    void endArray();
    void startMap();
    void endMap();

    void writeKeyValue(QLatin1StringView key, const QVariant& value);
    void writeKeyValue(QStringView key, const QVariant& value);
    void writeKeyValue(QUtf8StringView key, const QVariant& value);
    void writeNameSeparator();
    void writeValueSeparator();

    void writeString(QLatin1StringView s);
    void writeString(QStringView s);
    void writeString(QUtf8StringView s);
    void writeRaw(const char *data, qint64 len);
    void writeRaw(const char *data);
    void writeRaw(const QByteArray &ba);
    void writeVariant(const QVariant &v);

    static QByteArray fromVariant(const QVariant& variant, bool compact = true, int doublePrecision = QLocale::FloatingPointShortest);
    static void fromVariant(const QVariant& variant, QIODevice* device, bool compact = true, int doublePrecision = QLocale::FloatingPointShortest);

    static QByteArray escapedString(QStringView s);
    static QByteArray fromVariantDebug(const QVariant& variant, bool compact = true);

private:
    QIODevice *m_device;
    bool m_deleteDevice;
    bool m_showType;
    bool m_writeError;

    bool m_compact;
    int m_doublePrecision;
    int m_indent;
    int m_nestLevel;
};

#endif // QJSONVARIANTWRITER_H
