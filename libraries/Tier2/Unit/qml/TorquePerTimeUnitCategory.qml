import QtQuick
import Eco.Tier2.Unit

UnitCategoryObject {
    id: root

    category: UnitCategories.TorquePerTime
    defaultType: UnitTypes.TorquePerTime_mNm_sec

    UnitTypeObject {
        type: UnitTypes.TorquePerTime_mNm_sec
        name: qsTr("Millinewtons mètres par secondes")
        abbreviation: "mNm/s"
        ratio: 1.0
    }
}
