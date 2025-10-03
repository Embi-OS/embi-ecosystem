#ifndef SQLDBPROFILE_H
#define SQLDBPROFILE_H

#include <QDefs>

class SqlDbProfile
{
    Q_GADGET
    QML_VALUE_TYPE(sqlDbProfile)
    QML_STRUCTURED_VALUE

    Q_PROPERTY(QString type MEMBER type FINAL)
    Q_PROPERTY(QString connectionName MEMBER connectionName FINAL)
    Q_PROPERTY(QString path MEMBER path FINAL)
    Q_PROPERTY(QString name MEMBER name FINAL)
    Q_PROPERTY(QString userName MEMBER userName FINAL)
    Q_PROPERTY(QString password MEMBER password FINAL)
    Q_PROPERTY(QString hostName MEMBER hostName FINAL)
    Q_PROPERTY(int port MEMBER port FINAL)
    Q_PROPERTY(QString connectOptions MEMBER connectOptions FINAL)
    Q_PROPERTY(int timeout MEMBER timeout FINAL)

public:
    SqlDbProfile() = default;
    ~SqlDbProfile() = default;
    Q_INVOKABLE SqlDbProfile(const SqlDbProfile& other) = default;

    Q_INVOKABLE QString toString() const;
    Q_INVOKABLE QVariantMap toMap() const;

    QString type;
    QString connectionName;
    QString path;
    QString name;
    QString userName;
    QString password;
    QString hostName;
    int port;
    QString connectOptions;
    int timeout;

    SqlDbProfile& operator=(const SqlDbProfile &other) = default;
    bool operator==(const SqlDbProfile& rhs) const = default;
    Q_DECLARE_INSIDE_NOTEQUAL_OPERATOR(SqlDbProfile)
};

Q_DECLARE_METATYPE(SqlDbProfile)

QDebug operator<<(QDebug dbg, const SqlDbProfile& settings);

#endif // SQLDBPROFILE_H
