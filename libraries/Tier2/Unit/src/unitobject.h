#ifndef UNITOBJECT_H
#define UNITOBJECT_H

#include <QDefs>
#include <QUtils>

#include "qsignalthrottler.h"

#include "unitcategoryobject.h"
class UnitObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_VAR_PROPERTY(UnitCategories::Enum, category, Category, UnitCategories::NoCategory)
    Q_CONSTANT_PTR_PROPERTY(UnitCategoryObject, categoryObject, CategoryObject, nullptr)
    Q_CONSTANT_VAR_PROPERTY(UnitTypes::Enum, defaultType, DefaultType, UnitTypes::NoUnit)
    Q_WRITABLE_VAR_PROPERTY(UnitTypes::Enum, type, Type, UnitTypes::NoUnit)

    Q_WRITABLE_FUZ_PROPERTY(double, rawValue, RawValue, 0)
    Q_WRITABLE_VAR_PROPERTY(int, decimals, Decimals, 2)

    Q_READONLY_REF_PROPERTY(QString, displayType, DisplayType, "N/A")
    Q_READONLY_FUZ_PROPERTY(double, displayValue, DisplayValue, 0)
    Q_READONLY_REF_PROPERTY(QString, formattedValue, FormattedValue, "")
    Q_READONLY_REF_PROPERTY(QString, display, Display, "")

    Q_CONSTANT_PTR_PROPERTY(QGenericSignalThrottler, throttler, Throttler, nullptr)
    Q_PROPERTY(double rawValueThrottled READ getRawValue NOTIFY throttled FINAL)
    Q_PROPERTY(double displayValueThrottled READ getDisplayValue NOTIFY throttled FINAL)
    Q_PROPERTY(QString formattedValueThrottled READ getFormattedValue NOTIFY throttled FINAL)
    Q_PROPERTY(QString displayThrottled READ getDisplay NOTIFY throttled FINAL)

public:
    explicit UnitObject(UnitCategories::Enum category, QObject *parent = nullptr);

    Q_INVOKABLE double getValue(UnitTypes::Enum type=UnitTypes::NoUnit) const;
    Q_INVOKABLE bool setValue(double value, UnitTypes::Enum type=UnitTypes::NoUnit);

    // TODO: add operator=(), operator+(), operator-(), operator==() etc... qui agissent sur rawValue (avec setRawValue)

protected slots:
    void updateDisplay();

signals:
    void throttled();
    void displayUpdated();
};

class PercentUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit PercentUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class AngleUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit AngleUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class AngularVelocityUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit AngularVelocityUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class ForceUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit ForceUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class LengthUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit LengthUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class TimeUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit TimeUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class MassUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit MassUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class TorquePerTimeUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit TorquePerTimeUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

class TorqueUnit : public UnitObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit TorqueUnit(QObject *parent = nullptr);
    static UnitCategoryObject* makeCategory();
};

#endif // UNITOBJECT_H
