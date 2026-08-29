import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.Torque
    defaultType: UnitTypes.Torque_mNm

    UnitTypeObject {
        type: UnitTypes.Torque_mNm
        name: qsTr("Millinewtons mètres")
        abbreviation: "mNm"
        ratio: 1.0
    }

    UnitTypeObject {
        type: UnitTypes.Torque_cNm
        name: qsTr("Centinewtons mètres")
        abbreviation: "cNm"
        ratio: 0.1
    }

    UnitTypeObject {
        type: UnitTypes.Torque_Ncm
        name: qsTr("Newtons centimètres")
        abbreviation: "Ncm"
        ratio: 0.1
    }

    UnitTypeObject {
        type: UnitTypes.Torque_Nm
        name: qsTr("Newtons mètres")
        abbreviation: "Nm"
        ratio: 0.001
    }

    UnitTypeObject {
        type: UnitTypes.Torque_gmm
        name: qsTr("Grammes millimètres")
        abbreviation: "gmm"
        ratio: 101.936799185
    }

    UnitTypeObject {
        type: UnitTypes.Torque_gcm
        name: qsTr("Grammes centimètres")
        abbreviation: "gcm"
        ratio: 10.1936799185
    }

    UnitTypeObject {
        type: UnitTypes.Torque_lbfin
        name: qsTr("Pounds force inch")
        abbreviation: "lbfin"
        ratio: 0.008849558
    }

    UnitTypeObject {
        type: UnitTypes.Torque_ozfin
        name: qsTr("Ounces force inch")
        abbreviation: "ozfin"
        ratio: 0.141643059
    }

    UnitTypeObject {
        type: UnitTypes.Torque_uNm
        name: qsTr("Micronewtons mètres")
        abbreviation: "µNm"
        ratio: 1000.0
    }
}
