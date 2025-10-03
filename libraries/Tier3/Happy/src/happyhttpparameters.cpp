#include "happyhttpparameters.h"

HappyHttpParameters::HappyHttpParameters(const QHttpServerRequest &request)
{
    const QList<std::pair<QString, QString>> queryItems = request.query().queryItems();
    fromQueryItems(queryItems);
}

HappyHttpParameters::HappyHttpParameters(const QUrlQuery &query)
{
    const QList<std::pair<QString, QString>> queryItems = query.queryItems();
    fromQueryItems(queryItems);
}

void HappyHttpParameters::fromQueryItems(const QList<std::pair<QString, QString>>& queryItems)
{
    for(const std::pair<QString, QString>& queryItem: queryItems)
    {
        const QString key = queryItem.first;
        const QString value = queryItem.second;

        if(key.compare("sort")==0)
        {
            const QVariantMap parserdSorters = parseSorters(value.split(','));
            for(auto [key, value]: parserdSorters.asKeyValueRange())
                sorters.insert(key, value);
        }
        else if(key.compare("fields")==0)
        {
            fields.append(value.split(','));
        }
        else if(key.compare("omit")==0)
        {
            omit.append(value.split(','));
        }
        else if(key.compare("expand")==0)
        {
            expand.append(value.split(','));
        }
        else if(key.compare("limit")==0)
        {
            limit = value.toLongLong();
        }
        else if(key.compare("offset")==0)
        {
            offset = value.toLongLong();
        }
        else if(key.compare("per_page")==0)
        {
            perPage = value.toLongLong();
        }
        else if(key.compare("page")==0)
        {
            page = value.toLongLong();
        }
        else if(key.compare("flat")==0)
        {
            if(value.toLower()=="true" || value=="1")
                flat = true;
        }
        else if(key.compare("raw")==0)
        {
            if(value.toLower()=="true" || value=="1")
                raw = true;
        }
        else {
            const QVariantMap parserdFilters = parseFilters(key, value);
            for(auto [key, value]: parserdFilters.asKeyValueRange())
                filters.insert(key, value);
        }
    }
}

QString HappyHttpParameters::toString() const
{
    QString str;

    return str;
}

QVariant HappyHttpParameters::parseFilter(const QString& value)
{
    if(value.toLower()=="false") {
        return false;
    }
    else if(value.toLower()=="true") {
        return true;
    }
    else {
        // bool ok = false;

        // long long valLongLong = value.toLongLong(&ok);
        // if(ok)
        //     return valLongLong;

        // double valDouble = value.toDouble(&ok);
        // if(ok)
        //     return valDouble;
    }

    return value;
}

QVariantMap HappyHttpParameters::parseFilters(const QString& key, const QString& value)
{
    QVariantMap filters;

    QVariant var;
    if(value.contains(',')) {
        const QStringList values = value.split(',');
        QVariantList vars;
        for(const QString& value: values)
            vars.append(parseFilter(value));
        var = vars;
    }
    else {
        var = parseFilter(value);
    }

    filters.insert(key, var);

    return filters;
}

QVariantMap HappyHttpParameters::parseSorters(const QStringList& values)
{
    QVariantMap sorters;

    for(const QString& value: values) {
        if(value.isEmpty())
            continue;

        const QChar op = value.at(0);
        const Qt::SortOrder order = op=='-' ? Qt::DescendingOrder : Qt::AscendingOrder;

        const QString field = value.mid(1);
        if(field.isEmpty())
            continue;

        sorters.insert(field, order);
    }

    return sorters;
}
