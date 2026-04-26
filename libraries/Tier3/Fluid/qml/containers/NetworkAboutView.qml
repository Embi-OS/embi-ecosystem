import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Fluid

PaneTreeView {
    id: root

    title: qsTr("Informations liées au réseau")

    model: treeModel

    StandardObjectModel {
        id: treeModel
        InfoTreeDelegate {text: qsTr("Adresse IPv4");info: NetworkInformation.ipv4Address}
        InfoTreeDelegate {text: qsTr("Adresse IPv4 Ethernet");info: NetworkInformation.ethernetIPv4Address}
        InfoTreeDelegate {text: qsTr("Adresse IPv4 Wifi");info: NetworkInformation.wifiIPv4Address}
        InfoTreeDelegate {text: qsTr("Adresse IPv4 Usb");info: NetworkInformation.usbIPv4Address}
        InfoTreeDelegate {text: qsTr("Hostname");info: NetworkInformation.localHostName}
        InfoTreeDelegate {text: qsTr("Domain");info: NetworkInformation.localDomainName}

        SeparatorTreeDelegate {}

        SubtitleTreeDelegate {
            text: qsTr("Informations")
            InfoTreeDelegate {text: "Backend";info: NetworkInformation.backend}
            InfoTreeDelegate {text: "Reachability";info: NetworkInformation.reachability}
            InfoTreeDelegate {text: "Transport medium";info: NetworkInformation.transportMedium}
            InfoTreeDelegate {text: "Is behind captive portal";info: NetworkInformation.captivePortal}
            InfoTreeDelegate {text: "Is metered";info: NetworkInformation.metered}
        }
    }
}
