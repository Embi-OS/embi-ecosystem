import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.Force
    defaultType: UnitTypes.Force_N

    UnitTypeObject {
        type: UnitTypes.Force_N
        name: qsTr("Newtons")
        abbreviation: "N"
        ratio: 1.0
    }

    UnitTypeObject {
        type: UnitTypes.Force_mN
        name: qsTr("Millinewtons")
        abbreviation: "mN"
        ratio: 1000.0
    }

    UnitTypeObject {
        type: UnitTypes.Force_uN
        name: qsTr("Micronewtons")
        abbreviation: "µN"
        ratio: 1000000.0
    }
}
