import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Network

BasicItemDelegate {
    id: root

    property NetworkSettingsInterface netFace: null

    property string name: ""
    property int type: NetworkSettingsType.Unknown
    property int connectionState: NetworkSettingsState.Undefined
    property bool powered: false

    // At least 1s between switching on/off
    Timer {
        id: switchTimer
        interval: 1000
        running: false
    }

    contentItem: RowLayout {
        spacing: root.spacing

        SvgColorImage {
            size: 32
            color: Style.colorWhite
            Layout.alignment: Qt.AlignVCenter
            icon: root.type===NetworkSettingsType.Wired ? MaterialIcons.ethernet :
                  root.type===NetworkSettingsType.Wifi ? MaterialIcons.wifi :
                  root.type===NetworkSettingsType.Bluetooth ? MaterialIcons.bluetooth : ""
        }

        BasicLabel {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            verticalAlignment: Text.AlignVCenter
            font: Style.textTheme.title2
            text: root.netFace.name
        }

        BasicLabel {
            Layout.alignment: Qt.AlignVCenter
            text: root.connectionState===NetworkSettingsState.Idle ? qsTr("Idle") :
                  root.connectionState===NetworkSettingsState.Failure ? qsTr("Erreur") :
                  root.connectionState===NetworkSettingsState.Association ? qsTr("Association") :
                  root.connectionState===NetworkSettingsState.Configuration ? qsTr("Configuration") :
                  root.connectionState===NetworkSettingsState.Ready ? qsTr("Prêt") :
                  root.connectionState===NetworkSettingsState.Disconnect ? qsTr("Déconnecté") :
                  root.connectionState===NetworkSettingsState.Online ? qsTr("Connecté") : ""
        }

        BasicSwitch {
            Layout.alignment: Qt.AlignVCenter
            visible: root.netFace
            checked: root.powered
            checkable: visible && !switchTimer.running

            onCheckedChanged: {
                // Power on/off all WiFi interfaces
                if (root.netFace) {
                    root.netFace.powered = checked
                    switchTimer.start()
                }
            }
        }
    }
}
