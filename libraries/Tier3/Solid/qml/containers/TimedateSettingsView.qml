import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Solid

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

    SystemCtlUnitController {
        id: timesyncdService
        unit: "systemd-timesyncd"
    }

    TimedateCtlUnitController {
        id: timedatectlService
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
        FormTextFieldDelegate {
            visible: TimedateSettings.canSetNtpServer
            enabled: ntp.checked
            label: qsTr("Serveur NTP")
            placeholder: qsTr("Seulement si NTP activé")
            infos: qsTr("Laisser vide pour utiliser les valuers par défaut")
            targetProperty: "ntpServer"
            onAccepted: timesyncdService.restart()
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
        SubtitleTreeDelegate {
            visible: timedatectlService.available
            text: "timedatectl"
            onUnfoldedChanged: timedatectlService.refreshStatus()
            FormButtonDelegate {
                visible: TimedateSettings.canSyncRTC
                label: "Sync RTC"
                icon: MaterialIcons.clock
                onClicked: {
                    TimedateSettings.syncRtc()
                    timedatectlService.refreshStatus()
                }
            }
            LabelDelegate {
                font: Style.textTheme.code1
                text: timedatectlService.status
            }
        }
        SubtitleTreeDelegate {
            visible: timedatectlService.available
            text: "timedatectl timesync-status"
            onUnfoldedChanged: timedatectlService.refreshTimesyncStatus()
            LabelDelegate {
                font: Style.textTheme.code1
                text: timedatectlService.timesyncStatus
            }
        }
        SubtitleTreeDelegate {
            visible: timedatectlService.available
            text: "timedatectl show-timesync"
            onUnfoldedChanged: timedatectlService.refreshTimesyncProperties()
            LabelDelegate {
                font: Style.textTheme.code1
                text: timedatectlService.timesyncProperties
            }
        }
        SubtitleTreeDelegate {
            visible: timesyncdService.available
            text: "systemd-timesyncd"
            onUnfoldedChanged: timesyncdService.refreshStatus()
            FormButtonDelegate {
                label: "Sync NTP"
                icon: MaterialIcons.clockStarFourPoints
                onClicked: timesyncdService.restart()
            }
            LabelDelegate {
                font: timesyncdService.unitExists ? Style.textTheme.code1 : Style.textTheme.subtitle1
                text: timesyncdService.unitExists ? timesyncdService.status : qsTr("Aucun service ssh trouvé")
            }
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
        InfoTreeDelegate {
            text: qsTr("NTP")
            info: TimedateSettings.serverName
        }
    }
}
