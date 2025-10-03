pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Network

PaneTreeView {
    id: root

    title: qsTr("Informations liées au réseau")

    model: treeModel

    NetStatModel {
        id: netStats
        tick: (netSubtree.visible&&netSubtree.unfolded) ? 1000 : 0
    }

    ModelMatcher {
        id: matcher
        delayed: true
        sourceModel: NetworkSettingsManager.interfaces
        roleName: "state"
        value: NetworkSettingsState.Online
    }

    StandardObjectModel {
        id: treeModel
        InfoTreeDelegate {text: qsTr("Adresse IPv4 Ethernet");info: Network.ethernetIPv4Address}
        InfoTreeDelegate {text: qsTr("Adresse IPv4 Wifi");info: Network.wifiIPv4Address}
        InfoTreeDelegate {text: qsTr("Adresse IPv4 Usb");info: Network.usbIPv4Address}
        InfoTreeDelegate {text: qsTr("Hostname");info: Network.hostname}
        InfoTreeDelegate {text: qsTr("Domain");info: Network.domain}

        SeparatorTreeDelegate {}

        SubtitleTreeDelegate {
            text: qsTr("Hostname")
            InfoTreeDelegate {text: "hostname";info: HostnameSettings.hostname}
            InfoTreeDelegate {text: "staticHostname";info: HostnameSettings.staticHostname}
            InfoTreeDelegate {text: "prettyHostname";info: HostnameSettings.prettyHostname}
            InfoTreeDelegate {text: "iconName";info: HostnameSettings.iconName}
            InfoTreeDelegate {text: "chassis";info: HostnameSettings.chassis}
            InfoTreeDelegate {text: "kernelName";info: HostnameSettings.kernelName}
            InfoTreeDelegate {text: "kernelRelease";info: HostnameSettings.kernelRelease}
            InfoTreeDelegate {text: "kernelVersion";info: HostnameSettings.kernelVersion}
            InfoTreeDelegate {text: "operatingSystemPrettyName";info: HostnameSettings.operatingSystemPrettyName}
            InfoTreeDelegate {text: "operatingSystemCPEName";info: HostnameSettings.operatingSystemCPEName}
            InfoTreeDelegate {text: "hardwareVendor";info: HostnameSettings.hardwareVendor}
            InfoTreeDelegate {text: "hardwareModel";info: HostnameSettings.hardwareModel}
        }

        SubtitleTreeDelegate {
            text: qsTr("Informations")
            InfoTreeDelegate {text: "Backend";info: Network.backend}
            InfoTreeDelegate {text: "Reachability";info: Network.reachability}
            InfoTreeDelegate {text: "Transport medium";info: Network.transportMedium}
            InfoTreeDelegate {text: "Is behind captive portal";info: Network.captivePortal}
            InfoTreeDelegate {text: "Is metered";info: Network.metered}

            SubtitleTreeDelegate {
                text: "NetworkSettingsInformation"
                InfoTreeDelegate {text: "Initialized";info: NetworkSettingsInformation.isInitialized}
                InfoTreeDelegate {text: "Reachability";info: Network.reachabilityAsString(NetworkSettingsInformation.reachability)}
                InfoTreeDelegate {text: "Transport medium";info: Network.transportMediumAsString(NetworkSettingsInformation.transportMedium)}
            }
        }

        SubtitleTreeDelegate {
            id: netSubtree
            text: qsTr("Statistiques")
            InfoTreeDelegate {text: "NET rxThroughput";info: ("%1/s").arg(FormatUtils.bytes(netStats.net.rxThroughput))}
            InfoTreeDelegate {text: "NET rxBytes";info: ("%1").arg(FormatUtils.bytes(netStats.net.rxBytes))}
            InfoTreeDelegate {text: "NET txThroughput";info: ("%1/s").arg(FormatUtils.bytes(netStats.net.txThroughput))}
            InfoTreeDelegate {text: "NET txBytes";info: ("%1").arg(FormatUtils.bytes(netStats.net.txBytes))}
        }
    }
}
