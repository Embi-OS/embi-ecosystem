#include "qvariantreader.h"

QVariant QVariantReader::read()
{
    switch (type()) {
    case QVariantReader::List:
        return readList();
    case QVariantReader::Map:
        return readMap();
    default:
        return readValue();
        break;
    }
}
QVariantList QVariantReader::readList()
{
    QVariantList list;
    if (isLengthKnown())
        list.reserve(length());

    enterContainer();
    while (!hasError() && hasNext()) {
        QVariant value = read();
        if (hasError())
            break;
        list.append(std::move(value));
    }
    if (!hasError())
        leaveContainer();

    list.squeeze();

    return list;
}
QVariantMap QVariantReader::readMap()
{
    QVariantMap map;
    // if (isLengthKnown())
    //     map.reserve(length());

    enterContainer();
    while (!hasError() && hasNext()) {
        QString key = readString();
        if (hasError())
            break;

        QVariant value = read();
        if (hasError())
            break;

        map.insert(std::move(key), std::move(value));
    }
    if (!hasError())
        leaveContainer();

    return map;
}
