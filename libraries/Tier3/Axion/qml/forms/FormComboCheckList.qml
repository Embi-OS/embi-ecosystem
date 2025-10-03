import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal accepted()
    signal activated(list<int> index)

    property alias textRole: comboCheckList.textRole
    property alias valueRole: comboCheckList.valueRole
    property alias authorizeEmpty: comboCheckList.authorizeEmpty
    property alias authorizeGlobal: comboCheckList.authorizeGlobal
    property alias editableValues: comboCheckList.editableValues
    property alias currentIndexes: comboCheckList.currentIndexes
    property alias currentValues: comboCheckList.currentValues
    property alias currentTexts: comboCheckList.currentTexts
    property alias count: comboCheckList.count
    property alias options: comboCheckList.model
    property alias separator: comboCheckList.separator
    property alias globalText: comboCheckList.globalText
    property alias displayText: comboCheckList.displayText
    property alias hasSelection: comboCheckList.hasSelection

    property alias comboBoxFocus: comboCheckList.focus
    property alias comboBoxActiveFocus: comboCheckList.activeFocus
    property alias comboBoxOpened: comboCheckList.opened

    property alias comboCheckList: comboCheckList

    function selectAll() {
        comboCheckList.selectAll()
    }
    function select(index: int) {
        comboCheckList.select(index)
    }
    function deselect(index: int) {
        comboCheckList.deselect(index)
    }
    function clear() {
        comboCheckList.clear()
    }
    function isSelected(index: int): bool {
        return comboCheckList.isSelected(index)
    }
    function indexesOfValues(values: list<var>): list<int> {
        return comboCheckList.indexesOfValues(values)
    }

    placeholder: qsTr("Sélectionner un/des élément(s)")

    warning: !authorizeEmpty && (!hasSelection)
    highlighted: comboBoxOpened

    cellHeight: comboCheckList.implicitContentHeight+Style.formInnerMargin*2

    BasicComboCheckList {
        id: comboCheckList

        parent: root.controlRectangle
        anchors.fill: parent
        anchors.margins: Style.formInnerMargin
        innerMargin: 0
        topInset: 0
        bottomInset: 0

        font: root.font

        borderWidth: 0
        borderColor: "transparent"

        placeholderText: root.placeholder
        placeholderTextColor: ColorUtils.transparent(root.colorValue, 0.4)

        onAccepted: root.accepted()
        onActivated: (indexes) => root.activated(indexes)
    }
}
