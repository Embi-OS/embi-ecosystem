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

    property int nbDataMax: 0
    property bool canMove: true
    property bool canAddRemove: true

    readonly property ItemSelectionModel selectionModel: tableList.selectionModel
    readonly property bool hasSelection: tableList.hasSelection
    readonly property bool hasCurrent: tableList.hasCurrent
    readonly property int currentSourceRow: tableList.currentSourceRow

    property bool editable: true
    property bool bypassDialog: false

    signal editButtonClicked()
    signal editRowButtonClicked()
    signal plusButtonClicked()
    signal upButtonClicked()
    signal downButtonClicked()
    signal copyButtonClicked()
    signal deleteButtonClicked()
    signal validateButtonClicked()
    signal cancelButtonClicked()

    onEditButtonClicked: {
        root.state = "edition"
    }

    onPlusButtonClicked: {
        root.selectionModel.clear()
        root.showForm(function (formValues) {
            let index = root.currentSourceRow>0 ? root.currentSourceRow : root.sourceModel.rowCount()-1
            index = index+1;
            root.sourceModel.insert(index, formValues)
            root.selectionModel.SelectionHelper.setCurrentRow(index)
        })
    }

    onCopyButtonClicked: {
        let index = root.currentSourceRow>0 ? root.currentSourceRow : root.sourceModel.rowCount()-1
        let data = root.sourceModel.ModelHelper.get(index)
        index = index+1;
        root.sourceModel.insert(index, data)
        root.selectionModel.SelectionHelper.setCurrentRow(index)
    }

    onUpButtonClicked: {
        root.sourceModel.moveUp(root.currentSourceRow)
    }

    onDownButtonClicked: {
        root.sourceModel.moveDown(root.currentSourceRow)
    }

    onDeleteButtonClicked: {
        root.sourceModel.remove(root.currentSourceRow)
        root.selectionModel.clear()
    }

    onEditRowButtonClicked: {
        root.formObject = root.sourceModel.ModelHelper.map(root.currentSourceRow)
        root.showForm(function (formValues) {})
    }

    onValidateButtonClicked: {
        if (root.bypassDialog || !root.sourceModel.isDirty) {
            root.submit()
            return
        }

        var settings = {
            "onAccepted": function () {
                root.submit()
            }
        }
        DialogManager.showValidate(settings)
    }

    onCancelButtonClicked: {
        if (root.bypassDialog || !root.sourceModel.isDirty) {
            root.revert()
            return
        }

        var settings = {
            "onAccepted": function () {
                root.revert()
            }
        }
        DialogManager.showCancel(settings)
    }

    function submit() {
        root.sourceModel.submit()
        root.state = ""
    }

    function revert() {
        root.sourceModel.revert()
        root.state = ""
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
            "onFormValidated": function (formValues) {
                onFormValidated(formValues)
            }
        }
        DialogManager.showForm(settings)
    }

    Binding {
        target: root.tableList
        property: "checkable"
        value: false
    }

    Binding {
        target: root.tableList
        property: "sortable"
        value: false
    }

    Binding {
        target: root.tableList
        property: "navigationEnabled"
        value: root.state === ""
    }

    Binding {
        target: root.tableList
        property: "pointerNavigationEnabled"
        value: root.state === "edition"
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

    SvgColorImage {
        visible: root.sourceModel.isDirty
        color: Style.colorWhite
        size: 24
        icon: MaterialIcons.asterisk
    }

    FabButton {
        visible: root.state === "" && root.editable
        enabled: root.enabled
        highlighted: true
        hint: qsTr("Entrer en mode édition")
        icon.source: MaterialIcons.pen
        onClicked: root.editButtonClicked()
    }

    FabButton {
        visible: root.state === "edition"
        highlighted: true
        text: qsTr("Sauvegarder")
        hint: qsTr("Sauvegarder les modifications")
        icon.source: MaterialIcons.check
        onClicked: root.validateButtonClicked()
    }

    FabButton {
        visible: root.state === "edition"
        text: qsTr("Annuler")
        hint: qsTr("Annuler les modifications")
        icon.source: MaterialIcons.cancel
        onClicked: root.cancelButtonClicked()
    }

    BasicSeparator {
        visible: root.state === "edition"
    }

    FabButton {
        visible: root.canAddRemove && root.state === "edition" && (root.nbDataMax<=0 || root.sourceModel.count<root.nbDataMax)
        highlighted: true
        hint: qsTr("Ajouter un élément")
        icon.source: MaterialIcons.plus
        onClicked: root.plusButtonClicked()
    }

    FabButton {
        visible: root.hasCurrent && root.canAddRemove && root.state === "edition"
        // text: qsTr("Supprimer")
        hint: qsTr("Supprimer l'élément sélectionné")
        icon.source: MaterialIcons.delete_
        onClicked: root.deleteButtonClicked()
    }

    FabButton {
        visible: root.hasCurrent && root.state === "edition"
        // text: qsTr("Editer")
        hint: qsTr("Editer l'élément sélectionné")
        icon.source: MaterialIcons.pen
        onClicked: root.editRowButtonClicked()
    }

    FabButton {
        visible: root.hasCurrent && root.canMove && root.state === "edition"
        // text: qsTr("Monter")
        hint: qsTr("Déplacer l'élément sélectionné vers le haut")
        icon.source: MaterialIcons.arrowUp
        onClicked: root.upButtonClicked()
    }

    FabButton {
        visible: root.hasCurrent && root.canMove && root.state === "edition"
        // text: qsTr("Descendre")
        hint: qsTr("Déplacer l'élément sélectionné vers le bas")
        icon.source: MaterialIcons.arrowDown
        onClicked: root.downButtonClicked()
    }

    FabButton {
        visible: root.hasCurrent && root.canAddRemove && root.state === "edition"
        // text: qsTr("Copier")
        hint: qsTr("Copier l'élément sélectionné")
        icon.source: MaterialIcons.contentCopy
        onClicked: root.copyButtonClicked()
    }

    LayoutSpring {
        visible: (root.alignment & Qt.AlignLeft) || (root.alignment & Qt.AlignHCenter)
    }
}

