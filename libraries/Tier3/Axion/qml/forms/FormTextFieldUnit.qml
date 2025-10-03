import QtQuick
import Eco.Tier1.Models
import Eco.Tier2.Unit
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal valueAccepted(real value)
    signal valueEdited(real value)
    signal valueEditingFinished(real value)
    signal unitActivated(int index)
    signal unitAccepted()

    property alias textInput: textInput
    property alias validator: textInput.validator
    property double value: 0
    property alias valueEchoMode: textInput.echoMode
    property alias acceptableInput: textInput.acceptableInput
    property alias valueInputFocus: textInput.focus
    property alias valueInputActiveFocus: textInput.activeFocus
    property bool showClearIcon: true

    property alias comboBox: comboBox
    property alias unitTextRole: comboBox.textRole
    property alias unitValueRole: comboBox.valueRole
    property alias unitCurrentIndex: comboBox.currentIndex
    property alias unitCurrentText: comboBox.currentText
    property alias unitCurrentValue: comboBox.currentValue
    property alias unitCount: comboBox.count
    property alias unitFocus: comboBox.focus
    property alias unitActiveFocus: comboBox.activeFocus
    property UnitCategoryObject unitOptions: null
    property bool unitEditable: true

    function clearAndFocus() {
        textInput.clear();
        textInput.forceActiveFocus();
    }

    errorMessage: validator ? validator.ExtraValidator.errorMessage : ""

    warning: !acceptableInput
    highlighted: valueInputActiveFocus

    RowLayout {
        id: layout
        parent: root.controlRectangle
        anchors.fill: parent

        BasicTextField {
            id: textInput

            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: Style.formInnerMargin
            Layout.topMargin: Style.formInnerMargin
            Layout.bottomMargin: Style.formInnerMargin

            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter

            readOnly: !root.editable
            inputMethodHints: Qt.ImhFormattedNumbersOnly

            clip: true
            color: root.warning ? root.colorWarning : root.colorValue
            font: root.font
            text: root.unitOptions.rounded(root.value)

            onAccepted: root.valueAccepted(text)
            onTextEdited: root.valueEdited(text)
            onEditingFinished: {
                root.valueEditingFinished(text)
                if(acceptableInput)
                    root.valueAccepted(text)
            }

            placeholderText: root.placeholder
            placeholderTextColor: ColorUtils.transparent(root.colorValue, 0.4)
        }

        ClickableIcon {
            Layout.alignment: Qt.AlignVCenter
            visible: root.editable && root.enabled && root.showClearIcon
            icon: MaterialIcons.backspaceOutline
            onClicked: root.clearAndFocus()
        }

        BasicSeparator {
            foregroundColor: root.borderColorDefault
        }

        BasicComboBox {
            id: comboBox

            Layout.fillHeight: true
            Layout.preferredWidth: layout.width*0.3
            innerMargin: 0
            Layout.rightMargin: Style.formInnerMargin
            Layout.topMargin: Style.formInnerMargin
            Layout.bottomMargin: Style.formInnerMargin

            borderWidth: 0
            borderColor: color
            color: "transparent"
            font: root.font

            topInset: 0
            bottomInset: 0
            textRole: "abbreviation"
            valueRole: "type"
            model: root.unitOptions

            opacity: root.unitEditable ? 1.0 : 0.4
            enabled: root.unitEditable && root.editable

            onAccepted: root.unitAccepted()
            onActivated: (index) => root.unitActivated(index)
        }
    }
}
