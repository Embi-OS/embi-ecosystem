#ifndef HASSENTITYMAPPER_H
#define HASSENTITYMAPPER_H

#include "RestModel/restmapper.h"

class HassEntityMapper: public RestMapper
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit HassEntityMapper(QObject* parent=nullptr);
};

#endif // HASSENTITYMAPPER_H
