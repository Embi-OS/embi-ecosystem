import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

PaneTreeView {
    id: root

    title: qsTr("Gestion des logs")

    model: proxyModel

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
        target: Log

        FormSwitchDelegate {
            visible: Log.canOverlay
            editable: root.editable
            label: qsTr("Overlay")
            infos: qsTr("Afficher les logs en direct")
            leftLabel: qsTr("Off")
            rightLabel: qsTr("On")
            targetProperty: "overlay"
        }
        SeparatorTreeDelegate {}
        FormButtonDelegate {
            enabled: root.editable
            label: qsTr("Supprimer les logs")
            icon: MaterialIcons.trashCan
            onClicked: SystemHelper.clearLogs()
        }
        FormButtonDelegate {
            label: qsTr("Exporter les logs")
            icon: MaterialIcons.export_
            onClicked: SystemHelper.exportLogs()
        }
        SeparatorTreeDelegate {}
        InfoTreeDelegate {
            text: qsTr("Chemin");
            info: Paths.log()
        }
    }
}
