import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    title: qsTr("Informations")
    message: ""
    property string buttonReject: ""
    property string buttonAccept: qsTr("Fermer")
    property string buttonHelp: ""

    property bool rejectVisible: buttonReject!==""
    property bool acceptVisible: buttonAccept!==""
    property bool helpVisible: buttonHelp!==""

    property string infos: ""
    property string traces: ""
    property int textFormat: TextEdit.AutoText
    property font textFont: Style.textTheme.code1

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; visible: root.rejectVisible; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.acceptVisible; text: root.buttonAccept; highlighted: true },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.HelpRole; text: root.buttonHelp; visible: root.helpVisible}
    ]

    padding: Style.contentRectangleBorderWidth
    contentColor: Style.colorPrimaryDark

    contentItem: PaneTextView {
        padding: Style.contentRectangleBorderWidth
        implicitHeight: 400
        implicitWidth: 800
        comment: root.infos
        text: root.traces
        textFormat: root.textFormat
        font: root.textFont
    }
}
