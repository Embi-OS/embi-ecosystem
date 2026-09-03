pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import L02_Clock

Item {
    id: root

    readonly property AlarmModel alarmModel: Clock.alarmModel
    readonly property AlarmGroupModel alarmGroupModel: Clock.alarmGroupModel

    property string selectedGroupUuid: ""

    signal plusButtonClicked()
    signal deleteButtonClicked()
    signal cancelButtonClicked()
    signal validateConfirmed()
    signal deleteConfirmed()

    signal groupPlusButtonClicked()
    signal groupEditButtonClicked()

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
        var component = Qt.createComponent("L02_Clock", "Form_Clock_AlarmDetails");
        DialogManager.showAlarm({
            "hour": 6,
            "minute": 0,
            "detailsFormComponent": component,
            "onClosed": function() {
                component.destroy()
            },
            "onAlarmValidated": function(alarmMap) {
                alarmMap.group = selectedGroupUuid
                root.alarmModel.create(alarmMap);
            }
        });
    }

    onGroupPlusButtonClicked: {
        var component = Qt.createComponent("Eco.Tier3.Axion", "Form_AlarmGroup");
        var model = component.createObject(root) as FormObjectModel;
        DialogManager.showForm({
            "title": title,
            "formModel": model,
            "onClosed": function() {
                model.destroy()
                component.destroy()
            },
            "onFormValidated": function(formValues) {
                root.alarmGroupModel.create(formValues)
            }
        })
    }

    onGroupEditButtonClicked: {
        for(let index = 0; index < root.alarmGroupModel.count; index++) {
            const groupObject = root.alarmGroupModel.at(index) as AlarmGroupObject
            if(!groupObject || groupObject.uuid !== root.selectedGroupUuid)
                continue

            var component = Qt.createComponent("Eco.Tier3.Axion", "Form_AlarmGroup");
            var model = component.createObject(root) as FormObjectModel;
            DialogManager.showForm({
                "title": title,
                "formModel": model,
                "formValues": groupObject.toMap(),
                "onClosed": function() {
                    model.destroy()
                    component.destroy()
                },
                "onFormValidated": function(formValues) {
                    groupObject.fromMap(formValues)
                }
            })
            return
        }
    }

    function selectGroup(groupUuid: string) {
        root.selectedGroupUuid = root.selectedGroupUuid === groupUuid ? "" : groupUuid
    }

    function removeSelectedGroup() {
        const groupUuid = root.selectedGroupUuid
        if(groupUuid === "")
            return

        Clock.removeGroup(groupUuid)
        root.selectedGroupUuid = ""
    }

    function assignSelectedAlarmsToGroup() {
        const selectedAlarms = checkableModel.ModelHelper.getProperties(checkableModel.selectedRows, "qtObject")
        if(selectedAlarms.length === 0)
            return

        DialogManager.showSelect({
            "title": qsTr("Déplacer les alarmes"),
            "message": qsTr("Choisir un groupe pour %1 alarme(s)").arg(selectedAlarms.length),
            "label": qsTr("Groupe"),
            "options": root.alarmGroupModel,
            "textRole": "name",
            "valueRole": "uuid",
            "authorizeEmpty": true,
            "emptyText": qsTr("Sans groupe"),
            "value": "",
            "onSelectAccepted": function(groupUuid) {
                Clock.assignAlarmsToGroup(selectedAlarms.map(alarmObject => alarmObject.uuid), groupUuid ?? "")

                checkableModel.clear()
                root.state = ""
            }
        })
    }

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants de la page
//──────────────────────────────────────────────────────────────────────

    CheckableProxyModel {
        id: checkableModel
        sourceModel: root.alarmModel
    }

    Component {
        id: groupDefaultObject

        QtObject {
            property string name: ""
            property bool enabled: true
        }
    }

    SortFilterProxyModel {
        id: proxyModel
        sourceModel: checkableModel
        filters: ValueFilter {
            roleName: "group"
            value: root.selectedGroupUuid
        }
        sorters: [RoleSorter {
            roleName: "enabled"
            sortOrder: Qt.DescendingOrder
        },
        RoleSorter {
            roleName: "msToNextRingTime"
            sortOrder: Qt.AscendingOrder
        }]
    }

    PaneListView {
        id: view
        anchors.fill: parent
        anchors.topMargin: 0
        anchors.margins: 10
        drawFrame: true
        model: proxyModel

        header: PaneListView {
            visible: !root.alarmGroupModel.isEmpty
            topPadding: view.headerPadding
            leftPadding: view.headerPadding
            rightPadding: view.headerPadding
            headerPadding: view.headerPadding

            title: qsTr("Groupe(s)")
            viewOrientation: ListView.Horizontal
            viewAdd: null
            viewRemove: null
            viewMove: null
            view.ScrollBar.vertical: null
            view.implicitHeight: 120
            model: root.alarmGroupModel

            delegate: AlarmGroupDelegate {
                height: (ListView.view as BasicListView).viewHeight
                required property QtObject qtObject
                required property int index

                readonly property AlarmGroupObject groupObject: qtObject as AlarmGroupObject
                enabled: root.state === "" && groupObject
                name: groupObject?.name ?? ""
                enabledAlarmCount: groupObject?.enabledAlarmCount ?? 0
                disabledAlarmCount: groupObject?.disabledAlarmCount ?? 0
                groupEnabled: groupObject?.enabled ?? false
                highlighted: root.selectedGroupUuid === (groupObject?.uuid ?? "")

                onClicked: root.selectGroup(groupObject?.uuid ?? "")
                onGroupEnabledClicked: Clock.setGroupEnabled(groupObject?.uuid ?? "", !groupEnabled)
            }
        }

        viewAdd: null
        viewRemove: null
        viewMove: null
        view.ScrollBar.vertical: null
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
                if(alarmObject)
                    Clock.setAlarmEnabled(alarmObject.uuid, switchEnable.checked)
            }

            checkable: root.state==="selection"

            onClicked: {
                if(root.state==="selection") {
                    checkableModel.toggleRow(proxyModel.mapToSource(index))
                }
                else {
                    var component = Qt.createComponent("L02_Clock", "Form_Clock_AlarmDetails");
                    var settings = alarmObject.toMap()
                    settings.showName = true
                    settings.detailsFormComponent = component
                    settings.onClosed = function() {
                        component.destroy()
                    }
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
                backgroundImplicitSize: 64
                icon.source: MaterialIcons.alarmPlus
                onClicked: root.plusButtonClicked()
            }
            FabButton {
                visible: root.state === "selection"
                highlighted: true
                hint: qsTr("Supprimer les éléments sélectionnés")
                backgroundImplicitSize: 64
                icon.source: MaterialIcons.delete_
                onClicked: root.deleteButtonClicked()
            }
            FabButton {
                visible: root.state === "selection"
                hint: qsTr("Déplacer les alarmes sélectionnées dans un groupe")
                backgroundImplicitSize: 64
                icon.source: MaterialIcons.alarmMultiple
                onClicked: root.assignSelectedAlarmsToGroup()
            }
            FabButton {
                visible: root.state === "selection"
                hint: qsTr("Quitter le mode sélection")
                backgroundImplicitSize: 64
                icon.source: MaterialIcons.cancel
                onClicked: root.cancelButtonClicked()
            }
            FabButton {
                visible: root.state === "" && root.selectedGroupUuid === ""
                hint: qsTr("Ajouter un groupe")
                backgroundImplicitSize: 64
                icon.source: MaterialIcons.alarmMultiple
                onClicked: root.groupPlusButtonClicked()
            }
            FabButton {
                visible: root.state === "" && root.selectedGroupUuid !== ""
                hint: qsTr("Modifier le groupe sélectionné")
                backgroundImplicitSize: 64
                icon.source: MaterialIcons.pen
                onClicked: root.groupEditButtonClicked()
            }
            FabButton {
                visible: root.state === "" && root.selectedGroupUuid !== ""
                hint: qsTr("Supprimer le groupe sélectionné")
                backgroundImplicitSize: 64
                icon.source: MaterialIcons.delete_
                onClicked: root.removeSelectedGroup()
            }

            LayoutSpring {}
        }
    }
}
