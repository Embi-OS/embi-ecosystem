import QtQuick
import Eco.Tier1.Models
import Eco.Tier2.Solid
import Eco.Tier3.Axion
import L02_Clock

BasicDrawer {
    id: root

    contentItem: PaneTabView {
        id: pane
        proxyModel.enabled: pane.visible
        active: root.position>=1.0
        headerPadding: 10
        footerPadding: 10
        drawFooterSeparator: true
        footer: RowContainer {
            topInset: pane.footerPadding
            bottomInset: pane.footerPadding
            leftInset: pane.footerPadding
            rightInset: pane.footerPadding

            ClickableIcon {
                icon: MaterialIcons.close
                color: Style.colorWhite
                size: 32
                onClicked: root.close()
            }

            LabelWithCaption {
                Layout.fillWidth: true
                horizontalAlignment: Qt.AlignRight
                textFont: Style.textTheme.body1
                text: qsTr("Adresse IPv4 Ethernet: %1").arg(NetworkSettingsManager.currentWiredConnection?.ipv4.address ?? "N/A")
                captionFont: Style.textTheme.body1
                caption: qsTr("Adresse IPv4 Wifi: %1").arg(NetworkSettingsManager.currentWifiConnection?.ipv4.address ?? "N/A")
            }
        }

        tabsModel: StandardObjectModel {
            StandardObject {
                text: qsTr("Réglages")
                icon: MaterialIcons.cogs
                delegate: W0202_Tab1_Settings {}
            }
            StandardObject {
                text: qsTr("Pi-hole")
                icon: MaterialIcons.piHole
                delegate: W0202_Tab2_Pihole {}
            }
            StandardObject {
                text: qsTr("Home Assistant")
                icon: MaterialIcons.homeAssistant
                delegate: W0202_Tab3_HomeAssistant {}
            }
        }
    }
}
