import QtQuick
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal accepted()
    signal activated(int index)
    signal highlighted(int index)
    signal textEdited(string text)

    property alias validator: comboBox.validator
    property alias textRole: comboBox.textRole
    property alias valueRole: comboBox.valueRole
    property alias authorizeEmpty: comboBox.authorizeEmpty
    property alias authorizeCustom: comboBox.editable
    property alias currentIndex: comboBox.currentIndex
    property alias currentValue: comboBox.currentValue
    property alias count: comboBox.count
    property alias options: comboBox.model
    property alias emptyText: comboBox.emptyText
    property alias currentText: comboBox.currentText
    property alias editText: comboBox.editText
    property alias displayText: comboBox.displayText
    property alias inputMethodHints: comboBox.inputMethodHints
    property alias inputMethodComposing: comboBox.inputMethodComposing
    property alias selectTextByMouse: comboBox.selectTextByMouse

    property alias comboBoxFocus: comboBox.focus
    property alias comboBoxActiveFocus: comboBox.activeFocus
    property alias comboBoxOpened: comboBox.opened

    property alias comboBox: comboBox

    function decrementCurrentIndex() {
        comboBox.decrementCurrentIndex()
    }
    function incrementCurrentIndex() {
        comboBox.incrementCurrentIndex()
    }
    function selectAll() {
        comboBox.selectAll()
    }

    function find(text: string, flags: int): int {
        return comboBox.find(text, flags)
    }

    function indexOfValue(value: var): int {
        return comboBox.indexOfValue(value)
    }

    function textAt(index: int): string {
        return comboBox.textAt(index)
    }

    function valueAt(index: int): var {
        return comboBox.valueAt(index)
    }

    placeholder: qsTr("Sélectionner un élément")
    errorMessage: validator ? validator.ExtraValidator.errorMessage : ""

    warning: !authorizeEmpty && (currentIndex<0 || currentIndex>=count)
    highlighted: comboBoxOpened

    BasicComboBox {
        id: comboBox

        parent: root.controlRectangle
        anchors.fill: parent
        anchors.margins: Style.formInnerMargin
        innerMargin: 0
        topInset: 0
        bottomInset: 0

        displayText: currentIndex === -1 ? "" : currentText

        font: root.font

        borderWidth: 0
        borderColor: "transparent"

        placeholderText: root.placeholder
        placeholderTextColor: ColorUtils.transparent(root.colorValue, 0.4)

        onAccepted: root.accepted()
        onTextEdited: (text) => root.textEdited(text)
        onActivated: (index) => root.activated(index)
        onHighlighted: (index) => root.highlighted(index)
    }
}
