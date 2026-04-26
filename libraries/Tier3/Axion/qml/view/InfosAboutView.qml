import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

PaneTreeView {
    id: root

    title: qsTr("Informations générales sur le système")

    model: proxyModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
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
        InfoTreeDelegate {text: qsTr("Qt Version");info: DeviceInfo.qtVersion}
        InfoTreeDelegate {text: qsTr("Platforme");info: DeviceInfo.deviceName}
        InfoTreeDelegate {text: qsTr("Hostname");info: DeviceInfo.deviceHostName}
        InfoTreeDelegate {text: qsTr("Identifier");info: DeviceInfo.deviceId}
        InfoTreeDelegate {text: qsTr("Kernel");info: DeviceInfo.platformKernelVersion}
        InfoTreeDelegate {text: qsTr("Date de compilation");info: DeviceInfo.buildDate}
    }
}
