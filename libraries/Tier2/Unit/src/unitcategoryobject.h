#ifndef UNITCATEGORYOBJECT_H
#define UNITCATEGORYOBJECT_H

#include <QModels>
#include <QDefs>

#include "unit_helpertypes.h"

class UnitTypeObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_VAR_PROPERTY(UnitCategories::Enum, category, Category, UnitCategories::NoCategory)
    Q_CONSTANT_VAR_PROPERTY(UnitTypes::Enum, type, Type, UnitTypes::NoUnit)
    Q_CONSTANT_REF_PROPERTY(QString, name, Name, "")
    Q_CONSTANT_REF_PROPERTY(QString, abbreviation, Abbreviation, "")
    Q_CONSTANT_FUZ_PROPERTY(double, ratio, Ratio, 1.0)
    Q_CONSTANT_REF_PROPERTY(QString, display, Display, "")

public:
    explicit UnitTypeObject(UnitCategories::Enum category,
                            UnitTypes::Enum type,
                            const QString& name,
                            const QString& abbreviation,
                            double ratio,
                            QObject *parent = nullptr) :
        QObject(parent),
        m_category(category),
        m_type(type),
        m_name(name),
        m_abbreviation(abbreviation),
        m_ratio(ratio),
        m_display(QString("%1 [%2]").arg(m_name, m_abbreviation))
    {
    };
};

class UnitCategoryObject : public QObjectListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("")

    Q_CONSTANT_VAR_PROPERTY(UnitCategories::Enum, category, Category, UnitCategories::NoCategory)
    Q_CONSTANT_VAR_PROPERTY(UnitTypes::Enum, defaultType, DefaultType, UnitTypes::NoUnit)

public:
    explicit UnitCategoryObject(UnitCategories::Enum category, QObject *parent = nullptr);

    Q_INVOKABLE bool checkCategoryCompatibility(const UnitCategories::Enum& category) const;
    Q_INVOKABLE bool checkTypeCompatibility(UnitTypes::Enum type) const;

    Q_INVOKABLE UnitTypes::Enum type(int index) const;
    Q_INVOKABLE QString name(UnitTypes::Enum type) const;
    Q_INVOKABLE QString abbreviation(UnitTypes::Enum type) const;
    Q_INVOKABLE double ratio(UnitTypes::Enum type) const;

    Q_INVOKABLE QString rounded(double rawValue, int decimals=-1) const;
    Q_INVOKABLE QString formatted(double rawValue, UnitTypes::Enum type, int decimals=-1) const;
    Q_INVOKABLE QString display(double rawValue, UnitTypes::Enum type, int decimals=-1) const;
    Q_INVOKABLE double convert(double value, UnitTypes::Enum fromType, UnitTypes::Enum toType) const;
    Q_INVOKABLE double convertTo(double value, UnitTypes::Enum type) const;
    Q_INVOKABLE double convertFrom(double value, UnitTypes::Enum type) const;

    Q_INVOKABLE UnitTypeObject* unitObject(UnitTypes::Enum type) const;

protected:
    void onObjectAboutToBeInserted(QObject* object, int row) override;
};

#endif // UNITCATEGORYOBJECT_H
