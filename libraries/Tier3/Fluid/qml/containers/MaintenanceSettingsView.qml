import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Fluid

PaneTreeView {
    id: root

    title: qsTr("Maintenance de l'appareil")

    model: proxyModel

    property bool canDeleteAll: root.editable

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    BackupExporter {
        id: backupExporter
    }

    BackupImporter {
        id: backupImporter
    }

    StandardObjectModel {
        id: treeModel
        InfoTreeDelegate {visible: info!==""; text: qsTr("Produit");info: Version.productName}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Version");info: Version.version}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Entreprise");info: Version.company}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Site web");info: Version.website}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Maintainer");info: Version.maintainer}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Copyright");info: Version.copyright}

        SeparatorTreeDelegate {}
        FormButtonDelegate {
            enabled: root.editable
            label: qsTr("Importer")
            icon: MaterialIcons.import_
            onClicked: backupImporter.run()
        }
        FormButtonDelegate {
            label: qsTr("Exporter")
            icon: MaterialIcons.export_
            onClicked: backupExporter.run()
        }
        SeparatorTreeDelegate {}
        FormButtonDelegate {
            enabled: root.editable
            label: qsTr("Supprimer les fichiers temporaires")
            icon: MaterialIcons.fileRemove
            onClicked: MaintenanceHelper.clearCache()
        }
        FormButtonDelegate {
            enabled: root.canDeleteAll
            label: qsTr("Supprimer les réglages")
            icon: MaterialIcons.archiveRemove
            onClicked: MaintenanceHelper.clearSettings()
        }
        FormButtonDelegate {
            enabled: root.canDeleteAll
            label: qsTr("Tout supprimer (RàZ)")
            icon: MaterialIcons.lockReset
            onClicked: MaintenanceHelper.clearAll()
        }

        SeparatorTreeDelegate {}
        InfoTreeDelegate {
            text: qsTr("Chemin screenshot")
            info: Paths.capture()
        }
        InfoTreeDelegate {
            text: qsTr("Chemin fichier de réglage")
            info: Paths.setting()
        }
        InfoTreeDelegate {
            text: qsTr("Chemin base de données")
            info: Paths.database()
        }
        InfoTreeDelegate {
            text: qsTr("Chemin cache")
            info: Paths.cache()
        }
    }
}
