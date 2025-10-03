pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier2.Solid
import Eco.Tier3.Axion

Item {
    id: root

    required property AlarmModel alarmModel

    signal plusButtonClicked()
    signal deleteButtonClicked()
    signal cancelButtonClicked()
    signal validateConfirmed()
    signal deleteConfirmed()

    onCancelButtonClicked: {
        checkableModel.clear();
        root.state = "";
    }

    onDeleteButtonClicked: {
        let rowsSelected=checkableModel.selectedRows;
        let alarmSelected=checkableModel.ModelHelper.getProperties(rowsSelected,"qtObject")

        for (const alarm of alarmSelected) {
            root.alarmModel.remove(alarm);
        }

        checkableModel.clear();
        root.state = "";
    }

    onPlusButtonClicked: {
        DialogManager.showAlarm({
            hour: 6,
            minute: 0,
            onAlarmValidated: function(alarmMap) {
                root.alarmModel.create(alarmMap);
            }
        });
    }

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants de la page
//──────────────────────────────────────────────────────────────────────

    CheckableProxyModel {
        id: checkableModel
        sourceModel: root.alarmModel
    }

    SortFilterProxyModel {
        id: proxyModel
        sourceModel: checkableModel
        sorters: [RoleSorter {
            roleName: "enabled"
            sortOrder: Qt.DescendingOrder
        },
        RoleSorter {
            roleName: "hour"
            sortOrder: Qt.AscendingOrder
        },
        RoleSorter {
            roleName: "minute"
            sortOrder: Qt.AscendingOrder
        }]
    }

    PaneListView {
        id: view
        anchors.fill: parent
        anchors.margins: 20
        drawFrame: true
        model: proxyModel

        viewSection.property: "enabled"
        viewSection.delegate: BasicLabel {
            width: (ListView.view as BasicListView).viewWidth
            height: contentHeight + 2*Style.contentRectangleBorderWidth
            verticalAlignment: Text.AlignVCenter
            required property bool section
            text: section ? qsTr("Alarme(s) activée(s):") : qsTr("Alarme(s) désactivée(s):")
            font: Style.textTheme.title2
        }

        delegate: AlarmDelegate {
            id: alarmDelegate
            width: (ListView.view as BasicListView).viewWidth
            required property QtObject qtObject
            required property int index
            required checkState

            readonly property AlarmObject alarmObject: qtObject as AlarmObject
            hour: alarmObject?.hour ?? 0
            minute: alarmObject?.minute ?? 0
            date: alarmObject?.date ?? new Date()
            name: alarmObject?.name ?? ""
            repeat: alarmObject?.repeat ?? false
            weekdays: alarmObject?.weekdays ?? 0
            nextTimeRing: alarmObject?.nextTimeRing ?? ""
            switchEnableChecked: alarmObject?.enabled ?? false
            switchEnable.onClicked: {
                if(alarmObject) {
                    var alarmMap = {
                        "date": alarmObject.repeat ? "" : DateTimeUtils.nextValidDateTimeForTime(alarmObject.hour,alarmObject.minute,0,0),
                        "enabled": switchEnable.checked
                    }
                    alarmObject.fromMap(alarmMap)
                }
            }

            checkable: root.state==="selection"

            onClicked: {
                if(root.state==="selection") {
                    checkableModel.toggleRow(proxyModel.mapToSource(index))
                }
                else {
                    var settings = alarmObject.toMap()
                    settings.showName = true
                    settings.onAlarmValidated = function(alarmMap) {
                        alarmObject.fromMap(alarmMap);
                    }
                    DialogManager.showAlarm(settings);
                }
            }
            onPressAndHold: {
                if(root.state!=="selection")
                    checkableModel.clear();
                root.state="selection"
                checkableModel.toggleRow(proxyModel.mapToSource(index));
            }
        }

        viewFooter: RowLayout {
            z: 2
            width: ListView.view.width
            spacing: 10

            LayoutSpring {}

            FabButton {
                visible: root.state === ""
                highlighted: true
                hint: qsTr("Ajouter un élément")
                icon.source: MaterialIcons.plus
                onClicked: root.plusButtonClicked()
            }
            FabButton {
                visible: root.state === "selection"
                highlighted: true
                text: qsTr("Supprimer")
                hint: qsTr("Supprimer les éléments sélectionnés")
                icon.source: MaterialIcons.delete_
                onClicked: root.deleteButtonClicked()
            }
            FabButton {
                visible: root.state === "selection"
                text: qsTr("Quitter")
                hint: qsTr("Quitter le mode sélection")
                icon.source: MaterialIcons.cancel
                onClicked: root.cancelButtonClicked()
            }

            LayoutSpring {}
        }
    }
}
