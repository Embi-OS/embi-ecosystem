import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal inputAccepted(string value)
    signal inputApplied(string value)

    title: qsTr("Formulaire")
    message: qsTr("Editer le formulaire")
    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")
    property string buttonApply: ""

    property alias label: formTextField.label
    property alias placeholder: formTextField.placeholder
    property alias infos: formTextField.infos
    property alias textType: formTextField.textType
    property alias validator: formTextField.validator
    property alias text: formTextField.text

    property bool warning: false

    property bool rejectVisible: buttonReject!==""
    property bool acceptVisible: buttonAccept!==""
    property bool applyVisible: buttonApply!==""

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; visible: root.rejectVisible; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.acceptVisible; text: root.buttonAccept; onClicked: root.inputAccepted(root.text); highlighted: !root.applyVisible },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.applyVisible; text: root.buttonApply; onClicked: root.inputApplied(root.text); highlighted: true }
    ]

    contentItem: Item {
        FormTextField {
            id: formTextField
            width: parent.width

            borderColorDefault: Style.colorPrimaryLight
            fitLabel: true

            warning: root.warning || !acceptableInput
        }
    }
}
