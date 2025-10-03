#include "fstab.h"

#include <QChar>
#include <QDebug>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QTextStream>

// Based on https://github.com/KDE/kpmcore/blob/master/src/core/fstab.h

static QString escapeSpaces(const QString& mountPoint)
{
    QString tmp = mountPoint;
    tmp.replace(QStringLiteral(" "), QStringLiteral("\\040"));
    tmp.replace(QStringLiteral("\t"), QStringLiteral("\\011"));
    return tmp;
}

static QString unescapeSpaces(const QString& mountPoint)
{
    QString tmp = mountPoint;
    tmp.replace(QStringLiteral("\\040"), QStringLiteral(" "));
    tmp.replace(QStringLiteral("\\011"), QStringLiteral("\t"));
    return tmp;
}

static void parseFsSpec(const QString& fsSpec, FstabEntryType& entryType)
{
    entryType = FstabEntryTypes::Other;
    if (fsSpec.startsWith(QStringLiteral("UUID="))) {
        entryType = FstabEntryTypes::UUID;
    } else if (fsSpec.startsWith(QStringLiteral("LABEL="))) {
        entryType = FstabEntryTypes::Label;
    } else if (fsSpec.startsWith(QStringLiteral("PARTUUID="))) {
        entryType = FstabEntryTypes::UUID;
    } else if (fsSpec.startsWith(QStringLiteral("PARTLABEL="))) {
        entryType = FstabEntryTypes::Label;
    } else if (fsSpec.startsWith(QStringLiteral("/"))) {
        entryType = FstabEntryTypes::DeviceNode;
    } else if (fsSpec.isEmpty()) {
        entryType = FstabEntryTypes::Comment;
    }
}

// Used to nicely format fstab file
static std::array<unsigned int, 4> fstabColumnWidth(const QList<FstabEntry>& fstabEntries)
{
    std::array<unsigned int, 4> columnWidth;

#define FIELD_WIDTH(x) 3 + escapeSpaces(std::max_element(fstabEntries.begin(), fstabEntries.end(), [](const FstabEntry& a, const FstabEntry& b) {return escapeSpaces(a.x()).length() < escapeSpaces(b.x()).length(); })->x()).length();

    columnWidth[0] = FIELD_WIDTH(fsSpec);
    columnWidth[1] = FIELD_WIDTH(mountPoint);
    columnWidth[2] = FIELD_WIDTH(type);
    columnWidth[3] = FIELD_WIDTH(optionsString);

    return columnWidth;
}

static void writeEntry(QTextStream& s, const FstabEntry& entry, std::array<unsigned int, 4> columnWidth)
{
    if (entry.entryType() == FstabEntryTypes::Comment) {
        s << entry.comment() << "\n";
        return;
    }

    // "none" is only valid as mount point for swap partitions
    if ((entry.mountPoint().isEmpty() || entry.mountPoint() == "none") && entry.type() != QStringLiteral("swap")) {
        return;
    }

    s.setFieldAlignment(QTextStream::AlignLeft);
    s << qSetFieldWidth(columnWidth[0]) << escapeSpaces(entry.fsSpec())
      << qSetFieldWidth(columnWidth[1]) << escapeSpaces(entry.mountPoint().isEmpty() ? QStringLiteral("none") : entry.mountPoint())
      << qSetFieldWidth(columnWidth[2]) << escapeSpaces(entry.type())
      << qSetFieldWidth(columnWidth[3]) << escapeSpaces(entry.optionsString()) << qSetFieldWidth(0)
      << entry.dumpFreq() << " "
      << entry.passNumber() << " "
      << entry.comment() << "\n";
}

FstabEntry::FstabEntry(const QString& fsSpec, const QString& mountPoint, const QString& type, const QString& options, int dumpFreq, int passNumber, const QString& comment)
{
    m_fsSpec = fsSpec;
    m_mountPoint = mountPoint;
    m_type = type;
    m_dumpFreq = dumpFreq;
    m_passNumber = passNumber;
    m_comment = comment;

    m_options = options.split(QLatin1Char(','));
    m_options.removeAll(QStringLiteral("defaults"));
    parseFsSpec(m_fsSpec, m_entryType);
}

bool FstabEntry::isValid() const
{
    return !m_fsSpec.isEmpty() && !m_mountPoint.isEmpty() && !m_type.isEmpty();
}

const QString& FstabEntry::fsSpec() const
{
    return m_fsSpec;
}

void FstabEntry::setFsSpec(const QString& s)
{
    m_fsSpec = s;
    parseFsSpec(m_fsSpec, m_entryType);
}

const QString& FstabEntry::mountPoint() const
{
    return m_mountPoint;
}

void FstabEntry::setMountPoint(const QString& s)
{
    m_mountPoint = s;
}

const QString& FstabEntry::type() const
{
    return m_type;
}

void FstabEntry::setType(const QString& s)
{
    m_type = s;
}

const QStringList& FstabEntry::options() const
{
    return m_options;
}

void FstabEntry::setOptions(const QStringList& s)
{
    m_options = s;
}

const QString FstabEntry::optionsString() const
{
    return options().size() > 0 ? options().join(QLatin1Char(',')) : QStringLiteral("defaults");
}

int FstabEntry::dumpFreq() const
{
    return m_dumpFreq;
}

void FstabEntry::setDumpFreq(int s)
{
    m_dumpFreq = s;
}

int FstabEntry::passNumber() const
{
    return m_passNumber;
}

void FstabEntry::setPassNumber(int s)
{
    m_passNumber = s;
}

const QString& FstabEntry::comment() const
{
    return m_comment;
}

FstabEntryType FstabEntry::entryType() const
{
    return m_entryType;
}

QVariantMap FstabEntry::toSmbParams() const
{
    const QString hostName = m_fsSpec;
    const QString mountPath = m_mountPoint;
    QVariantMap options = parseOptions(m_options);

    SMBIdentification smbIdentification = SMBIdentifications::Anonym;
    if(options.contains("guest"))
        smbIdentification = SMBIdentifications::Guest;
    else if(options.contains("credentials") || options.contains("username"))
        smbIdentification = SMBIdentifications::Password;

    QString credentials = options.take("credentials").toString();
    QString userName = options.take("username").toString();
    QString password = options.take("password").toString();

    // Check if credentials file is specified
    if (!credentials.isEmpty()) {
        QFile file(credentials);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList parts = line.split('=');
                if (parts.size() == 2) {
                    QString key = parts.at(0).trimmed();
                    QString value = parts.at(1).trimmed();
                    if (key == "username") {
                        userName = value;
                    } else if (key == "password") {
                        password = value;
                    }
                }
            }
            file.close();
        }
    }

    QString domain = options.take("domain").toString();
    int port = options.take("port").toInt();
    if(port==0)
        port=445;

    bool nofail = options.contains("nofail");
    options.remove("nofail");
    options.remove("_netdev");
    options.remove("x-systemd.automount");

    SMBVersion smbVersion = SMBVersions::Vers_3_0;
    QString vers = options.take("vers").toString();
    if(vers=="1.0")
        smbVersion = SMBVersions::Vers_1_0;
    else if(vers=="2.0")
        smbVersion = SMBVersions::Vers_2_0;
    else if(vers=="2.1")
        smbVersion = SMBVersions::Vers_2_1;
    else if(vers=="3.0")
        smbVersion = SMBVersions::Vers_3_0;

    QStringList advancedOptions;
    for(auto [key, value]: options.asKeyValueRange())
    {
        QString options = key+"="+value.toString();
        if(options.endsWith("="))
            options.removeLast();
        advancedOptions.append(options);
    }

    QVariantMap mountParams;
    mountParams.insert("hostName", hostName);
    mountParams.insert("port", port);
    mountParams.insert("mountPath", mountPath);
    mountParams.insert("name", "");
    mountParams.insert("nofail", nofail);
    mountParams.insert("smbVersion", smbVersion);
    mountParams.insert("smbIdentification", smbIdentification);
    mountParams.insert("credentials", credentials);
    mountParams.insert("userName", userName);
    mountParams.insert("password", password);
    mountParams.insert("domain", domain);
    mountParams.insert("advancedOptions", advancedOptions.join(","));

    return mountParams;
}

FstabEntry FstabEntry::fromSmbParams(const QVariantMap& map, bool createCredentials)
{
    QString fsSpec = map.value("hostName").toString();

    SMBVersion version = SMBVersions::Vers_3_0;
    if(map.contains("smbVersion"))
        version = (SMBVersion)map.value("smbVersion").toInt();

    SMBIdentification indentification = SMBIdentifications::Anonym;
    if(map.contains("smbIdentification"))
        indentification = (SMBIdentification)map.value("smbIdentification").toInt();

    QString name = map.value("name").toString();
    if(name.isEmpty())
    {
        static const QRegularExpression regexp = QRegularExpression(R"([\s/])");
        name = fsSpec;
        name.replace(regexp, "_");
    }

    QString mountPoint = map.value("mountPath").toString();
    if(mountPoint.isEmpty())
        mountPoint = QDir(SMB_ROOT_MOUNT_PATH).absoluteFilePath(name);

    const QString type = "cifs";

    bool nofail = map.value("nofail").toBool();

    const QString options = [&]() -> QString {
        QStringList optionsList;

        if(nofail)
            optionsList<<"nofail";
        optionsList<<"_netdev";
        optionsList<<"x-systemd.automount";

        switch (version) {
        case SMBVersions::Vers_1_0:
            optionsList<<"vers=1.0";
            break;
        case SMBVersions::Vers_2_0:
            optionsList<<"vers=2.0";
            break;
        case SMBVersions::Vers_2_1:
            optionsList<<"vers=2.1";
            break;
        case SMBVersions::Vers_3_0:
            optionsList<<"vers=3.0";
            break;
        }

        QString advancedOptions = map.value("advancedOptions").toString();
        if(!advancedOptions.isEmpty())
            optionsList<<advancedOptions;

        QString port = map.value("port").toString();
        if(!port.isEmpty())
            optionsList<<"port="+port;

        QString credentials = map.value("credentials").toString();
        if(credentials.isEmpty())
            credentials = QString("/etc/.%1_creds").arg(name);
        if(createCredentials)
            QFile::remove(credentials);

        if(indentification==SMBIdentifications::Password)
        {
            QString userName = map.value("userName").toString();
            QString password = map.value("password").toString();
            if(createCredentials)
            {
                createCredentialFile(credentials, userName, password);
                optionsList<<"credentials="+credentials;
            }
            else
            {
                optionsList<<"username="+userName;
                optionsList<<"password="+password;
            }

            QString domain = map.value("domain").toString();
            if(!domain.isEmpty())
                optionsList<<"domain="+domain;
        }
        else if(indentification==SMBIdentifications::Guest)
            optionsList<<"guest";

        return optionsList.join(',');
    }();

    FstabEntry entry{fsSpec, mountPoint, type, options};

    return entry;
}

QVariantMap FstabEntry::parseOptions(const QStringList& options)
{
    QVariantMap ret;
    for(const QString& option: options) {
        const QStringList keyValuePair = option.split('=', Qt::SkipEmptyParts);
        const QString key = keyValuePair[0].trimmed();
        const QString value = keyValuePair.size() > 1 ? keyValuePair[1].trimmed() : QString();
        ret.insert(key, value);
    }

    return ret;
}

QStringList FstabEntry::serializeOptions(const QVariantMap& options)
{
    QStringList ret;
    ret.reserve(options.size());

    for(auto [key, value]: options.asKeyValueRange())
    {
        QString options = key+"="+value.toString();
        if(options.endsWith("="))
            options.removeLast();
        ret.append(options);
    }

    return ret;
}

QList<FstabEntry> FstabEntry::entries(const QString& fstabPath)
{
    QList<FstabEntry> entries;
    QFile fstabFile( fstabPath );
    if ( fstabFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        const QStringList fstabLines = QString::fromLocal8Bit(fstabFile.readAll()).split( QLatin1Char('\n') );
        for ( const QString& rawLine : fstabLines )
        {
            QString line = rawLine.trimmed();
            if ( line.startsWith( QLatin1Char('#') ) || line.isEmpty()) {
                entries.push_back( { {}, {}, {}, {}, {}, {}, line } );
                continue;
            }

            QString comment = line.section(QLatin1Char('#'), 1);
            static QRegularExpression regExp = QRegularExpression(QStringLiteral("[\\s]+"));
            QStringList splitLine = line.section(QLatin1Char('#'), 0, 0).split(regExp, Qt::SkipEmptyParts);

            // We now split the standard components of /etc/fstab entry:
            // (0) path, or UUID, or LABEL, etc,
            // (1) mount point,
            // (2) file system type,
            // (3) options,
            // (4) dump frequency (optional, defaults to 0), no comment is allowed if omitted,
            // (5) pass number (optional, defaults to 0), no comment is allowed if omitted,
            // (#) comment (optional).

            // Handle deprecated subtypes, e.g. sshfs#example. They are not relevant for partitioning, ignore them.
            if (splitLine.size() < 3) {
                continue;
            }

            auto fsSpec = unescapeSpaces(splitLine.at(0));
            auto mountPoint = unescapeSpaces(splitLine.at(1));
            auto fsType = unescapeSpaces(splitLine.at(2));
            // Options may be omitted in some rare cases like NixOS generated fstab.
            auto options = unescapeSpaces(splitLine.length() >= 4 ? splitLine.at(3) : QStringLiteral("defaults"));

            switch (splitLine.length()) {
                case 4:
                    entries.push_back( {fsSpec, mountPoint, fsType, options } );
                    break;
                case 5:
                    entries.push_back( {fsSpec, mountPoint, fsType, options, splitLine.at(4).toInt() } );
                    break;
                case 6:
                    entries.push_back( {fsSpec, mountPoint, fsType, options, splitLine.at(4).toInt(), splitLine.at(5).toInt(), comment.isEmpty() ? QString() : QLatin1Char('#') + comment } );
                    break;
                default:
                    entries.push_back( { {}, {}, {}, {}, {}, {}, QLatin1Char('#') + line } );
            }
        }

        fstabFile.close();
        if (entries.back().entryType() == FstabEntryTypes::Comment && entries.back().comment().isEmpty())
            entries.pop_back();
    }

    return entries;
}

int FstabEntry::indexOf(const QList<FstabEntry>& entries, const QString& mountPoint)
{
    int index = -1;
    if(mountPoint.isEmpty())
        return index;
    for (const FstabEntry& entry : entries) {
        index++;
        if(entry.mountPoint()==mountPoint)
            return index;
    }
    return -1;
}

QString FstabEntry::generateFstab(const QList<FstabEntry>& entries)
{
    QString fstabContents;
    QTextStream out(&fstabContents);

    std::array<unsigned int, 4> columnWidth = fstabColumnWidth(entries);

    for (const auto &e : entries)
        writeEntry(out, e, columnWidth);

    out.flush();
    return fstabContents;
}

bool FstabEntry::writeMountpoints(const QList<FstabEntry>& entries, const QString& fstabPath)
{
    auto fstab = generateFstab(entries);

    static qsizetype MiB = 1 << 20;
    if (fstab.size() > MiB) {
        qCritical()<<fstabPath<<"size limit exceeded";
        return false;
    }
    QFile fstabFile(fstabPath);

    // WriteOnly implies O_TRUNC
    auto flags = QIODevice::WriteOnly | QIODevice::Unbuffered;
    if (!fstabFile.open(flags)) {
        qCritical()<<"Could not open file"<<fstabPath<<"for writing";
        return false;
    }

    if (fstabFile.write(fstab.toLocal8Bit()) != fstab.size()) {
        qCritical()<<"Could not write to file"<<fstabPath;
        return false;
    }

    return true;
}

bool FstabEntry::createCredentialFile(const QString& filePath, const QString& username, const QString& password)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    file.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser);

    QTextStream out(&file);
    out << "username=" << username << "\n";
    out << "password=" << password << "\n";

    file.close();
    return true;
}

QDebug operator<<(QDebug dbg, const FstabEntry &data)
{
    dbg.nospace().noquote()<<"FsTabEntry("
                            <<data.fsSpec()<<", "
                            <<data.mountPoint()<<", "
                            <<data.type()<<", "
                            <<data.optionsString()<<", "
                            <<data.dumpFreq()<<", "
                            <<data.passNumber()<<")";
    return dbg.space().quote();
}
