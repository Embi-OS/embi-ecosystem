#ifndef VERSION_H
#define VERSION_H

#include <QDefs>
#include "qsingleton.h"

class Version : public QObject,
                public QQmlSingleton<Version>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_CONSTANT_REF_PROPERTY(QVersionNumber, globalVersion, GlobalVersion, {})
    Q_CONSTANT_REF_PROPERTY(QString, versionSuffix, VersionSuffix, "Unreleased")
    Q_CONSTANT_REF_PROPERTY(QString, company, Company, "N/A")
    Q_CONSTANT_REF_PROPERTY(QString, website, Website, "N/A")
    Q_CONSTANT_REF_PROPERTY(QString, maintainer, Maintainer, "N/A")
    Q_CONSTANT_REF_PROPERTY(QString, copyright, Copyright, "N/A")
    Q_CONSTANT_REF_PROPERTY(QString, version, Version, {})

    Q_WRITABLE_REF_PROPERTY(QString, productName, ProductName, "N/A")

protected:
    friend QQmlSingleton<Version>;
    explicit Version(QObject *parent = nullptr);

public:
    Q_INVOKABLE void dumpInfos() const;
    Q_INVOKABLE QString about() const;
    Q_INVOKABLE QString aboutQt() const;
    Q_INVOKABLE static QString versionToString(const QVersionNumber& version);
};

#endif // VERSION_H
