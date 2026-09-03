import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Fluid

PaneFormView {
    id: root

    directEdit: true
    title: qsTr("Gestion des logs")

    TextFile {
        id: textFile
    }

    function showLogs(filePath: string) {
        textFile.open(filePath, TextFileOpenModes.Read)
        var settings = {
            "textFormat": TextEdit.PlainText,
            "title": textFile.fileName,
            "message": textFile.error ? textFile.error : textFile.fileUrl,
            "traces": textFile.content,
            "textFont": Style.textTheme.code2,
            "fullscreen": true,
            "diagnose": false
        }
        DialogManager.showText(settings);
    }

    formModel: FormObjectModel {
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
            highlighted: true
            label: qsTr("Afficher les logs")
            icon: MaterialIcons.fileEyeOutline
            onClicked: root.showLogs(Paths.log("Log.log"))
        }
        FormButtonDelegate {
            enabled: root.editable
            label: qsTr("Supprimer les logs")
            icon: MaterialIcons.trashCan
            onClicked: MaintenanceHelper.clearLogs()
        }
        FormButtonDelegate {
            label: qsTr("Exporter les logs")
            icon: MaterialIcons.export_
            onClicked: MaintenanceHelper.exportLogs()
        }
        SeparatorTreeDelegate {}
        InfoTreeDelegate {
            text: qsTr("Chemin");
            info: Paths.log()
        }
    }
}
