import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Network

BasicItemDelegate {
    id: root

    property NetworkSettingsService service: null

    property bool canEdit: true

    property string name: ""
    property string mac: ""
    property string identifier: ""
    property string iface: ""
    property int type: NetworkSettingsType.Unknown
    property int signalStrength: 0
    property bool connected: false
    property int connectionState: NetworkSettingsState.Undefined

    signal connectClicked(NetworkSettingsService service)
    signal settingsClicked(NetworkSettingsService service)

    contentItem: RowLayout {
        spacing: root.spacing

        SvgColorImage {
            size: 32
            color: Style.colorWhite
            Layout.alignment: Qt.AlignVCenter
            visible: root.type===NetworkSettingsType.Wifi
            icon: root.signalStrength < 0 ? MaterialIcons.wifiStrengthOffOutline :
                  root.signalStrength <= 10 ? MaterialIcons.wifiStrengthOutline :
                  root.signalStrength <= 30 ? MaterialIcons.wifiStrength1 :
                  root.signalStrength <= 55 ? MaterialIcons.wifiStrength2 :
                  root.signalStrength <= 80 ? MaterialIcons.wifiStrength3 :
                  root.signalStrength <= 100 ? MaterialIcons.wifiStrength4 : MaterialIcons.wifiStrengthOffOutline
        }

        LabelWithCaption {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            text: root.type===NetworkSettingsType.Wired ? root.iface : root.name
            textFont: Style.textTheme.title2
            caption: (root.connected && root.service) ? qsTr("IP Address: %1").arg(root.service.ipv4.address) : ""
            captionFont: Style.textTheme.subtitle1
        }

        BasicLabel {
            Layout.alignment: Qt.AlignVCenter
            text: root.connectionState===NetworkSettingsState.Failure ? qsTr("Erreur") :
                  root.connectionState===NetworkSettingsState.Association ? qsTr("Association") :
                  root.connectionState===NetworkSettingsState.Configuration ? qsTr("Configuration") :
                  root.connectionState===NetworkSettingsState.Disconnect ? qsTr("Déconnecté") :
                  root.connectionState===NetworkSettingsState.Online ? qsTr("Connecté") : ""
        }

        RawButton {
            Layout.alignment: Qt.AlignVCenter
            visible: root.type===NetworkSettingsType.Wifi && root.canEdit
            highlighted: !root.connected
            color: Style.colorPrimaryLight
            text: root.connected ? qsTr("DISCONNECT") : qsTr("CONNECT")
            onClicked: root.connectClicked(root.service)
        }

        RawButton {
            Layout.alignment: Qt.AlignVCenter
            visible: (root.connected || root.type===NetworkSettingsType.Wired) && root.canEdit
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.cogOutline
            onClicked: root.settingsClicked(root.service)
        }
    }
}
