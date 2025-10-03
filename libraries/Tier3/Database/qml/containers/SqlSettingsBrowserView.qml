import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Database

BasicPane {
    id: root

    property string tableName: ""
    property string statusMessage: ""

    onTableNameChanged: browser.showTable(root.tableName)

    function select() {
        if (browser.model?.select) {
            console.time("select");
            browser.model.select();
            root.selectionModel.clear();
            console.timeEnd("select");
        }
        else
            browser.showTable(root.tableName);
    }

    function submit() {
        if (browser.model?.submitAll) {
            console.time("submitAll");
            browser.model.submitAll();
            console.timeEnd("submitAll");
        }
    }

    function revert() {
        if (browser.model?.revertAll) {
            console.time("revertAll");
            browser.model.revertAll();
            console.timeEnd("revertAll");
        }
    }

    function editButtonClicked() {
        root.state = "edition"
    }

    function cancelButtonClicked() {
        if (browser.model && browser.model.hasChanged && browser.model.hasChanged())
            DialogManager.showMessage({
                message: qsTr("Êtes-vous sûr de vouloir quitter le mode édition?"),
                infos: qsTr("Les modifications ne seront pas sauvegardées!"),
                buttonAccept: qsTr("Quitter"),
                buttonReject: qsTr("Annuler"),
                onAccepted: function() {
                    root.revert();
                    root.state = ""
                }
            });
        else
            root.state = ""
    }

    function validateButtonClicked() {
        if (browser.model && browser.model.hasChanged && browser.model.hasChanged())
            DialogManager.showMessage({
                message: qsTr("Êtes-vous sûr de vouloir sauvegarder les changements ?"),
                buttonAccept: qsTr("Sauvegarder"),
                buttonReject: qsTr("Annuler"),
                onAccepted: function() {
                    root.submit();
                    root.state = ""
                }
            });
        else
            root.state = ""
    }

    function deleteButtonClicked() {
        if (browser.model && browser.model.select)
            DialogManager.showMessage({
                message: qsTr("Êtes-vous sûr de vouloir supprimer les éléments sélectionnés ?"),
                buttonAccept: qsTr("Supprimer"),
                buttonReject: qsTr("Annuler"),
                onAccepted: function() {
                    console.time("delete");
                    let rowsSelected=root.selectionModel.SelectionHelper.selectedRows;
                    let idSelected=browser.model.ModelHelper.getProperties(rowsSelected,"display")
                    //SqlQuery.deleteWhere(root.tableName, {ID: idSelected});
                    browser.model.select();
                    root.selectionModel.clear();
                    console.timeEnd("delete");
                    root.state = ""
                }
            });
        else
            root.state = ""
    }

    SqlBrowser {
        id: browser
        delayed: true

        onStatusMessage: (message) => root.statusMessage = message
    }

    property alias selectionModel: view.selectionModel
    contentItem: PaneTableCheckable {
        id: view
        // drawFrame: true
        // color: borderColor
        // borderColor: Style.colorPrimary
        // contentColor: Style.colorPrimaryDark

        checkable: root.state===""
        pointerNavigationEnabled: false
        sortable: false
        model: browser.model

        delegate: SqlBrowserTableDelegate {
            editable: root.state==="edition"
        }
    }

    header: RowContainer {
        topInset: root.headerPadding
        leftInset: root.headerPadding
        rightInset: root.headerPadding
        spacing: root.spacing
        drawFrame: true

        FormComboBox {
            Layout.preferredWidth: 300
            borderColorDefault: Style.colorPrimaryLight
            placeholder: "Table"
            options: browser.tableNames
            textRole: "display"
            valueRole: "display"
            onActivated: root.tableName = currentValue
            Component.onCompleted: currentIndex = indexOfValue(root.tableName)
        }
        LayoutSpring {}
        RawButton {
            visible: root.state==="" && !root.selectionModel.hasSelection
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.restore
            text: "Select"
            onClicked: root.select()
        }
        RawButton {
            visible: root.state==="" && !root.selectionModel.hasSelection
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.information
            text: "Metadata"
            onClicked: browser.showMetaData(root.tableName)
        }
        RawButton {
            visible: root.state==="" && root.selectionModel.hasSelection
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.trashCan
            text: "Delete"
            onClicked: root.deleteButtonClicked()
        }
        BasicSeparator { color: Style.colorPrimaryLight }
        RawButton {
            visible: root.state==="edition"
            highlighted: true
            color: Style.colorPrimaryLight
            text: qsTr("Sauvegarder")
            icon.source: MaterialIcons.contentSave
            onClicked: root.validateButtonClicked()
        }
        RawButton {
            visible: root.state==="edition"
            text: qsTr("Annuler")
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.cancel
            onClicked: root.cancelButtonClicked()
        }
        RawButton {
            visible: root.state==="" && browser.editable && root.editable
            text: qsTr("Editer")
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.pen
            onClicked: root.editButtonClicked()
        }
    }

    footer: RowContainer {
        bottomInset: root.footerPadding
        leftInset: root.footerPadding
        rightInset: root.footerPadding
        spacing: root.spacing
        alignment: Qt.AlignVCenter
        drawFrame: true

        BasicLabel {
            text: qsTr("Nb. data: %1").arg(view.rows)
        }

        LayoutSpring {}

        BasicLabel {
            text: root.statusMessage!=="" ? qsTr("Erreur: ")+root.statusMessage : ""
        }
    }
}
