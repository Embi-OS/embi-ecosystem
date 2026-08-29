import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.Length
    defaultType: UnitTypes.Length_mm

    UnitTypeObject {
        type: UnitTypes.Length_mm
        name: qsTr("Millimètres")
        abbreviation: "mm"
        ratio: 1.0
    }

    UnitTypeObject {
        type: UnitTypes.Length_um
        name: qsTr("Micromètres")
        abbreviation: "µm"
        ratio: 1000.0
    }

    UnitTypeObject {
        type: UnitTypes.Length_m
        name: qsTr("Mètres")
        abbreviation: "m"
        ratio: 0.001
    }
}
