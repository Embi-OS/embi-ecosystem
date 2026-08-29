import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.Angle
    defaultType: UnitTypes.Angle_deg

    UnitTypeObject {
        type: UnitTypes.Angle_deg
        name: qsTr("Degrés")
        abbreviation: "°"
        ratio: 1.0
    }

    UnitTypeObject {
        type: UnitTypes.Angle_tr
        name: qsTr("Tours")
        abbreviation: "tr"
        ratio: 1.0 / 360.0
    }

    UnitTypeObject {
        type: UnitTypes.Angle_rad
        name: qsTr("Radians")
        abbreviation: "rad"
        ratio: Math.PI / 180.0
    }
}
