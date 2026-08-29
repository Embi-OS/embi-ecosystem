import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.Time
    defaultType: UnitTypes.Time_msec

    UnitTypeObject {
        type: UnitTypes.Time_sec
        name: qsTr("Secondes")
        abbreviation: "s"
        ratio: 0.001
    }

    UnitTypeObject {
        type: UnitTypes.Time_min
        name: qsTr("Minutes")
        abbreviation: "m"
        ratio: 0.001 / 60.0
    }

    UnitTypeObject {
        type: UnitTypes.Time_hour
        name: qsTr("Heures")
        abbreviation: "h"
        ratio: 0.001 / 3600.0
    }

    UnitTypeObject {
        type: UnitTypes.Time_msec
        name: qsTr("Millisecondes")
        abbreviation: "ms"
        ratio: 1.0
    }
}
