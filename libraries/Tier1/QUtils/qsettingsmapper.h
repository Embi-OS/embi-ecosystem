#ifndef QSETTINGSMAPPER_H
#define QSETTINGSMAPPER_H

#include "qvariantmapper.h"
#include <QSettings>

class QSettingsMapper: public QVariantMapper
{
    Q_OBJECT
    QML_NAMED_ELEMENT(SettingsMapper)

    Q_WRITABLE_REF_PROPERTY(QString, settingsPath, SettingsPath, "")
    Q_WRITABLE_REF_PROPERTY(QString, settingsCategory, SettingsCategory, "")

public:
    explicit QSettingsMapper(QObject *parent = nullptr);
    explicit QSettingsMapper(const QString& baseName, QObject *parent = nullptr);
    explicit QSettingsMapper(const QString& settingsPath, const QString& baseName, QObject *parent = nullptr);
    explicit QSettingsMapper(const QString& settingsPath, const QString& baseName, const QString& settingsCategory, QObject *parent = nullptr);
    ~QSettingsMapper() override;

    static void setDefaultPath(const QString& path);
    static void setDefaultName(const QString& name);
    static const QString& defaultPath();
    static const QString& defaultName();

protected:
    bool doSelect() final override;
    bool doSubmit(const QStringList& dirtyKeys) final override;

    QVariantMap selectSettings(bool* result=nullptr);
    QVariantMap fromFile(bool* result=nullptr);

    bool submitSettings(const QStringList& dirtyKeys);
    bool toFile(const QVariantMap& map);

private:
    QSettings *instance();
    QPointer<QSettings> m_settings = nullptr;
};

#endif // QSETTINGSMAPPER_H
