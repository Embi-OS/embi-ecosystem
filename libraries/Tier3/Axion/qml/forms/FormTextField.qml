import QtQuick
import Eco.Tier1.Utils
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal accepted(string text)
    signal textEdited(string text)
    signal editingFinished(string text)

    property alias icons: trailingLayout.data

    property alias validator: textInput.validator
    property alias text: textInput.text
    property alias echoMode: textInput.echoMode
    property alias inputMethodHints: textInput.inputMethodHints
    property alias acceptableInput: textInput.acceptableInput
    property alias textInputFocus: textInput.focus
    property alias textInputActiveFocus: textInput.activeFocus
    property int textType: FormTextTypes.String

    property bool showClearIcon: true

    errorMessage: validator ? validator.ExtraValidator.errorMessage : ""

    warning: !acceptableInput
    highlighted: textInputActiveFocus

    function clear() {
        textInput.clear();
    }

    function clearAndFocus() {
        textInput.clear();
        textInput.forceActiveFocus();
    }

    RowLayout {
        parent: root.controlRectangle
        anchors.fill: parent
        anchors.margins: Style.formInnerMargin

        BasicTextField {
            id: textInput

            Layout.fillHeight: true
            Layout.fillWidth: true

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            readOnly: !root.editable

            echoMode: TextInput.Normal
            inputMethodHints: root.textType===FormTextTypes.Number ? Qt.ImhFormattedNumbersOnly : 0

            clip: true
            color: root.warning ? root.colorWarning : root.colorValue
            font: root.font

            onAccepted: root.accepted(text)
            onTextEdited: root.textEdited(text)
            onEditingFinished: {
                root.editingFinished(text)
                if(acceptableInput)
                    root.accepted(text)
            }

            placeholderText: root.placeholder
            placeholderTextColor: ColorUtils.transparent(root.colorValue, 0.4)

            ScanEventNotifier {
                enabled: root.enabled && root.textType===FormTextTypes.Scan
                onNewScan: (scanValue) => {
                    textInput.clear();
                    textInput.insert(0, scanValue)
                    textInput.focus = false
                }
            }
        }

        RowLayout {
            id: trailingLayout
            Layout.alignment: Qt.AlignVCenter
            visible: root.editable && root.enabled

            ClickableIcon {
                visible: root.showClearIcon
                icon: MaterialIcons.backspaceOutline
                onClicked: root.clearAndFocus()
            }
        }
    }
}
