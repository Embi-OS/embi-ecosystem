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

    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, percentUnitModel, PercentUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, torqueUnitModel, TorqueUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, torquePerTimeUnitModel, TorquePerTimeUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, angularVelocityUnitModel, AngularVelocityUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, angleUnitModel, AngleUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, timeUnitModel, TimeUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, massUnitModel, MassUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, forceUnitModel, ForceUnitModel, nullptr)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, lengthUnitModel, LengthUnitModel, nullptr)

protected:
    friend QQmlSingleton<Units>;
    explicit Units(QObject *parent = nullptr);

public:
   Q_INVOKABLE static UnitCategoryObject* make(UnitCategories::Enum category);

};

#endif // UNITS_H
