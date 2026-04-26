#include "qenvironmentvariables.h"
#include "qutils_log.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

QString environmentVariableName(const QByteArray& line)
{
    const QByteArray trimmedLine = line.trimmed();
    if (!trimmedLine.startsWith(QByteArrayLiteral("env=")))
        return {};

    const qsizetype valueSeparator = trimmedLine.indexOf('=', 4);
    if (valueSeparator <= 4)
        return {};

    const QByteArray name = trimmedLine.mid(4, valueSeparator - 4);
    if (name.isEmpty())
        return {};

    return QString::fromUtf8(name);
}

void writeLine(QFile& file, const QByteArray& line)
{
    file.write(line);
    file.write(QByteArrayLiteral("\n"));
}

QString QUtils::EnvironmentVariables::path()
{
    const QString appConfigLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (appConfigLocation.isEmpty())
        return {};

    return QDir(appConfigLocation).filePath(QStringLiteral("environment"));
}

QByteArrayMap QUtils::EnvironmentVariables::readFile(const QString& filePath, bool* ok)
{
    QByteArrayMap variables;

    if (filePath.isEmpty() || !QFileInfo::exists(filePath)) {
        if (ok)
            *ok = false;
        return variables;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QUTILSLOG_WARNING() << "Cannot open environment config:" << file.fileName() << file.errorString();
        if (ok)
            *ok = false;
        return variables;
    }

    while (!file.atEnd()) {
        const QByteArray line = file.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#'))
            continue;

        const QString name = environmentVariableName(line);
        if (name.isEmpty())
            continue;

        const qsizetype valueSeparator = line.indexOf('=', 4);
        variables[name] = line.mid(valueSeparator + 1);
    }

    if (ok)
        *ok = true;

    return variables;
}

QByteArrayMap QUtils::EnvironmentVariables::readDirectory(const QString& path, const QStringList& filters)
{
    QByteArrayMap variables;

    if (path.isEmpty())
        return variables;

    QDir dir(path);
    if (!dir.exists())
        return variables;

    const QStringList files = dir.entryList(filters,
                                            QDir::Files | QDir::Readable,
                                            QDir::Name | QDir::IgnoreCase);
    for (const QString& file : files) {
        const QByteArrayMap fileVariables = readFile(dir.filePath(file));
        for (auto it = fileVariables.constBegin(); it != fileVariables.constEnd(); ++it)
            variables.insert(it.key(), it.value());
    }

    return variables;
}

bool QUtils::EnvironmentVariables::writeFile(const QString& filePath, const QByteArrayMap& variables, const QStringList& managedKeys)
{
    if (filePath.isEmpty())
        return false;

    QFile file(filePath);
    QByteArrayList lines;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QByteArray line = file.readLine();
            while (line.endsWith('\n') || line.endsWith('\r'))
                line.chop(1);

            const QString name = environmentVariableName(line);
            if (name.isEmpty() || !managedKeys.contains(name))
                lines.append(line);
        }
        file.close();
    }

    for (const QString& key : managedKeys) {
        if (!variables.contains(key))
            continue;

        lines.append(QByteArrayLiteral("env=") + key.toUtf8() + QByteArrayLiteral("=") + variables.value(key));
    }

    if (lines.isEmpty()) {
        if (QFileInfo::exists(filePath) && !QFile::remove(filePath)) {
            QUTILSLOG_WARNING() << "Cannot remove empty environment config:" << filePath;
            return false;
        }

        return true;
    }

    const QFileInfo fileInfo(filePath);
    if (!QDir().mkpath(fileInfo.absolutePath())) {
        QUTILSLOG_WARNING() << "Cannot create environment config path:" << fileInfo.absolutePath();
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QUTILSLOG_WARNING() << "Cannot open environment config:" << file.fileName() << file.errorString();
        return false;
    }

    for (const QByteArray& line : std::as_const(lines))
        writeLine(file, line);

    return true;
}
