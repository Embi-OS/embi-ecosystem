import QtQuick
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Swupdate

PaneTreeView {
    id: root

    signal updateButtonClicked()
    signal restartButtonClicked()

    title: "SWUpdate"

    model: proxyModel

    onUpdateButtonClicked: {
        var settings = {
            "title": qsTr("Mettre à jour"),
            "message": qsTr("Sélectionner le fichier de mise à jour"),
            "selectionType": FolderTreeTypes.File,
            "nameFilters": ["*.swu"],
            "onPathSelected": function(path) {
                Swupdate.update(path)
            }
        }
        DialogManager.showFileTree(settings);
    }

    onRestartButtonClicked: {
        Swupdate.restart();
    }

    SystemCtlUnitController {
        id: swupdateService
        unit: "swupdate.service"
    }

    SystemCtlUnitController {
        id: swupdateProgressService
        unit: "swupdate-progress.service"
    }

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    StandardObjectModel {
        id: treeModel
        FormButtonDelegate {
            enabled: root.editable && Swupdate.isReady
            highlighted: true
            label: qsTr("Mettre à jour")
            icon: MaterialIcons.update
            onClicked: root.updateButtonClicked()
        }

        SeparatorTreeDelegate {}
        FormButtonDelegate {
            enabled: root.editable && Swupdate.isReady
            label: qsTr("Relancer SWUpdate")
            icon: MaterialIcons.restart
            onClicked: root.restartButtonClicked()
        }

        SeparatorTreeDelegate {}
        InfoTreeDelegate {text: qsTr("Produit");info: Version.productName}
        InfoTreeDelegate {text: qsTr("Version");info: Version.version}
        SeparatorTreeDelegate {}
        InfoTreeDelegate {text: qsTr("Qt Version");info: DeviceInfo.qtVersion}
        InfoTreeDelegate {text: qsTr("Platforme");info: DeviceInfo.deviceName}
        InfoTreeDelegate {text: qsTr("Kernel");info: DeviceInfo.platformKernelVersion}
        InfoTreeDelegate {text: qsTr("Date de compilation");info: DeviceInfo.buildDate}

        SeparatorTreeDelegate {}
        SubtitleTreeDelegate {
            text: "swupdate.service"
            onUnfoldedChanged: swupdateService.refreshStatus()
            LabelDelegate {
                font: swupdateService.unitExists ? Style.textTheme.code : Style.textTheme.subtitle1
                text: swupdateService.unitExists ? swupdateService.status : qsTr("Aucun service ssh trouvé")
            }
        }

        SubtitleTreeDelegate {
            text: "swupdate-progress.service"
            onUnfoldedChanged: swupdateProgressService.refreshStatus()
            LabelDelegate {
                font: swupdateProgressService.unitExists ? Style.textTheme.code : Style.textTheme.subtitle1
                text: swupdateProgressService.unitExists ? swupdateProgressService.status : qsTr("Aucun service ssh trouvé")
            }
        }

        SubtitleTreeDelegate {
            text: "Progress"
            InfoTreeDelegate { text: "API Version"; info: Swupdate.progressMessage.apiVersion }
            InfoTreeDelegate { text: "Update Status"; info: SwupdateRecoveryStatuses.asString(Swupdate.progressMessage.status) }
            InfoTreeDelegate { text: "Interface that triggered the update"; info: SwupdateSourceTypes.asString(Swupdate.progressMessage.source) }
            InfoTreeDelegate { text: "Additional information"; info: Swupdate.progressMessage.info }
            InfoTreeDelegate { text: "Downloaded data [%]"; info: Swupdate.progressMessage.downloadPercent }
            InfoTreeDelegate { text: "Total of bytes to be downloaded"; info: FormatUtils.bytes(Swupdate.progressMessage.downloadBytes) }
            InfoTreeDelegate { text: "No. total of steps"; info: Swupdate.progressMessage.nbSteps }
            InfoTreeDelegate { text: "Current step index"; info: Swupdate.progressMessage.currentStep }
            InfoTreeDelegate { text: "Current step progress [%]"; info: Swupdate.progressMessage.currentStepPercent }
            InfoTreeDelegate { text: "Name of image to be installed"; info: Swupdate.progressMessage.currrentImage }
            InfoTreeDelegate { text: "Name of running handler"; info: Swupdate.progressMessage.handlerName }
        }

        SeparatorTreeDelegate {}
        SubtitleTreeDelegate {
            text: qsTr("Eteindre / Redémarrer")
            FormButtonDelegate {
                enabled: Power.canShutdown
                highlighted: true
                label: qsTr("Eteindre")
                onClicked: Power.shutdown()
            }
            FormButtonDelegate {
                enabled: Power.canReboot
                label: qsTr("Redémarrer")
                onClicked: Power.reboot()
            }
        }
    }
}
