import QtQuick
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System
import L00_Launcher

Item {
    id: root

    readonly property bool editable: applicationController.hasAppController

    ApplicationController {
        id: applicationController
        onErrorOccurred: (error) => DialogManager.showError(error)
    }

    component LauncherButton: RawButton {
        implicitWidth: 300
        implicitHeight: 120
        spacing: 20
        font: Style.textTheme.title1
    }

    ColumnLayout {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 40
        spacing: 40

        RowLayout {
            id: buttons
            spacing: 20

            LauncherButton {
                highlighted: true
                text: "Install"
                icon.source: MaterialIcons.applicationImport
                onClicked: applicationController.install()
            }
            LauncherButton {
                enabled: root.editable
                text: "Launch"
                icon.source: MaterialIcons.rocketLaunch
                onClicked: applicationController.launch()
            }
        }

        BasicLabel {
            Layout.maximumWidth: buttons.width
            Layout.fillHeight: true
            text: Version.aboutQt()
            wrapMode: Text.Wrap
        }

        BasicLabel {
            font: Style.textTheme.caption1
            text: DeviceInfo.about()
            wrapMode: Text.Wrap
        }
    }

    ColumnLayout {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 10

        BasicLabel {
            Layout.fillWidth: true
            horizontalAlignment: Qt.AlignHCenter
            font: Style.textTheme.title2
            text: "Networks:"
            color: Style.colorWhiteFade
        }

        Repeater {
            model: ProxyModel {
                delayed: true
                sourceModel: NetworkInterfaceModel {
                    refreshInterval: 1000
                }
                filterRoleName: "up"
                filterValue: true
            }
            delegate: BasicLabel {
                required property string address
                required property string name
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignRight
                font: Style.textTheme.title1
                text: ("%1: %2").arg(name).arg(address)
                color: Style.colorWhiteFade
            }
        }
    }
}
