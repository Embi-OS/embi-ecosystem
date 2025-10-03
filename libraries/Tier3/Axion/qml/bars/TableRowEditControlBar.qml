import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

RowContainer {
    id: root

    z: 2

    spacing: 10
    visible: editable

    required property PaneTableList tableList
    required property VariantListModel sourceModel
    required property Component formModel
    required property Component defaultObject
    property QtObject formObject

    readonly property ItemSelectionModel selectionModel: tableList.selectionModel
    readonly property bool hasSelection: tableList.hasSelection
    readonly property bool hasCurrent: tableList.hasCurrent
    readonly property int currentSourceRow: tableList.currentSourceRow

    property bool editable: true
    property bool bypassDialog: false

    signal editButtonClicked()
    signal plusButtonClicked()
    signal deleteButtonClicked()

    onEditButtonClicked: {
        root.state = "edition"
        root.formObject = root.sourceModel.ModelHelper.map(root.currentSourceRow)
        root.showForm(function (formValues) {
            root.submit()
        })
    }

    onPlusButtonClicked: {
        root.state = "creation"
        root.selectionModel.clear()
        root.showForm(function (formValues) {
            root.sourceModel.append(formValues)
            root.submit()
        })
    }

    onDeleteButtonClicked: {
        if (root.bypassDialog) {
            deleteSelectedRows()
            return
        }

        var settings = {
            "onAccepted": function () {
                deleteSelectedRows()
            }
        }
        DialogManager.showDelete(settings)
    }

    function showForm(onFormValidated: var) {
        var destroyObject = false
        if (!root.formObject) {
            root.formObject = root.defaultObject.createObject(root) as QtObject
            destroyObject = true
        }

        var model = root.formModel.createObject(root) as FormObjectModel
        var settings = {
            // "title": root.title,
            "formModel": model,
            "formObject": root.formObject,
            "onClosed": function () {
                if (destroyObject)
                    root.formObject.destroy()
                root.formObject = null
                model.destroy()
            },
            "onRejected": function() {
                root.state = ""
            },
            "onFormValidated": function (formValues) {
                onFormValidated(formValues)
            }
        }
        DialogManager.showForm(settings)
    }

    function deleteSelectedRows() {
        root.state = "deletion"
        var rowsSelected = root.selectionModel.SelectionHelper.selectedRows
        var sourceRows = root.tableList.sortProxyModel.mapToSource(rowsSelected)
        sourceRows = root.tableList.proxyModel.mapToSource(sourceRows)
        root.sourceModel.remove(sourceRows)
        root.selectionModel.clear()
        root.submit()
    }

    function submit() {
        root.sourceModel.submit()
        root.state = ""
    }

    function revert() {
        root.sourceModel.revert()
        root.state = ""
    }

    Binding {
        when: root.state!==""
        target: root.sourceModel
        property: "selectPolicy"
        value: QVariantListModelPolicies.Disabled
    }

    Binding {
        when: root.state!==""
        target: root.sourceModel
        property: "submitPolicy"
        value: QVariantListModelPolicies.Manual
    }

    LayoutSpring {
        visible: (root.alignment & Qt.AlignRight) || (root.alignment & Qt.AlignHCenter)
    }

    FabButton {
        visible: root.editable
        highlighted: true
        hint: qsTr("Ajouter un élément")
        icon.source: MaterialIcons.plus
        onClicked: root.plusButtonClicked()
    }

    FabButton {
        visible: root.hasSelection && root.editable
        text: qsTr("Supprimer")
        hint: qsTr("Supprimer les éléments sélectionnés")
        icon.source: MaterialIcons.delete_
        onClicked: root.deleteButtonClicked()
    }

    FabButton {
        visible: root.hasCurrent && root.editable
        text: qsTr("Editer")
        hint: qsTr("Editer l'élément sélectionné")
        icon.source: MaterialIcons.pen
        onClicked: root.editButtonClicked()
    }

    LayoutSpring {
        visible: (root.alignment & Qt.AlignLeft) || (root.alignment & Qt.AlignHCenter)
    }
}
