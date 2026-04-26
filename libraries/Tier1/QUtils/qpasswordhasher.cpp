#include "qpasswordhasher.h"

#include <QCryptographicHash>
#include <QPasswordDigestor>
#include <QRandomGenerator>
#include <QStringList>

static const QString PBKDF2_ALGORITHM = "pbkdf2_sha256";
static const QByteArray ENCRYPTION_ALGORITHM = "enc_xor";
static const char ENCRYPTION_SEPARATOR = '$';
static const int IV_SIZE = 32; // random IV size

QPasswordHasher::QPasswordHasher(QObject *parent):
    QObject(parent)
{

}

QString QPasswordHasher::makePassword(const QString &password) const
{
    const QString salt = generateSalt(m_saltLength);
    const QByteArray hash = pbkdf2(password, salt, m_iterations, m_hashLength);
    const QString hashB64 = hash.toBase64(QByteArray::OmitTrailingEquals);

    QStringList result;
    result += PBKDF2_ALGORITHM;
    result += QString::number(m_iterations);
    result += salt;
    result += hashB64;

    return result.join(ENCRYPTION_SEPARATOR);
}

bool QPasswordHasher::checkPassword(const QString &password, const QString &storedPassword)
{
    const QStringList parts = storedPassword.split(ENCRYPTION_SEPARATOR);
    if (parts.size() != 4)
        return false;

    const QString algorithm = parts[0];
    const int iterations = parts[1].toInt();
    const QString salt = parts[2];
    const QByteArray storedHash = QByteArray::fromBase64(parts[3].toUtf8());

    if (algorithm != PBKDF2_ALGORITHM || iterations <= 0)
        return false;

    const QByteArray computedHash = pbkdf2(password, salt, iterations, storedHash.size());

    return constantTimeCompare(computedHash, storedHash);
}

bool QPasswordHasher::isPasswordHashed(const QString &password)
{
    const QStringList parts = password.split(ENCRYPTION_SEPARATOR);
    if (parts.size() != 4)
        return false;

    const QString algorithm = parts[0];
    if (algorithm != PBKDF2_ALGORITHM)
        return false;

    return true;
}

static QByteArray deviceKey()
{
    const QByteArray deviceId = QSysInfo::machineUniqueId();
    return QCryptographicHash::hash(deviceId, QCryptographicHash::Sha256);
}

// Encrypt password: returns IV + ciphertext as a single QByteArray
QByteArray QPasswordHasher::encryptPassword(const QByteArray &password)
{
    QByteArray key = deviceKey();
    QByteArray iv(IV_SIZE, '\0');

    // Generate random IV
    for (int i = 0; i < IV_SIZE; ++i)
    {
        iv[i] = static_cast<char>(QRandomGenerator::system()->bounded(256));
    }

    QByteArray cipher(password.size(), '\0');

    for (int i = 0; i < password.size(); ++i)
    {
        // XOR password byte with IV + key
        cipher[i] = password[i] ^ key[i % key.size()] ^ iv[i % IV_SIZE];
    }

    // Prepend IV to cipher
    const QByteArray payload = (iv + cipher).toBase64();

    QByteArrayList result;
    result += ENCRYPTION_ALGORITHM;
    result += QByteArray::number(IV_SIZE);
    result += payload;

    return result.join(ENCRYPTION_SEPARATOR);
}

// Decrypt password: input is IV+ciphertext (Base64)
QByteArray QPasswordHasher::decryptPassword(const QByteArray &encryptedPassword)
{
    if(encryptedPassword.isEmpty())
        return QByteArray();

    const QByteArrayList parts = encryptedPassword.split(ENCRYPTION_SEPARATOR);
    if (parts.size() != 3)
        return QByteArray();

    const QByteArray algorithm = parts[0];
    const int ivSize = parts[1].toInt();
    const QByteArray storedPassword = parts[2];

    if (algorithm != ENCRYPTION_ALGORITHM)
        return QByteArray();

    QByteArray data = QByteArray::fromBase64(storedPassword);
    if (data.size() < ivSize)
        return QByteArray();

    QByteArray iv = data.left(ivSize);
    QByteArray cipher = data.mid(ivSize);

    QByteArray key = deviceKey();
    QByteArray plain(cipher.size(), '\0');

    for (int i = 0; i < cipher.size(); ++i)
    {
        plain[i] = cipher[i] ^ key[i % key.size()] ^ iv[i % ivSize];
    }

    return plain;
}

bool QPasswordHasher::isPasswordEncrypted(const QByteArray &password)
{
    if(password.isEmpty())
        return false;

    const QByteArrayList parts = password.split(ENCRYPTION_SEPARATOR);
    if (parts.size() != 3)
        return false;

    const QByteArray algorithm = parts[0];
    if (algorithm != ENCRYPTION_ALGORITHM)
        return false;

    return true;
}

QString QPasswordHasher::generateSalt(int saltLength)
{
    QByteArray salt(saltLength, Qt::Uninitialized);
    QRandomGenerator *rng = QRandomGenerator::system();

    for (int i = 0; i < saltLength; ++i) {
        salt[i] = static_cast<char>(rng->generate() & 0xFF);
    }

    return salt.toBase64(QByteArray::OmitTrailingEquals);
}

QByteArray QPasswordHasher::pbkdf2(const QString &password, const QString &salt, int iterations, int hashLength)
{
    return QPasswordDigestor::deriveKeyPbkdf2(
        QCryptographicHash::Sha256,
        password.toUtf8(),
        salt.toUtf8(),
        iterations,
        hashLength
        );
}

bool QPasswordHasher::constantTimeCompare(const QByteArray &a, const QByteArray &b)
{
    if (a.size() != b.size())
        return false;

    volatile uchar diff = 0;
    for (int i = 0; i < a.size(); ++i)
        diff |= a[i] ^ b[i];

    return diff == 0;
}
