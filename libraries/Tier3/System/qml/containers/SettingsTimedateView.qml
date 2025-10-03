pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.System

PaneTreeView {
    id: root

    title: qsTr("Réglage date et heure")

    model: proxyModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    footer: FormEditControlBar {
        formModel: treeModel
        editable: root.editable
    }

    FormObjectModel {
        id: treeModel
        editable: root.editable
        target: TimedateSettings

        FormSwitchDelegate {
            id: ntp
            visible: TimedateSettings.canSetNtp
            label: qsTr("Réglage automatique (NTP)")
            infos: qsTr("Nécessite une connexion internet")
            leftLabel: qsTr("Off")
            rightLabel: qsTr("On")
            targetProperty: "ntp"
        }
        FormComboBoxDelegate {
            visible: TimedateSettings.canSetTimezone
            label: qsTr("Fuseau horaire")
            options: TimezoneModel {}
            isSorted: true
            textRole: "identifier"
            targetProperty: "timezone"
        }
        FormTextFieldDateDelegate {
            visible: TimedateSettings.canSetSystemDateTime
            enabled: !ntp.checked
            label: qsTr("Date")
            placeholder: qsTr("Seulement si NTP désactivé")
            targetProperty: "systemDate"
        }
        FormTextFieldTimeDelegate {
            visible: TimedateSettings.canSetSystemDateTime
            enabled: !ntp.checked
            label: qsTr("Heure")
            placeholder: qsTr("Seulement si NTP désactivé")
            targetProperty: "systemTime"
        }
        SeparatorTreeDelegate {}
        FormButtonDelegate {
            label: "Timedate CTL"
            icon: MaterialIcons.consoleLine
            onClicked: SystemHelper.showTimedateCtl()
        }
        FormButtonDelegate {
            visible: TimedateSettings.canReadRTC
            label: "Sync RTC"
            icon: MaterialIcons.clock
            onClicked: TimedateSettings.syncRtc()
        }
        SeparatorTreeDelegate {}
        InfoTreeDelegate {
            text: qsTr("Date")
            info: DateTimeUtils.formatDate(DateTimeUtils.systemDateTime)
        }
        InfoTreeDelegate {
            text: qsTr("Heure")
            info: DateTimeUtils.formatTime(DateTimeUtils.systemDateTime)
        }
        SeparatorTreeDelegate {}
        InfoTreeDelegate {
            text: qsTr("Serveur NTP")
            info: TimedateSettings.serverName
        }
        FormButtonDelegate {
            label: "Sync NTP"
            icon: MaterialIcons.clockStarFourPoints
            onClicked: TimedateSettings.syncNtp()
        }
    }
}
