pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.System
import Eco.Tier3.Network
import Eco.Tier3.Files

StandardObjectModel {
    id: root

    StandardObject {
        text: qsTr("A propos")
        icon: MaterialIcons.informationOutline
        delegate: SettingsInfosView {}
    }

    StandardObject {
        group: "200_System"
        text: qsTr("Affichage")
        icon: MaterialIcons.monitor
        delegate: SettingsDisplayView {}
    }

    StandardObject {
        group: "200_System"
        text: qsTr("Langue")
        icon: MaterialIcons.earth
        delegate: SettingsLocaleView {}
    }

    StandardObject {
        group: "200_System"
        text: qsTr("Heure et date")
        icon: MaterialIcons.calendarClock
        delegate: SettingsTimedateView {}
    }

    StandardObject {
        id: network
        group: "200_System"
        text: qsTr("Réseau")
        icon: MaterialIcons.lan

        asynchronous: false
        delegate: PaneTabView { tabsModel: network.tabsModel }

        value: tabsModel
        readonly property StandardObjectModel tabsModel: StandardObjectModel {
            StandardObject {
                text: qsTr("A propos")
                icon: MaterialIcons.informationOutline
                delegate: NetworkSettingsAboutView {}
            }
        }

        property Instantiator instantiator: Instantiator {
            model: NetworkSettingsManager.interfaces
            delegate: StandardObject {
                id: interfaceSettingObject
                editable: network.editable
                required property NetworkSettingsInterface entry
                required property string name
                required property int type
                text: name
                icon: type===NetworkSettingsType.Wired ? MaterialIcons.ethernet :
                      type===NetworkSettingsType.Wifi ? MaterialIcons.wifi:
                      type===NetworkSettingsType.Bluetooth ? MaterialIcons.bluetooth : MaterialIcons.accountQuestion
                delegate: NetworkSettingsInterfaceView {
                     networkInterface: interfaceSettingObject.entry
                }
            }
            onObjectAdded: (index, object) => network.tabsModel.append(object)
            onObjectRemoved: (index, object) => network.tabsModel.remove(object)
        }
    }

    StandardObject {
        id: filesystem
        group: "200_System"
        text: qsTr("Fichiers")
        icon: MaterialIcons.folderOutline

        asynchronous: false
        delegate: PaneTabView { tabsModel: filesystem.tabsModel }

        value: tabsModel
        readonly property StandardObjectModel tabsModel: StandardObjectModel {
            StandardObject {
                text: qsTr("A propos")
                icon: MaterialIcons.informationOutline
                delegate: FileSettingsAboutView {}
            }

            StandardObject {
                text: qsTr("Stockage")
                icon: MaterialIcons.harddisk
                delegate: FileSettingsStorageView {}
            }

            StandardObject {
                editable: filesystem.editable
                text: qsTr("Navigateur")
                icon: MaterialIcons.folderSearchOutline
                delegate: FileSettingsBrowserView {}
            }
        }
    }

    StandardObject {
        group: "300_System"
        text: qsTr("U-Boot")
        icon: MaterialIcons.submarine
        delegate: SettingsUBootView {}
    }

    StandardObject {
        group: "300_System"
        text: qsTr("SSH")
        icon: MaterialIcons.ssh
        delegate: SettingsSshView {}
    }

    StandardObject {
        group: "300_System"
        text: qsTr("Launcher")
        icon: MaterialIcons.rocketLaunch
        delegate: SettingsAppControllerView {}
    }

    StandardObject {
        group: "300_System"
        text: qsTr("Logs")
        icon: MaterialIcons.fileDocumentOutline
        delegate: SettingsLogsView {}
    }

    StandardObject {
        id: swupdate
        group: "300_System"
        text: "SWUpdate"
        icon: MaterialIcons.swupdate

        asynchronous: false
        delegate: PaneTabView { tabsModel: swupdate.tabsModel }

        value: tabsModel
        readonly property StandardObjectModel tabsModel: StandardObjectModel {
            StandardObject {
                text: qsTr("A propos")
                icon: MaterialIcons.informationOutline
                delegate: SwupdateAboutView {}
            }

            StandardObject {
                text: qsTr("Configuration")
                icon: MaterialIcons.cog
                delegate: SwupdateSettingsView {}
            }
        }
    }
}
