#ifndef QENVIRONMENTVARIABLES_H
#define QENVIRONMENTVARIABLES_H

#include <QByteArray>
#include <QMap>
#include <QString>
#include <QStringList>

using QByteArrayMap = QMap<QString, QByteArray>;

namespace QUtils::EnvironmentVariables
{

QString path();

QByteArrayMap readFile(const QString& filePath, bool* ok = nullptr);
QByteArrayMap readDirectory(const QString& path, const QStringList& filters = QStringList{QStringLiteral("*.conf")});
bool writeFile(const QString& filePath, const QByteArrayMap& variables, const QStringList& managedKeys);

}

#endif // QENVIRONMENTVARIABLES_H
