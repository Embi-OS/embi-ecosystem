import QtQuick
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal accepted(string text)
    signal editingFinished(string text)

    property alias textArea: textArea

    property alias text: textArea.text
    property alias inputMethodHints: textArea.inputMethodHints
    property alias textInputFocus: textArea.focus
    property alias textInputActiveFocus: textArea.activeFocus
    property alias lineCount: textArea.lineCount

    highlighted: textInputActiveFocus

    cellHeight: textArea.contentHeight+Style.formInnerMargin*2

    BasicTextArea {
        id: textArea
        parent: root.controlRectangle
        anchors.fill: parent
        anchors.margins: Style.formInnerMargin

        verticalAlignment: TextInput.AlignVCenter
        wrapMode: TextEdit.Wrap
        readOnly: !root.editable
        font: root.font
        color: root.colorValue

        onEditingFinished: {
            root.editingFinished(text)
            root.accepted(text)
        }

        placeholderText: root.placeholder
        placeholderTextColor: ColorUtils.transparent(root.colorValue, 0.4)
    }
}
