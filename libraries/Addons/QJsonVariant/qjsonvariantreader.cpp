#include "qjsonvariantreader.h"
#include <QBuffer>
#include <QVariant>
#include <QJsonValue>

#include "qutf8.h"

enum {
    Space = 0x20,
    Tab = 0x09,
    LineFeed = 0x0a,
    Return = 0x0d,
    BeginArray = 0x5b,
    BeginObject = 0x7b,
    EndArray = 0x5d,
    EndObject = 0x7d,
    NameSeparator = 0x3a,
    ValueSeparator = 0x2c,
    Quote = 0x22
};

QJsonVariantReader::QJsonVariantReader(QIODevice *device):
    QJsonVariantReader(device ? device->readAll() : QByteArray())
{
    if (!device)
        setError(QJsonParseError::IllegalValue);
}

QJsonVariantReader::QJsonVariantReader(const QByteArray &data):
    m_buffer(data),
    json(m_buffer.constData()),
    ptr(m_buffer.constData()),
    end(m_buffer.constData() + m_buffer.size())
{
    m_error.error = QJsonParseError::NoError;
    m_error.offset = 0;
    skipByteOrderMark();
    skipWhitespace();
}

QJsonVariantReader::~QJsonVariantReader()
{

}

bool QJsonVariantReader::hasNext() const
{
    return m_error.error == QJsonParseError::NoError && isValid() && ptr < end;
}
bool QJsonVariantReader::next()
{
    skipWhitespace();
    return !hasError();
}

bool QJsonVariantReader::completeValue()
{
    if (!next())
        return false;
    if (m_containers.isEmpty()) {
        if (ptr < end)
            setError(QJsonParseError::GarbageAtEnd);
        return !hasError();
    }

    Container &container = m_containers.last();
    if (ptr == end) {
        setError(container.closing == EndArray ? QJsonParseError::UnterminatedArray : QJsonParseError::UnterminatedObject);
        return false;
    }
    if (container.expectingKey) {
        if (*ptr != NameSeparator) {
            setError(QJsonParseError::MissingNameSeparator);
            return false;
        }
        container.expectingKey = false;
    } else {
        if (*ptr == container.closing)
            return true;
        if (*ptr != ValueSeparator) {
            setError(QJsonParseError::MissingValueSeparator);
            return false;
        }
        container.expectingKey = container.closing == EndObject;
    }
    ++ptr;
    skipWhitespace();
    if (ptr == end || *ptr == EndArray || *ptr == EndObject || *ptr == ValueSeparator || *ptr == NameSeparator) {
        setError(QJsonParseError::IllegalValue);
        return false;
    }
    return true;
}
bool QJsonVariantReader::atEnd()
{
    return ptr >= end;
}

bool QJsonVariantReader::enterContainer()
{
    if (ptr >= end) {
        setError(QJsonParseError::IllegalValue);
        return false;
    }

    if(*ptr!=BeginArray && *ptr!=BeginObject) {
        setError(QJsonParseError::IllegalValue);
        return false;
    }
    if (hasError() || (!m_containers.isEmpty() && m_containers.last().expectingKey)) {
        setError(QJsonParseError::IllegalValue);
        return false;
    }
    constexpr qsizetype MaxNestingDepth = 1024;
    if (m_containers.size() >= MaxNestingDepth) {
        setError(QJsonParseError::DeepNesting);
        return false;
    }
    const bool object = *ptr == BeginObject;
    m_containers.append(Container{char(object ? EndObject : EndArray), object});
    ++ptr; // skip '{' or '['
    return next();
}
bool QJsonVariantReader::leaveContainer()
{
    if (ptr >= end) {
        setError(QJsonParseError::MissingObject);
        return false;
    }

    if (hasError() || m_containers.isEmpty() || *ptr != m_containers.last().closing) {
        setError(QJsonParseError::IllegalValue);
        return false;
    }
    m_containers.removeLast();
    ++ptr; // skip '}' or ']'
    return completeValue();
}

QVariantReader::Type QJsonVariantReader::type() const
{
    if (ptr >= end)
        return QJsonVariantReader::Invalid;

    switch (*ptr) {
    case BeginArray:
        return QJsonVariantReader::List;
    case BeginObject:
        return QJsonVariantReader::Map;
    case Space:
    case Tab:
    case LineFeed:
    case Return:
    case EndArray:
    case EndObject:
    case NameSeparator:
    case ValueSeparator:
        return QJsonVariantReader::Invalid;
    case Quote: // String
    case 'n': // null
    case 't': // true
    case 'f': // false
    default:
        return QJsonVariantReader::Value;
    }
}

QString QJsonVariantReader::readString()
{
    return parseString();
}

QVariant QJsonVariantReader::readValue()
{
    if (hasError() || (!m_containers.isEmpty() && m_containers.last().expectingKey)) {
        setError(QJsonParseError::IllegalValue);
        return QVariant();
    }
    if (ptr >= end) {
        setError(QJsonParseError::IllegalValue);
        return QVariant();
    }

    switch (*ptr) {
    case 'n':
        ++ptr;
        if (end - ptr < 3) {
            setError(QJsonParseError::IllegalValue);
            return QVariant();
        }
        if (*ptr++ == 'u' &&
            *ptr++ == 'l' &&
            *ptr++ == 'l') {
            completeValue();
            return QVariant::fromValue(nullptr);
        }
        setError(QJsonParseError::IllegalValue);
        return QVariant();
    case 't':
        ++ptr;
        if (end - ptr < 3) {
            setError(QJsonParseError::IllegalValue);
            return QVariant();
        }
        if (*ptr++ == 'r' &&
            *ptr++ == 'u' &&
            *ptr++ == 'e') {
            completeValue();
            return QVariant(true);
        }
        setError(QJsonParseError::IllegalValue);
        return QVariant();
    case 'f':
        ++ptr;
        if (end - ptr < 4) {
            setError(QJsonParseError::IllegalValue);
            return QVariant();
        }
        if (*ptr++ == 'a' &&
            *ptr++ == 'l' &&
            *ptr++ == 's' &&
            *ptr++ == 'e') {
            completeValue();
            return QVariant(false);
        }
        setError(QJsonParseError::IllegalValue);
        return QVariant();
    case Quote:
        return parseString();
    case ValueSeparator:
        // Essentially missing value, but after a colon, not after a comma
        // like the other MissingObject errors.
        setError(QJsonParseError::IllegalValue);
        return QVariant();
    case EndObject:
    case EndArray:
        setError(QJsonParseError::MissingObject);
        return QVariant();
    default:
        return parseNumber();
    }
}

void QJsonVariantReader::skipByteOrderMark()
{
    // eat UTF-8 byte order mark
    uchar utf8bom[3] = { 0xef, 0xbb, 0xbf };
    if (end - ptr > 3 &&
        (uchar)ptr[0] == utf8bom[0] &&
        (uchar)ptr[1] == utf8bom[1] &&
        (uchar)ptr[2] == utf8bom[2])
        ptr += 3;
}

bool QJsonVariantReader::skipWhitespace()
{
    while (ptr < end) {
        if (*ptr > Space)
            break;
        if (*ptr != Space &&
            *ptr != Tab &&
            *ptr != LineFeed &&
            *ptr != Return)
            break;
        ++ptr;
    }
    return (ptr < end);
}

QString QJsonVariantReader::parseString()
{
    if (ptr >= end) {
        setError(QJsonParseError::MissingObject);
        return QString();
    }

    if(*ptr!=Quote) {
        setError(QJsonParseError::MissingObject);
        return QString();
    }
    ++ptr;

    bool isUtf8 = true;
    const char* start = ptr;
    while (ptr < end && *ptr != '"') {
        if (static_cast<uchar>(*ptr) < 0x20) {
            setError(QJsonParseError::IllegalValue);
            return QString();
        }
        if (*ptr == '\\') {
            isUtf8 = false;
            if (++ptr == end) {
                setError(QJsonParseError::UnterminatedString);
                return QString();
            }
            if (*ptr == 'u') {
                if (end - ptr < 5) {
                    setError(QJsonParseError::IllegalEscapeSequence);
                    return QString();
                }
                for (int i = 1; i <= 4; ++i) {
                    const char c = ptr[i];
                    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
                        setError(QJsonParseError::IllegalEscapeSequence);
                        return QString();
                    }
                }
                ptr += 4;
            } else if (!QByteArrayView("\"\\/bfnrt").contains(*ptr)) {
                setError(QJsonParseError::IllegalEscapeSequence);
                return QString();
            }
        }
        ++ptr;
    }
    if (ptr < end && *ptr == '"')
        ++ptr;
    else {
        setError(QJsonParseError::UnterminatedString);
        return QString();
    }

    qsizetype len = ptr - start;
    if (!QByteArrayView(start, len - 1).isValidUtf8()) {
        setError(QJsonParseError::IllegalUTF8String);
        return QString();
    }
    completeValue();
    if(isUtf8) {
        return QString::fromUtf8(start, len-1); // exclude surrounding quotes
    }
    return QUtf8::unescapedString(QByteArrayView(start, len-1)); // exclude surrounding quotes
}

QVariant QJsonVariantReader::parseNumber()
{
    const char *start = ptr;
    bool isInt = true;

    // minus
    if (ptr < end && *ptr == '-')
        ++ptr;

    // int = zero / ( digit1-9 *DIGIT )
    if (ptr < end && *ptr == '0') {
        ++ptr;
        if (ptr < end && QUtf8::isAsciiDigit(*ptr)) {
            setError(QJsonParseError::IllegalNumber);
            return QVariant();
        }
    } else {
        if (ptr >= end || !QUtf8::isAsciiDigit(*ptr)) {
            setError(QJsonParseError::IllegalNumber);
            return QVariant();
        }
        while (ptr < end && QUtf8::isAsciiDigit(*ptr))
            ++ptr;
    }

    // frac = decimal-point 1*DIGIT
    if (ptr < end && *ptr == '.') {
        ++ptr;
        if (ptr >= end || !QUtf8::isAsciiDigit(*ptr)) {
            setError(QJsonParseError::IllegalNumber);
            return QVariant();
        }
        while (ptr < end && QUtf8::isAsciiDigit(*ptr)) {
            isInt = isInt && *ptr == '0';
            ++ptr;
        }
    }

    // exp = e [ minus / plus ] 1*DIGIT
    if (ptr < end && (*ptr == 'e' || *ptr == 'E')) {
        isInt = false;
        ++ptr;
        if (ptr < end && (*ptr == '-' || *ptr == '+'))
            ++ptr;
        if (ptr >= end || !QUtf8::isAsciiDigit(*ptr)) {
            setError(QJsonParseError::IllegalNumber);
            return QVariant();
        }
        while (ptr < end && QUtf8::isAsciiDigit(*ptr))
            ++ptr;
    }

    const QByteArray number = QByteArray::fromRawData(start, ptr - start);
    completeValue();

    if (isInt) {
        bool ok;
        qlonglong n = number.toLongLong(&ok);
        if (ok) {
            return QVariant(n);
        }
    }

    bool ok;
    double d = number.toDouble(&ok);

    if (!ok) {
        setError(QJsonParseError::IllegalNumber);
        return QVariant();
    }

    return QVariant(d);
}

void QJsonVariantReader::setError(QJsonParseError::ParseError error, qint64 offset)
{
    if (m_error.error != QJsonParseError::NoError)
        return;

    m_error.error = error;
    m_error.offset = offset >= 0 ? offset : currentOffset();
}

QVariant QJsonVariantReader::fromJson(const QByteArray& json, QJsonParseError* error)
{
    QJsonVariantReader reader(json);
    QVariant variant = reader.read();
    if (!reader.hasError()) {
        reader.skipWhitespace();
        if (!reader.atEnd())
            reader.setError(QJsonParseError::GarbageAtEnd);
    }
    if(error)
        *error = reader.error();
    return reader.hasError() ? QVariant() : variant;
}

QVariant QJsonVariantReader::fromJson(QIODevice* device, QJsonParseError* error)
{
    QJsonVariantReader reader(device);
    QVariant variant = reader.read();
    if (!reader.hasError()) {
        reader.skipWhitespace();
        if (!reader.atEnd())
            reader.setError(QJsonParseError::GarbageAtEnd);
    }
    if(error)
        *error = reader.error();
    return reader.hasError() ? QVariant() : variant;
}
