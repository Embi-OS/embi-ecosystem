#include "sqldbprofile.h"

#include <QDebug>
#include <QVariant>

QString SqlDbProfile::toString() const
{
    QString str;
    const QVariantMap map = toMap();
    for(auto [key, value]: map.asKeyValueRange())
    {
        if(!str.isEmpty())
            str.append(", ");
        str.append(QString("%1: %2").arg(key, value.toString()));
    }
    return str;
}

QVariantMap SqlDbProfile::toMap() const
{
    QVariantMap map;
    map.insert("name", name);
    map.insert("userName", userName);
    map.insert("password", password);
    map.insert("hostName", hostName);
    map.insert("port", port);
    map.insert("connectOptions", connectOptions);
    return map;
}

QDebug operator<<(QDebug dbg, const SqlDbProfile& settings)
{
    dbg.nospace().noquote()<<"SqlDbProfile("<<settings.toString()<<")";
    return dbg.space().quote();
}
