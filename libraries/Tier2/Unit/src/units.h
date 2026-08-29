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

    Q_PROPERTY(UnitCategoryObject* percentUnitModel READ percentUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* torqueUnitModel READ torqueUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* torquePerTimeUnitModel READ torquePerTimeUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* angularVelocityUnitModel READ angularVelocityUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* angleUnitModel READ angleUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* timeUnitModel READ timeUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* massUnitModel READ massUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* forceUnitModel READ forceUnitModel CONSTANT FINAL)
    Q_PROPERTY(UnitCategoryObject* lengthUnitModel READ lengthUnitModel CONSTANT FINAL)

protected:
    friend QQmlSingleton<Units>;
    explicit Units(QObject *parent = nullptr);

public:
    Q_INVOKABLE static UnitCategoryObject* make(UnitCategories::Enum category);

    UnitCategoryObject* percentUnitModel();
    UnitCategoryObject* torqueUnitModel();
    UnitCategoryObject* torquePerTimeUnitModel();
    UnitCategoryObject* angularVelocityUnitModel();
    UnitCategoryObject* angleUnitModel();
    UnitCategoryObject* timeUnitModel();
    UnitCategoryObject* massUnitModel();
    UnitCategoryObject* forceUnitModel();
    UnitCategoryObject* lengthUnitModel();

private:
    UnitCategoryObject* loadCategory(UnitCategoryObject*& storage, const QString& qmlType);

    UnitCategoryObject* m_percentUnitModel=nullptr;
    UnitCategoryObject* m_torqueUnitModel=nullptr;
    UnitCategoryObject* m_torquePerTimeUnitModel=nullptr;
    UnitCategoryObject* m_angularVelocityUnitModel=nullptr;
    UnitCategoryObject* m_angleUnitModel=nullptr;
    UnitCategoryObject* m_timeUnitModel=nullptr;
    UnitCategoryObject* m_massUnitModel=nullptr;
    UnitCategoryObject* m_forceUnitModel=nullptr;
    UnitCategoryObject* m_lengthUnitModel=nullptr;
};

#endif // UNITS_H
