import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

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

    UptimeInfo {
        id: uptimeInfos
        tick: (uptimeSubtree.visible&&uptimeSubtree.unfolded) ? 1000 : 0
    }
    CpuInfoModel {
        id: cpuInfos
    }
    CpuStatModel {
        id: cpuStats
        tick: (cpuStatsSubtree.visible&&cpuStatsSubtree.unfolded) ? 1000 : 0
    }
    ThermalZoneModel {
        id: thermalZones
        tick: (thermalZonesSubtree.visible&&thermalZonesSubtree.unfolded) ? 1000 : 0
    }
    MemoryInfo {
        id: memoryInfos
        tick: (memorySubtree.visible&&memorySubtree.unfolded) ? 1000 : 0
    }

    StandardObjectModel {
        id: treeModel
        InfoTreeDelegate {visible: info!==""; text: qsTr("Produit");info: Version.productName}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Version");info: Version.version}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Entreprise");info: Version.company}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Site web");info: Version.website}
        InfoTreeDelegate {visible: info!==""; text: qsTr("Copyright");info: Version.copyright}
        SeparatorTreeDelegate {}
        InfoTreeDelegate {text: qsTr("Qt Version");info: DeviceInfo.qtVersion}
        InfoTreeDelegate {text: qsTr("Platforme");info: DeviceInfo.deviceName}
        InfoTreeDelegate {text: qsTr("Kernel");info: DeviceInfo.platformKernelVersion}
        InfoTreeDelegate {text: qsTr("Date de compilation");info: DeviceInfo.buildDate}

        SeparatorTreeDelegate {}

        SubtitleTreeDelegate {
            text: "CPU Infos"
            InfoTreeDelegate {text: "CPU model";info: cpuInfos.model}
            InfoTreeDelegate {text: "CPU revision";info: cpuInfos.revision}
            InfoTreeDelegate {text: "CPU serial";info: cpuInfos.serial}
            InfoTreeDelegate {text: "CPU productId";info: cpuInfos.productId}
            InfoTreeDelegate {text: "CPU coreCount";info: cpuInfos.coreCount}
        }
        SubtitleTreeDelegate {
            id: cpuStatsSubtree
            text: "CPU Stats"
            InfoTreeDelegate {text: "Total usage";info: ("%1 %").arg(FormatUtils.realToString(cpuStats.cpu.usage,2))}
            property Instantiator instantiator: Instantiator {
                active: cpuStatsSubtree.visible && cpuStatsSubtree.unfolded
                model: cpuStats
                delegate: InfoTreeDelegate {
                    required property string name
                    required property double usage
                    text: ("%1 usage").arg(name)
                    info: ("%1 %").arg(FormatUtils.realToString(usage,2))
                }
                onObjectAdded: (index, object) => treeModel.insertObject(object as TreeObject, cpuStatsSubtree)
                onObjectRemoved: (index, object) => treeModel.removeObject(object as TreeObject, cpuStatsSubtree)
            }
        }
        SubtitleTreeDelegate {
            id: cpuCoresSubtree
            text: "CPU Cores";
            InfoTreeDelegate {text: "CPU coreCount";info: cpuInfos.coreCount}
            property Instantiator instantiator: Instantiator {
                active: cpuCoresSubtree.visible && cpuCoresSubtree.unfolded
                model: cpuInfos
                delegate: InfoTreeDelegate {
                    required property string name
                    required value
                    text: name
                    info: value
                }
                onObjectAdded: (index, object) => treeModel.insertObject(object as TreeObject, cpuCoresSubtree)
                onObjectRemoved: (index, object) => treeModel.removeObject(object as TreeObject, cpuCoresSubtree)
            }
        }
        SubtitleTreeDelegate {
            id: thermalZonesSubtree
            text: "TEMP Infos"
            InfoTreeDelegate {text: "TEMP count";info: thermalZones.count}
            property Instantiator instantiator: Instantiator {
                active: thermalZonesSubtree.visible && thermalZonesSubtree.unfolded
                model: thermalZones
                delegate: InfoTreeDelegate {
                    required property string key
                    required value
                    text: key
                    info: ("%1 °C").arg(FormatUtils.realToString(value/1000.0,2))
                }
                onObjectAdded: (index, object) => treeModel.insertObject(object as TreeObject, thermalZonesSubtree)
                onObjectRemoved: (index, object) => treeModel.removeObject(object as TreeObject, thermalZonesSubtree)
            }
        }
        SubtitleTreeDelegate {
            id: memorySubtree
            text: "RAM Infos"
            InfoTreeDelegate {text: "RAM total physical";info: FormatUtils.bytes(memoryInfos.totalPhysical)}
            InfoTreeDelegate {text: "RAM free physical";info: FormatUtils.bytes(memoryInfos.freePhysical)}
            InfoTreeDelegate {text: "RAM available physical";info: FormatUtils.bytes(memoryInfos.availablePhysical)}
            InfoTreeDelegate {text: "RAM cached";info: FormatUtils.bytes(memoryInfos.cached)}
            InfoTreeDelegate {text: "RAM buffers";info: FormatUtils.bytes(memoryInfos.buffers)}
            InfoTreeDelegate {text: "RAM total swap file";info: FormatUtils.bytes(memoryInfos.totalSwapFile)}
            InfoTreeDelegate {text: "RAM free swap file";info: FormatUtils.bytes(memoryInfos.freeSwapFile)}
            InfoTreeDelegate {text: "RAM usage";info: ("%1 %").arg(FormatUtils.realToString(memoryInfos.usage,2))}
        }
        SubtitleTreeDelegate {
            id: uptimeSubtree
            text: "UPTIME"
            InfoTreeDelegate {text: "UPTIME total";info: uptimeInfos.upTime}
            InfoTreeDelegate {text: "UPTIME systemTime";info: uptimeInfos.systemTime}
            InfoTreeDelegate {text: "UPTIME idleTime";info: uptimeInfos.idleTime}
        }
    }
}
