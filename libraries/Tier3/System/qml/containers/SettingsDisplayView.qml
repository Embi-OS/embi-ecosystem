import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

PaneTreeView {
    id: root

    title: qsTr("Réglage de l'affichage")

    model: proxyModel

    readonly property BasicWindow window: Window.window as BasicWindow

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
        target: DisplaySettings

        FormSliderDelegate {
            visible: DisplaySettings.canSetBrightness
            label: qsTr("Luminosité")
            infos: qsTr("Valable uniquement pour écran DSI")
            from: 1
            to: 255
            targetProperty: "brightness"
        }
        FormSwitchDelegate {
            visible: DisplaySettings.canSetHighDpi
            label: qsTr("Ecran haute densité")
            targetProperty: "highDpi"
            onAccepted: AxionHelper.warningRestart()
        }
        FormComboBoxDelegate {
            visible: DisplaySettings.canSetScaleFactor
            label: qsTr("Mise à l'échelle")
            options: StandardObjectModel {
                StandardObject {text: "75%"; value: DisplayScaleFactors.Pct75}
                StandardObject {text: "100%"; value: DisplayScaleFactors.Default}
                StandardObject {text: "125%"; value: DisplayScaleFactors.Pct125}
                StandardObject {text: "150%"; value: DisplayScaleFactors.Pct150}
                StandardObject {text: "175%"; value: DisplayScaleFactors.Pct175}
                StandardObject {text: "200%"; value: DisplayScaleFactors.Pct200}
            }
            textRole: "text"
            valueRole: "value"
            targetProperty: "scaleFactor"
            onAccepted: AxionHelper.warningRestart()
        }
        FormComboBoxDelegate {
            visible: DisplaySettings.canSetRotation
            label: qsTr("Orientation de l'affichage")
            options: StandardObjectModel {
                StandardObject {text: "0°"; value: DisplayOrientations.Default}
                StandardObject {text: "90°"; value: DisplayOrientations.Deg90}
                StandardObject {text: "180°"; value: DisplayOrientations.Deg180}
                StandardObject {text: "270°"; value: DisplayOrientations.Deg270}
            }
            textRole: "text"
            valueRole: "value"
            targetProperty: "rotation"
            onAccepted: AxionHelper.warningRestart()
        }
        FormSwitchDelegate {
            visible: DisplaySettings.canHideCursor
            label: qsTr("Masquer le curseur")
            targetProperty: "hideCursor"
        }
        FormSwitchDelegate {
            label: qsTr("Montrer les clicks")
            targetProperty: "showMouseClick"
        }
        FormSwitchDelegate {
            visible: DisplaySettings.canHideKeyboard
            label: qsTr("Masquer le clavier")
            targetProperty: "hideKeyboard"
        }
        SeparatorTreeDelegate {}
        InfoTreeDelegate {
            text: qsTr("Résolution");
            info: root.window ? ("%1 x %2").arg(root.window.contentWidth).arg(root.window.contentHeight) : "N/A"
        }
        InfoTreeDelegate {
            text: qsTr("Résolution disponible");
            info: ("%1 x %2").arg(DeviceInfo.screenAvailableWidth).arg(DeviceInfo.screenAvailableHeight)
        }
        InfoTreeDelegate {
            text: qsTr("Taille physique");
            info: ("%1 x %2").arg(DeviceInfo.screenPhysicalWidth).arg(DeviceInfo.screenPhysicalHeight)
        }
        InfoTreeDelegate {
            text: qsTr("DPI");
            info: ("%1 x %2").arg(DeviceInfo.screenDpiX).arg(DeviceInfo.screenDpiY)
        }
        InfoTreeDelegate {
            text: qsTr("Nom")
            info: DeviceInfo.screen.name
        }
        InfoTreeDelegate {
            text: qsTr("Fabricant")
            info: DeviceInfo.screen.manufacturer
        }
        InfoTreeDelegate {
            text: qsTr("Modèle")
            info: DeviceInfo.screen.model
        }
        InfoTreeDelegate {
            text: qsTr("SN")
            info: DeviceInfo.screen.serialNumber
        }
    }
}
