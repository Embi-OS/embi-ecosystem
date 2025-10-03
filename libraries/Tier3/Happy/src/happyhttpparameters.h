#ifndef HAPPYHTTPPARAMETERS_H
#define HAPPYHTTPPARAMETERS_H

#include <QDefs>
#include <QHttpServerRequest>

class HappyHttpParameters
{
    Q_GADGET
    QML_VALUE_TYPE(happyHttpParameters)
    QML_STRUCTURED_VALUE

    Q_MEMBER_PROPERTY(QVariantMap, filters, {})
    Q_MEMBER_PROPERTY(QVariantMap, sorters, {})
    Q_MEMBER_PROPERTY(QStringList, fields, {})
    Q_MEMBER_PROPERTY(QStringList, omit, {})
    Q_MEMBER_PROPERTY(QStringList, expand, {})
    Q_MEMBER_PROPERTY(int, limit, -1)
    Q_MEMBER_PROPERTY(int, offset, -1)
    Q_MEMBER_PROPERTY(int, perPage, -1)
    Q_MEMBER_PROPERTY(int, page, -1)
    Q_MEMBER_PROPERTY(bool, flat, false)
    Q_MEMBER_PROPERTY(bool, raw, false)

public:
    ~HappyHttpParameters() = default;
    HappyHttpParameters() = default;
    HappyHttpParameters(const HappyHttpParameters& other) = default;
    HappyHttpParameters(const QHttpServerRequest &request);
    HappyHttpParameters(const QUrlQuery &query);

    void fromQueryItems(const QList<std::pair<QString, QString>>& queryItems);

    Q_INVOKABLE QString toString() const;

    QVariant parseFilter(const QString& value);
    QVariantMap parseFilters(const QString& key, const QString& value);
    QVariantMap parseSorters(const QStringList& values);

    HappyHttpParameters& operator=(const HappyHttpParameters &other) = default;
    bool operator==(const HappyHttpParameters&) const = default;
    bool operator!=(const HappyHttpParameters&) const = default;
};

#endif // HAPPYHTTPPARAMETERS_H
