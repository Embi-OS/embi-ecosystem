import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.Percent
    defaultType: UnitTypes.Percent

    UnitTypeObject {
        type: UnitTypes.Percent
        name: qsTr("Pourcent")
        abbreviation: "%"
        ratio: 1.0
    }
}
