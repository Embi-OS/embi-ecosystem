pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Fluid
import Eco.Tier3.Solid
import Eco.Tier3.Database
import Eco.Tier3.Postman
import L02_Clock

StandardObjectModel {
    id: root

    StandardObject {
        group: "100_About"
        text: qsTr("A propos")
        icon: MaterialIcons.informationOutline
        delegate: InfosAboutView {}
    }

    StandardObject {
        group: "100_About"
        visible: ClockDisplay.registered && ClockMedia.registered
        text: qsTr("Clock")
        icon: MaterialIcons.clock
        delegate: ClockSettingsView {}
    }

    StandardObject {
        group: "200_System"
        text: "Monitoring"
        icon: MaterialIcons.chartLine
        delegate: SystemMonitoringAboutView {}
    }

    StandardObject {
        group: "200_System"
        text: qsTr("Affichage")
        icon: MaterialIcons.monitor
        delegate: DisplaySettingsView {}
    }

    StandardObject {
        group: "200_System"
        text: qsTr("Langue")
        icon: MaterialIcons.earth
        delegate: LocaleSettingsView {}
    }

    StandardObject {
        group: "200_System"
        text: qsTr("Heure et date")
        icon: MaterialIcons.calendarClock
        delegate: TimedateSettingsView {}
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
                delegate: NetworkAboutView {}
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
                delegate: NetworkInterfaceSettingsView {
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
                delegate: FilesystemAboutView {}
            }

            StandardObject {
                text: qsTr("Stockage")
                icon: MaterialIcons.harddisk
                delegate: FstabStorageSettingsView {}
            }

            StandardObject {
                text: qsTr("Navigateur")
                icon: MaterialIcons.folderSearchOutline
                delegate: FilesystemBrowserView {}
            }
        }
    }

    StandardObject {
        group: "300_System"
        text: qsTr("Maintenance")
        icon: MaterialIcons.tools
        delegate: MaintenanceSettingsView {}
    }

    StandardObject {
        group: "300_System"
        text: qsTr("SSH")
        icon: MaterialIcons.ssh
        delegate: SshSettingsView {}
    }

    StandardObject {
        group: "300_System"
        text: qsTr("Launcher")
        icon: MaterialIcons.rocketLaunch
        delegate: AppControllerSettingsView {}
    }

    StandardObject {
        group: "300_System"
        text: qsTr("Logs")
        icon: MaterialIcons.fileDocumentOutline
        delegate: LogsSettingsView {}
    }

    StandardObject {
        group: "300_System"
        text: qsTr("U-Boot")
        icon: MaterialIcons.submarine
        delegate: UBootSettingsView {}
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

    StandardObject {
        id: rest
        group: "400_Application"
        visible: RestManager.registered
        text: qsTr("Rest API")
        icon: MaterialIcons.api

        asynchronous: false
        delegate: PaneTabView { tabsModel: rest.tabsModel }

        value: tabsModel
        readonly property StandardObjectModel tabsModel: StandardObjectModel {
            StandardObject {
                id: restSettings
                editable: rest.editable
                text: qsTr("A propos")
                icon: MaterialIcons.informationOutline
                delegate: RestSettingsAboutView { editable: restSettings.editable }
            }
            StandardObject {
                id: restBrowser
                editable: rest.editable
                text: qsTr("Navigateur")
                icon: MaterialIcons.magnify
                delegate: RestSettingsBrowserView { editable: restBrowser.editable }
            }
        }
    }

    StandardObject {
        id: database
        group: "400_Application"
        visible: SqlManager.registered
        text: qsTr("Database")
        icon: MaterialIcons.databaseOutline

        asynchronous: false
        delegate: PaneTabView { tabsModel: database.tabsModel }

        value: tabsModel
        readonly property StandardObjectModel tabsModel: StandardObjectModel {
            StandardObject {
                id: databaseSettings
                editable: database.editable
                text: qsTr("A propos")
                icon: MaterialIcons.informationOutline
                delegate: SqlSettingsAboutView { editable: databaseSettings.editable }
            }
            StandardObject {
                id: databaseBrowser
                editable: database.editable
                text: qsTr("Navigateur")
                icon: MaterialIcons.magnify
                delegate: SqlSettingsBrowserView { editable: databaseBrowser.editable }
            }
        }
    }
}
