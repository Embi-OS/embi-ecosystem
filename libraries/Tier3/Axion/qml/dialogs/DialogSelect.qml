import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal selectAccepted(var value)

    title: qsTr("Formulaire")
    message: qsTr("Editer le formulaire")
    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")

    property alias label: formComboBox.label
    property alias placeholder: formComboBox.placeholder
    property alias infos: formComboBox.infos
    property alias options: formComboBox.options
    property alias textRole: formComboBox.textRole
    property alias valueRole: formComboBox.valueRole
    property alias value: formComboBox.currentValue

    property bool warning: false

    property bool rejectVisible: buttonReject!==""
    property bool acceptVisible: buttonAccept!==""

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; visible: root.rejectVisible; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.acceptVisible; text: root.buttonAccept; onClicked: root.selectAccepted(root.value); highlighted: true }
    ]

    contentItem: Item {
        FormComboBox {
            id: formComboBox
            width: parent.width

            borderColorDefault: Style.colorPrimaryLight
            fitLabel: true

            warning: root.warning || !acceptableInput
        }
    }
}
