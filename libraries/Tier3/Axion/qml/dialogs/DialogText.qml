import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    title: qsTr("Information")
    message: ""
    property string buttonReject: ""
    property string buttonAccept: qsTr("Fermer")

    property bool rejectVisible: buttonReject!==""
    property bool acceptVisible: buttonAccept!==""

    property string infos: ""
    property int textFormat: TextEdit.AutoText

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; visible: root.rejectVisible; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.acceptVisible; text: root.buttonAccept; highlighted: true }
    ]

    padding: Style.contentRectangleBorderWidth
    contentColor: Style.colorPrimaryDark

    contentItem: PaneTextView {
        padding: Style.contentRectangleBorderWidth
        implicitHeight: 400
        implicitWidth: 800
        text: root.infos
        textFormat: root.textFormat
    }
}
