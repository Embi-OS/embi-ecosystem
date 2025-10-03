import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

RowContainer {
    id: root

    z: 2

    spacing: 10
    visible: editable

    required property FormObjectModel formModel

    readonly property bool warning: formModel.warning
    property bool editable: true
    property bool bypassDialog: false

    signal editButtonClicked()
    signal cancelButtonClicked()
    signal validateButtonClicked()

    onEditButtonClicked: {
        root.state = "edition"
    }

    onValidateButtonClicked: {
        if (root.bypassDialog || !root.formModel.isDirty) {
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
        if (root.bypassDialog || !root.formModel.isDirty) {
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
        root.formModel.submit()
        root.state = ""
    }

    function revert() {
        root.formModel.revert()
        root.state = ""
    }

    Binding {
        target: root.formModel
        property: "editable"
        value: root.editable && root.state === "edition"
    }

    Binding {
        when: root.state!==""
        target: root.formModel
        property: "selectPolicy"
        value: FormEditPolicies.Disabled
    }

    Binding {
        when: root.state!==""
        target: root.formModel
        property: "submitPolicy"
        value: FormEditPolicies.Manual
    }

    LayoutSpring {
        visible: (root.alignment & Qt.AlignRight) || (root.alignment & Qt.AlignHCenter)
    }

    SvgColorImage {
        visible: root.formModel.isDirty
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
        enabled: !root.warning
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

    LayoutSpring {
        visible: (root.alignment & Qt.AlignLeft) || (root.alignment & Qt.AlignHCenter)
    }
}
