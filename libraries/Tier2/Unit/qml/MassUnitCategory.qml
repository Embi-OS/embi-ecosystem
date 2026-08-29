import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.Mass
    defaultType: UnitTypes.Mass_gr

    UnitTypeObject {
        type: UnitTypes.Mass_gr
        name: qsTr("Grams")
        abbreviation: "gr"
        ratio: 1.0
    }

    UnitTypeObject {
        type: UnitTypes.Mass_kg
        name: qsTr("Kilograms")
        abbreviation: "kg"
        ratio: 0.001
    }
}
