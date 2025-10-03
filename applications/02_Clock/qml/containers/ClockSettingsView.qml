import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import L02_Clock

PaneTreeView {
    id: root

    title: qsTr("Gestion du Pi-Clock")

    model: treeModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    FormObjectModel {
        id: treeModel
        editable: root.editable

        FormSwitchDelegate {
            label: qsTr("Always on")
            target: ClockDisplay
            targetProperty: "alwaysOnEnabled"
        }
    }
}
