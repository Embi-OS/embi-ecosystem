#ifndef QJSONMAPPER_H
#define QJSONMAPPER_H

#include "qvariantmapper.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>
#include <QDir>

class QJsonMapper: public QVariantMapper
{
public:
    enum JsonFormat {
        Indented = QJsonDocument::Indented,
        Compact = QJsonDocument::Compact
    };
    Q_ENUM (JsonFormat)

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(JsonMapper)

    Q_WRITABLE_REF_PROPERTY(QString, jsonPath, JsonPath, "")
    Q_WRITABLE_VAR_PROPERTY(QJsonMapper::JsonFormat, jsonFormat, JsonFormat, QJsonMapper::Indented)

    Q_PROPERTY(QString json READ getJson WRITE setJson NOTIFY jsonChanged FINAL)

public:
    explicit QJsonMapper(QObject *parent = nullptr);
    explicit QJsonMapper(const QString& baseName, QObject *parent = nullptr);
    explicit QJsonMapper(const QString& jsonPath, const QString& baseName, QObject *parent = nullptr);
    ~QJsonMapper() override;

    const QString& getJson() const;

public slots:
    void setJson(const QString& json);

signals:
    void jsonChanged(const QString& json);

protected:
    bool doSelect() final override;
    bool doSubmit(const QStringList& dirtyKeys) final override;

    QVariantMap selectJson(bool* result=nullptr);
    QVariantMap fromFile(bool* result=nullptr);
    QVariantMap fromJson(const QString& json, bool* result=nullptr);

    bool submitJson(const QStringList& dirtyKeys);
    bool toFile(const QString& json);

    QString m_json;
};

#endif // QJSONMAPPER_H
