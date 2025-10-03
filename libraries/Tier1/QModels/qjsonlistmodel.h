#ifndef QJSONLISTMODEL_H
#define QJSONLISTMODEL_H

#include "qvariantlistmodel.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QFile>

class QJsonListModel: public QVariantListModel
{
public:
    enum JsonFormat {
        Indented = QJsonDocument::Indented,
        Compact = QJsonDocument::Compact
    };
    Q_ENUM (JsonFormat)

private:
    Q_OBJECT
    QML_NAMED_ELEMENT(JsonListModel)

    Q_WRITABLE_REF_PROPERTY(QString, jsonPath, JsonPath, "")
    Q_WRITABLE_VAR_PROPERTY(QJsonListModel::JsonFormat, jsonFormat, JsonFormat, QJsonListModel::Indented)
    Q_READONLY_REF_PROPERTY(QString, json, Json, "")

public:
    explicit QJsonListModel(QObject * parent = nullptr);
    explicit QJsonListModel(const QString& baseName, QObject *parent = nullptr);
    explicit QJsonListModel(const QString& jsonPath, const QString& baseName, QObject *parent = nullptr);
    ~QJsonListModel() override;

protected:
    QVariantList selectSource(bool* result=nullptr) override final;
    bool doSubmit() override final;

    QVariantList fromFile(bool* result=nullptr);
    QVariantList fromJson(const QString& json, bool* result=nullptr);

    bool submitJson();
    bool toFile(const QString& json) const;
};

#endif // QJSONLISTMODEL_H
