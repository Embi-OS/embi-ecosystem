#ifndef QPASSWORDHASHER_H
#define QPASSWORDHASHER_H

#include <QDefs>

/*
 * Django-style password hasher
 *
 * Format:
 *   algorithm$iterations$salt$hash
 *
 * Example:
 *   pbkdf2_sha256$100000$abc123$base64hash
 */
class QPasswordHasher : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_WRITABLE_VAR_PROPERTY(int, iterations, Iterations, 100000)
    Q_WRITABLE_VAR_PROPERTY(int, saltLength, SaltLength, 16)
    Q_WRITABLE_VAR_PROPERTY(int, hashLength, HashLength, 32)

public:
    explicit QPasswordHasher(QObject *parent = nullptr);

    // Create a new password hash (for registration / password change)
    QString makePassword(const QString &password) const;

    // Verify a password against a stored hash (login)
    static bool checkPassword(const QString &password, const QString &storedPassword);
    static bool isPasswordHashed(const QString &password);

    static QByteArray encryptPassword(const QByteArray &password);
    static QByteArray decryptPassword(const QByteArray &encryptedPassword);
    static bool isPasswordEncrypted(const QByteArray &password);

private:
    static QString generateSalt(int saltLength);
    static QByteArray pbkdf2(const QString &password, const QString &salt, int iterations, int hashLength);

    static bool constantTimeCompare(const QByteArray &a, const QByteArray &b);
};

#endif // QPASSWORDHASHER_H
