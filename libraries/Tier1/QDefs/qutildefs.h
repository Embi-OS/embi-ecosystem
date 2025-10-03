#ifndef QUTILDEFS_H
#define QUTILDEFS_H

#include <QObject>
#include <QString>
#include <QStringBuilder>
#include <QFileInfo>
#include <QPointF>
#include <QUuid>
#include <QtEndian>
#include <QtMath>
#include <QSize>
#include <QFile>
#include <QDir>
#include <QEventLoop>
#include <QReadWriteLock>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QMutexLocker>
#include <QMutex>
#include <QPainter>

//──────────────────────────────────────────────────────────────────────
// Math methods
//──────────────────────────────────────────────────────────────────────

Q_REQUIRED_RESULT inline double qFuzzyModulo(double p1, double p2, double precision=12)
{
    precision = qBound(-12.,precision,12.);
    qint64 val1 = qRound64(p1*qPow(10, precision));
    qint64 val2 = qRound64(p2*qPow(10, precision));
    return val2!=0 ? (val1 % val2)/qPow(10, precision) : qQNaN();
}

Q_REQUIRED_RESULT inline bool qFuzzyCompare(double p1, double p2, double precision)
{
    precision = qBound(-12.,precision,12.);
    qint64 val1 = qRound64(p1*qPow(10, precision));
    qint64 val2 = qRound64(p2*qPow(10, precision));
    return val1 == val2;
}

Q_REQUIRED_RESULT inline bool qFuzzyIsNull(double d, double precision)
{
    precision = qBound(-12.,precision,12.);
    qint64 val = qRound64(d*qPow(10, precision));
    return val == 0;
}

Q_REQUIRED_RESULT inline double qFuzzyRound(double d, double precision)
{
    precision = qBound(-12.,precision,12.);
    qint64 val = qRound64(d*qPow(10, precision));
    return val/qPow(10, precision);
}

template <typename T>
Q_DECL_CONSTEXPR inline int qSign(const T &a) { return (a < 0) ? -1 : 1; }

Q_REQUIRED_RESULT inline QPointF qLinearRegression(const QVector<QPointF> &data)
{
    int n = data.size();

    if (n < 2) {
        qWarning("Insufficient data points for linear regression.");
        return QPointF();
    }

    double sumX = 0;
    double sumY = 0;
    double sumXY = 0;
    double sumX2 = 0;

    // Calcul des sommes nécessaires
    for (const QPointF &point : data) {
        sumX += point.x();
        sumY += point.y();
        sumXY += point.x() * point.y();
        sumX2 += point.x() * point.x();
    }

    // Calcul des coefficients de la régression linéaire
    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - qPow(sumX, 2));
    double intercept = (sumY - slope * sumX) / n;

    return QPointF(slope, intercept);
}

Q_REQUIRED_RESULT inline double qLinearCoefficient(const QVector<QPointF> &data)
{
    int n = data.size();

    if (n < 2) {
        qWarning("Insufficient data points for linear regression.");
        return 0.0;
    }

    double sumX = 0;
    double sumY = 0;
    double sumXY = 0;
    double sumX2 = 0;
    double sumY2 = 0;

    // Calcul des sommes nécessaires
    for (const QPointF &point : data) {
        sumX += point.x();
        sumY += point.y();
        sumXY += point.x() * point.y();
        sumX2 += point.x() * point.x();
        sumY2 += point.y() * point.y();
    }

    // Calcul du coefficient de corrélation
    double correlationCoefficient = (n * sumXY - sumX * sumY) / qSqrt((n * sumX2 - qPow(sumX, 2)) * (n * sumY2 - qPow(sumY, 2)));

    return correlationCoefficient;
}

Q_REQUIRED_RESULT inline double qSigma(const QVector<double> &data)
{
    int n = data.size();

    if (n < 2) {
        qWarning("Insufficient data points for calculating sigma.");
        return 0;
    }

    double sum = 0;
    double mean = 0;

    // Calcul de la somme des valeurs
    for (double value : data) {
        sum += value;
    }

    // Calcul de la moyenne
    mean = sum / n;

    // Calcul de la somme des carrés des écarts à la moyenne
    double sumSquaredDeviations = 0.0;
    for (double value : data) {
        double deviation = value - mean;
        sumSquaredDeviations += qPow(deviation, 2);
    }

    // Calcul de l'écart type (sigma)
    double sigma = qSqrt(sumSquaredDeviations / (n - 1));

    return sigma;
}

Q_REQUIRED_RESULT inline double qAverage(const QList<double>& values)
{
    if(values.isEmpty())
        return 0;

    double sum = 0;
    for (double value : values)
        sum += value;

    return sum / values.size();
}

Q_REQUIRED_RESULT inline double qMedian(const QList<double>& values)
{
    QList<double> sortedValues = values;
    std::stable_sort(sortedValues.begin(), sortedValues.end());

    double median = 0;
    int size = sortedValues.size();
    if (size % 2 == 0) {
        // If even number of elements, take the average of the two middle values
        median = (sortedValues[size / 2 - 1] + sortedValues[size / 2]) / 2.0;
    } else {
        // If odd number of elements, take the middle value
        median = sortedValues[size / 2];
    }

    return median;
}

//──────────────────────────────────────────────────────────────────────
// QHash methods
//──────────────────────────────────────────────────────────────────────

inline void reverseStringIntHash(QHash<QString, int> &dst, const QHash<int, QByteArray> &src)
{
    dst.clear();
    dst.reserve(src.count());
    for (auto i = src.constBegin(); i != src.constEnd(); ++i) {
        dst[QString::fromUtf8(i.value())] = i.key();
    }
}

//──────────────────────────────────────────────────────────────────────
// QString methods
//──────────────────────────────────────────────────────────────────────

inline QString hex(qint64 num, int digits=0) {
    if(digits==0)
        return QStringLiteral("0x") % QString::number(num, 16).toUpper();
    return (QStringLiteral("0x") % QString::number(num, 16).rightJustified(digits, QLatin1Char ('0'), false).toUpper().right(digits));
}

inline QString hex(const QByteArray& byteArray) {
    return (QStringLiteral("0x") % byteArray.toHex().toUpper());
}

inline QString bin(qint64 num, int digits=0) {
    if(digits==0)
        return QStringLiteral("0b") % QString::number(num, 2).toUpper();
    return (QStringLiteral("0b") % QString::number(num, 2).rightJustified(digits, QLatin1Char('0'), false).right(digits));
}

inline QString bin(const QByteArray& byteArray) {
    QString bin;
    for(unsigned char byte: byteArray) {
        bin.append(QString::number(byte, 2).rightJustified(8, QLatin1Char('0'), false).right(8));
    }
    return (QStringLiteral("0b") % bin.toUpper());
}

inline QByteArray fromHex(QString byteArray) {
    if(byteArray.startsWith(QStringLiteral("0x")))
        byteArray.remove(0,2);
    else if(byteArray.startsWith(QStringLiteral("x")))
        byteArray.remove(0,1);

    return QByteArray::fromHex(byteArray.toUtf8());
}

inline qint32 intFromHex(QString byteArray) {
    if(byteArray.startsWith(QStringLiteral("0x")))
        byteArray.remove(0,2);
    else if(byteArray.startsWith(QStringLiteral("x")))
        byteArray.remove(0,1);

    return byteArray.toUtf8().toInt(0, 16);
}

inline QString bytes(qint64 num, int decimals=3, QLocale::DataSizeFormats format=QLocale::DataSizeTraditionalFormat) {
    return QLocale::c().formattedDataSize(num, decimals, format);
}

inline QString pointerString(void* ptr) {
    return QString("0x%1").arg(reinterpret_cast<quintptr>(ptr), QT_POINTER_SIZE * 2, 16, QChar('0'));
}

inline QString permissionString(const QFileInfo &fi)
{
    const QFile::Permissions permissions = fi.permissions();
    QString result = QLatin1String("----------");
    if (fi.isSymLink())
        result[0] = QLatin1Char('l');
    else if (fi.isDir())
        result[0] = QLatin1Char('d');
    if (permissions & QFileDevice::ReadUser)
        result[1] = QLatin1Char('r');
    if (permissions & QFileDevice::WriteUser)
        result[2] = QLatin1Char('w');
    if (permissions & QFileDevice::ExeUser)
        result[3] = QLatin1Char('x');
    if (permissions & QFileDevice::ReadGroup)
        result[4] = QLatin1Char('r');
    if (permissions & QFileDevice::WriteGroup)
        result[5] = QLatin1Char('w');
    if (permissions & QFileDevice::ExeGroup)
        result[6] = QLatin1Char('x');
    if (permissions & QFileDevice::ReadOther)
        result[7] = QLatin1Char('r');
    if (permissions & QFileDevice::WriteOther)
        result[8] = QLatin1Char('w');
    if (permissions & QFileDevice::ExeOther)
        result[9] = QLatin1Char('x');
    return result;
}

inline QString lsqueeze(const QString &str, int maxlen = 40){
    if (str.length() > maxlen) {
        const int part = maxlen - 3;
        return QLatin1String("...") % QStringView(str).right(part);
    } else {
        return str;
    }
}

inline QString csqueeze(const QString &str, int maxlen = 40){
    if (str.length() > maxlen && maxlen > 3) {
        const int part = (maxlen - 3) / 2;
        const QStringView strView{str};
        return strView.left(part) % QLatin1String("...") % strView.right(part);
    } else {
        return str;
    }
}

inline QString rsqueeze(const QString &str, int maxlen = 40){
    if (str.length() > maxlen) {
        const int part = maxlen - 3;
        return QStringView(str).left(part) % QLatin1String("...");
    } else {
        return str;
    }
}

inline QString obscure(const QString &str){
    QString result;
    for (const QChar ch : str) {
        // yes, no typo. can't encode ' ' or '!' because
        // they're the unicode BOM. stupid scrambling. stupid.
        const ushort uc = ch.unicode();
        result += (uc <= 0x21) ? ch : QChar(0x1001F - uc);
    }
    return result;
}

inline QString camelToSnake(const QString &input) {
    QString output;
    for (int i = 0; i < input.size(); ++i) {
        QChar c = input[i];
        // Check if the character is uppercase
        if (c.isUpper()) {
            // If it's not the first character, add an underscore before it
            if (i != 0) {
                output += '_';
            }
            // Convert to lowercase and append
            output += c.toLower();
        } else {
            // If it's already lowercase, just append
            output += c;
        }
    }
    return output;
}

inline QString snakeToCamel(const QString &input) {
    QString output;
    bool toUpper = false;
    for (int i = 0; i < input.size(); ++i) {
        QChar c = input[i];
        if (c == '_') {
            // Skip the underscore and set the flag to capitalize the next character
            toUpper = true;
        } else {
            // If the flag is set, convert the character to uppercase
            if (toUpper) {
                output += c.toUpper();
                toUpper = false;
            } else {
                // Otherwise, just append the character as is
                output += c;
            }
        }
    }
    return output;
}

//──────────────────────────────────────────────────────────────────────
// QUuid methods
//──────────────────────────────────────────────────────────────────────

inline QUuid uuidFromUInt128(__uint128_t uuid, QSysInfo::Endian order = QSysInfo::LittleEndian) {
    QUuid result = {};
    if (order == QSysInfo::BigEndian) {
        result.data1 = qFromBigEndian<quint32>(int(uuid));
        result.data2 = qFromBigEndian<quint16>(ushort(uuid >> 32));
        result.data3 = qFromBigEndian<quint16>(ushort(uuid >> 48));
        for (int i = 0; i < 8; ++i)
            result.data4[i] = uchar(uuid >> (64 + i * 8));
    } else {
        result.data1 = qFromLittleEndian<quint32>(uint(uuid >> 96));
        result.data2 = qFromLittleEndian<quint16>(ushort(uuid >> 80));
        result.data3 = qFromLittleEndian<quint16>(ushort(uuid >> 64));
        for (int i = 0; i < 8; ++i)
            result.data4[i] = uchar(uuid >> (56 - i * 8));
    }
    return result;
}

inline __uint128_t uuidToUInt128(QUuid uuid, QSysInfo::Endian order = QSysInfo::LittleEndian) {
    __uint128_t result = {};
    if (order == QSysInfo::BigEndian) {
        for (int i = 0; i < 8; ++i)
            result |= quint64(uuid.data4[i]) << (i * 8);
        result = result << 64;
        result |= quint64(qToBigEndian<quint16>(uuid.data3)) << 48;
        result |= quint64(qToBigEndian<quint16>(uuid.data2)) << 32;
        result |= qToBigEndian<quint32>(uuid.data1);
    } else {
        result = qToLittleEndian<quint32>(uuid.data1);
        result = result << 32;
        result |= quint64(qToLittleEndian<quint16>(uuid.data2)) << 16;
        result |= quint64(qToLittleEndian<quint16>(uuid.data3));
        result = result << 64;
        for (int i = 0; i < 8; ++i)
            result |= quint64(uuid.data4[i]) << (56 - i * 8);
    }
    return result;
}

//──────────────────────────────────────────────────────────────────────
// QFont methods
//──────────────────────────────────────────────────────────────────────

inline QFont adjustedFontForRect(const QString& text, const QFont& font, const QRectF& rect, QPainter* painter)
{
    int fontSize = font.pixelSize();
    if (fontSize <= 0) {
        fontSize = QFontInfo(font).pointSize() * painter->device()->logicalDpiY() / 72;
    }

    QFont adjustedFont = font;
    QFontMetrics fm(font);
    QRect boundingRect = fm.boundingRect(text);
    while ((boundingRect.width()>rect.width() || boundingRect.height() > rect.height()) && fontSize > 1)
    {
        fontSize--;
        adjustedFont.setPixelSize(fontSize);
        fm = QFontMetrics(adjustedFont);
        boundingRect = fm.boundingRect(text);
    }

    return adjustedFont;
}

//──────────────────────────────────────────────────────────────────────
// Math macros
//──────────────────────────────────────────────────────────────────────

#define ABS(N)              qAbs(N) //(((N)<0)?(-(N)):(N))
#define ROUND(N)            qRound(N)
#define SIGN(N)             qSign(N) //(((N)<0)?(-1):(1))
#define BOUND(N,V,M)        qBound(N,V,M)

#define MIN(N,M)            qMin(N,M) //(((N)<(M))?(N):(M))
#define MAX(N,M)            qMax(N,M) //(((N)>(M))?(N):(M))

#define FLOAT_NAN           std::numeric_limits<float>::quiet_NaN()
#define DOUBLE_NAN          std::numeric_limits<double>::quiet_NaN()

#define FLOAT_INF           std::numeric_limits<float>::infinity()
#define DOUBLE_INF          std::numeric_limits<double>::infinity()

#define MAX_FLOAT_VAL       std::numeric_limits<float>::max()
#define MAX_DOUBLE_VAL      std::numeric_limits<double>::max()
#define MAX_INT_VAL         std::numeric_limits<int>::max()
#define MAX_UINT_VAL        std::numeric_limits<unsigned int>::max()
#define MAX_LONG_VAL        std::numeric_limits<long long>::max()
#define MAX_ULONG_VAL       std::numeric_limits<unsigned long long>::max()

#define MIN_FLOAT_VAL       std::numeric_limits<float>::min()
#define MIN_DOUBLE_VAL      std::numeric_limits<double>::min()
#define MIN_INT_VAL         std::numeric_limits<int>::min()
#define MIN_UINT_VAL        std::numeric_limits<unsigned int>::min()
#define MIN_LONG_VAL        std::numeric_limits<long long>::min()
#define MIN_ULONG_VAL       std::numeric_limits<unsigned long long>::min()

#endif // QUTILDEFS_H
