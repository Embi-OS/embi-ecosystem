import QtQuick
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal acceptedOverwrite()
    signal acceptedVersion()

    title: qsTr("Attention")
    message: qsTr("Êtes-vous sûr de vouloir effectuer cette action?")
    property string infos
    property string traces
    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Confirmer")
    property string buttonApply: ""

    property bool rejectVisible: buttonReject!==""
    property bool acceptVisible: buttonAccept!==""
    property bool applyVisible: buttonApply!==""

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; visible: root.rejectVisible; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.acceptVisible; text: root.buttonAccept; onClicked: root.acceptedOverwrite(); highlighted: !root.applyVisible },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.applyVisible; text: root.buttonApply; onClicked: root.acceptedVersion(); highlighted: true }
    ]

    contentItem: LabelWithCaption {
        spacing: root.spacing
        relativeBackgroundColor: root.color
        textFont: root.font
        text: root.infos
        textWrapMode: Text.Wrap
        textMaximumLineCount: 0
        captionFont: root.font
        caption: root.traces
        captionMaximumLineCount: 0
    }
}
