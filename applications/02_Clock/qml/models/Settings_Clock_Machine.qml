pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.System
import Eco.Tier3.Network
import Eco.Tier3.Files
import Eco.Tier3.Database
import Eco.Tier3.Postman
import L02_Clock

StandardObjectModel {
    StandardObject {
        group: "100_About"
        text: qsTr("A propos")
        icon: MaterialIcons.informationOutline
        delegate: SettingsInfosView {}
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
                id: networkEntryTab
                required property NetworkSettingsInterface entry
                required property string name
                required property int type
                text: name
                icon: type===NetworkSettingsType.Wired ? MaterialIcons.ethernet :
                      type===NetworkSettingsType.Wifi ? MaterialIcons.wifi:
                      type===NetworkSettingsType.Bluetooth ? MaterialIcons.bluetooth : MaterialIcons.accountQuestion
                delegate: NetworkSettingsInterfaceView { networkInterface: networkEntryTab.entry }
            }
            onObjectAdded: (index, object) => network.tabsModel.append(object)
            onObjectRemoved: (index, object) => network.tabsModel.append(object)
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
                delegate: FileSettingsAboutView { editable: filesystem.editable }
            }
            StandardObject {
                text: qsTr("Stockage")
                icon: MaterialIcons.harddisk
                delegate: FileSettingsStorageView { editable: filesystem.editable }
            }
            StandardObject {
                text: qsTr("Navigateur")
                icon: MaterialIcons.folderSearchOutline
                delegate: FileSettingsBrowserView { editable: filesystem.editable }
            }
        }
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
        text: qsTr("Maintenance")
        icon: MaterialIcons.tools
        delegate: SettingsMaintenanceView {}
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

    property alias rest: rest
    property alias restSettings: restSettings
    property alias restBrowser: restBrowser
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

    property alias database: database
    property alias databaseSettings: databaseSettings
    property alias databaseBrowser: databaseBrowser
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
