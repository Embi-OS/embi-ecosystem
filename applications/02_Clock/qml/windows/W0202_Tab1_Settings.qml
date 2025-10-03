import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import L02_Clock

BasicPane {
    id: root

    padding: 10
    topPadding: 0
    contentItem: PaneSplitView {
        title: qsTr("Paramètres appareil")
        comment: qsTr("Certains changements peuvent nécessiter un redémarrage du système")

        model: Settings_Clock_Machine {}
    }
}
