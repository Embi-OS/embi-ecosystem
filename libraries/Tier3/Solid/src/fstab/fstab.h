#ifndef FSTAB_H
#define FSTAB_H

#include <QDefs>

#define FSTAB_FILE_PATH (QStringLiteral("/etc/fstab"))

#define FTP_ROOT_MOUNT_PATH (QStringLiteral("/mnt/ftp/"))
#define SFTP_ROOT_MOUNT_PATH (QStringLiteral("/mnt/sftp/"))
#define SMB_ROOT_MOUNT_PATH (QStringLiteral("/mnt/smb/"))

Q_ENUM_CLASS(SMBVersions, SMBVersion,
             Vers_1_0 = 0,
             Vers_2_0 = 1,
             Vers_2_1 = 2,
             Vers_3_0 = 3,)

Q_ENUM_CLASS(SMBIdentifications, SMBIdentification,
             Password,
             Guest,
             Anonym)

Q_ENUM_CLASS(FstabEntryTypes, FstabEntryType,
             DeviceNode,
             UUID,
             Label,
             PartLabel,
             PartUUID,
             Comment,
             Other)

struct FstabEntry
{
    Q_GADGET
    QML_VALUE_TYPE(fstabEntry)

    Q_PROPERTY(QString fsSpec READ fsSpec WRITE setFsSpec FINAL)
    Q_PROPERTY(QString mountPoint READ mountPoint WRITE setMountPoint FINAL)
    Q_PROPERTY(QString type READ type WRITE setType FINAL)
    Q_PROPERTY(QStringList options READ options WRITE setOptions FINAL)
    Q_PROPERTY(QString optionsString READ optionsString FINAL)
    Q_PROPERTY(int dumpFreq READ dumpFreq WRITE setDumpFreq FINAL)
    Q_PROPERTY(int passNumber READ passNumber WRITE setPassNumber FINAL)
    Q_PROPERTY(QString comment READ comment FINAL)
    Q_PROPERTY(FstabEntryTypes::Enum entryType READ entryType FINAL)

public:
    FstabEntry() = default;
    ~FstabEntry() = default;
    Q_INVOKABLE FstabEntry(const FstabEntry &other) = default;
    FstabEntry(const QString& fsSpec, const QString& mountPoint, const QString& type, const QString& options, int dumpFreq = 0, int passNumber = 0, const QString& comment = QString());

    bool isValid() const;

    const QString& fsSpec() const;
    void setFsSpec(const QString& s);

    const QString& mountPoint() const;
    void setMountPoint(const QString& s);

    const QString& type() const;
    void setType(const QString& s);

    const QStringList& options() const;
    void setOptions(const QStringList& s);
    const QString optionsString() const;

    int dumpFreq() const;
    void setDumpFreq(int s);

    int passNumber() const;
    void setPassNumber(int s);

    const QString& comment() const;

    FstabEntryType entryType() const;

    QVariantMap toSmbParams() const;
    static FstabEntry fromSmbParams(const QVariantMap& map, bool createCredentials);

    static QVariantMap parseOptions(const QStringList& options);
    static QStringList serializeOptions(const QVariantMap& options);

    static QList<FstabEntry> entries(const QString& fstabPath = FSTAB_FILE_PATH);
    static int indexOf(const QList<FstabEntry>& entries, const QString& mountPoint);
    static QString generateFstab(const QList<FstabEntry>& entries);
    static bool writeMountpoints(const QList<FstabEntry>& entries, const QString& fstabPath = FSTAB_FILE_PATH);
    static bool createCredentialFile(const QString& filePath, const QString& username, const QString& password);

private:
    QString m_fsSpec;
    QString m_mountPoint;
    QString m_type;
    QStringList m_options;
    int m_dumpFreq;
    int m_passNumber;
    QString m_comment;
    FstabEntryType m_entryType;
};
Q_DECLARE_METATYPE(FstabEntry)

QDebug operator<<(QDebug dbg, const FstabEntry &data);

#endif // FSTAB_H
