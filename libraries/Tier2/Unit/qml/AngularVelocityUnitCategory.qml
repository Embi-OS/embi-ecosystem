import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.AngularVelocity
    defaultType: UnitTypes.AngularVelocity_tr_min

    UnitTypeObject {
        type: UnitTypes.AngularVelocity_tr_min
        name: qsTr("Tours par minutes")
        abbreviation: "tr/m"
        ratio: 1.0
    }

    UnitTypeObject {
        type: UnitTypes.AngularVelocity_tr_s
        name: qsTr("Tours par secondes")
        abbreviation: "tr/s"
        ratio: 1.0 / 60.0
    }

    UnitTypeObject {
        type: UnitTypes.AngularVelocity_deg_min
        name: qsTr("Degrés  par minutes")
        abbreviation: "°/m"
        ratio: 360.0
    }

    UnitTypeObject {
        type: UnitTypes.AngularVelocity_deg_s
        name: qsTr("Degrés par secondes")
        abbreviation: "°/s"
        ratio: 1.0 / 360.0
    }
}
