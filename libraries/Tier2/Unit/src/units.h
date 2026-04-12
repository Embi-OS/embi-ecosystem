#ifndef UNITS_H
#define UNITS_H

#include <QDefs>
#include <QUtils>

#include "unitcategoryobject.h"

class Units : public QObject,
              public QQmlSingleton<Units>
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, percentUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, torqueUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, torquePerTimeUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, angularVelocityUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, angleUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, timeUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, massUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, forceUnitModel)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, lengthUnitModel)

protected:
    friend QQmlSingleton<Units>;
    explicit Units(QObject *parent = nullptr);

public:
   Q_INVOKABLE static UnitCategoryObject* make(UnitCategories::Enum category);

};

#endif // UNITS_H
