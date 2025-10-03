pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier1.Models
import Eco.Tier3.Axion

T.AbstractButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                    Math.max(implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding))

    topInset: 6
    bottomInset: 6

    leftPadding: padding + (!mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding: padding + (mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)

    property bool authorizeEmpty: true
    property bool authorizeGlobal: true
    property list<var> editableValues: []
    property string placeholderText
    property color placeholderTextColor: ColorUtils.transparent(colorValue, 0.4)

    property double listWidth: Math.max(root.width,200)
    property int innerMargin: 8
    property double elevation: Style.menuElevation

    property int borderWidth: Style.contentRectangleBorderWidth
    property color borderColor: Style.colorPrimaryLight
    property color backgroundColor: Style.colorPrimary
    property color color: Style.colorTransparent
    property color colorValue: Style.colorWhite

    property string separator: "; "
    property string textRole: "display"
    property string valueRole: textRole

    property string globalText: qsTr("Select all")

    readonly property list<var> currentTexts: model.ModelHelper.getProperties(currentIndexes, textRole)
    readonly property list<var> currentValues: model.ModelHelper.getProperties(currentIndexes, valueRole)
    readonly property bool hasSelection: checkableProxyModel.hasSelection
    readonly property int count: model.ModelHelper.count
    readonly property string displayText: root.currentTexts.join(root.separator)
    readonly property bool opened: popup.visible

    property AbstractItemModel model: null
    property alias currentIndexes: checkableProxyModel.selectedRows

    signal accepted()
    signal activated(list<int> indexes)

    CheckableProxyModel {
        id: checkableProxyModel
        sourceModel: root.model
        onSelectedRowsChanged: root.activated(selectedRows)
    }

    function selectAll() {
        checkableProxyModel.selectAll()
    }
    function select(index: int) {
        checkableProxyModel.selectRow(index)
    }
    function deselect(index: int) {
        checkableProxyModel.deselectRow(index)
    }
    function isSelected(index: int): bool {
        return checkableProxyModel.isChecked(index)
    }
    function clear() {
        checkableProxyModel.clear()
    }

    function indexesOfValues(values: list<var>): list<int> {
        var indexes=[]
        if(root.model.ModelHelper.isEmpty) {
            return indexes;
        }
        for(const value of values) {
            indexes.push(root.model.ModelHelper.indexOf((root.valueRole!=="" ? root.valueRole : root.textRole),value))
        }
        return indexes
    }

    font: Style.textTheme.headline5

    onClicked: popup.open()

    component ComboCheckBoxItem: BasicCheckBox {
        implicitHeight: Style.menuItemMinHeight
        topInset: Style.menuItemTopInset
        bottomInset: Style.menuItemBottomInset
        leftInset: Style.menuItemLeftInset
        rightInset: Style.menuItemRightInset

        leftPadding: Style.menuItemIconPadding + leftInset
        rightPadding: Style.menuItemIconPadding + rightInset
        topPadding: 0
        bottomPadding: 0

        spacing: 10
        font: Style.menuItemFont

        foregroundColor: root.colorValue
    }

    Binding {
        target: popup.contentItem
        property: "header"
        when: root.authorizeGlobal
        value: Component {
            ColumnLayout {
                width: parent ? parent.width : 0
                ComboCheckBoxItem {
                    Layout.fillWidth: true
                    tristate: true
                    text: root.globalText

                    checkState: !checkableProxyModel.hasSelection ? Qt.Unchecked :
                                checkableProxyModel.selectionCount===root.count ? Qt.Checked : Qt.PartiallyChecked

                    nextCheckState: function() {
                        if (checkState===Qt.Unchecked)
                            return Qt.Checked
                        else
                            return Qt.Unchecked
                    }

                    onClicked: {
                        if (checkState===Qt.Checked)
                            root.selectAll()
                        else if (checkState===Qt.Unchecked)
                            root.clear()
                    }
                }
                BasicSeparator {
                    color: Style.colorPrimaryLight
                    orientation: Qt.Horizontal
                }
            }
        }
    }

    property Component delegate: ComboCheckBoxItem {
        required property var model
        required property int index

        readonly property var value: model[root.valueRole]
        enabled: root.editableValues.length===0 || root.editableValues.includes(value)

        width: ListView.view.width
        text: model[root.textRole]
        checked: checkableProxyModel.hasSelection && root.isSelected(index)
        onClicked: checkableProxyModel.setRow(index, checked)
    }

    indicator: SvgColorImage {
        visible: root.enabled
        x: root.mirrored ? root.padding : root.width - width - root.padding
        y: root.topPadding + (root.availableHeight - height) / 2
        icon: MaterialIcons.chevronDown
        size: 24
        color: root.pressed ? Style.colorPrimaryLight : Style.colorWhite
    }

    contentItem: Text {
        text: checkableProxyModel.hasSelection ? root.displayText : root.placeholderText
        elide: checkableProxyModel.hasSelection ? Text.ElideNone : Text.ElideRight
        wrapMode: Text.Wrap
        font: root.font
        color: checkableProxyModel.hasSelection ? root.colorValue : root.placeholderTextColor
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        color: root.color
        border.color: root.borderColor
        border.width: root.borderWidth
    }

    BasicPopup {
        id: popup
        ExtraPosition.margins: 10
        ExtraPosition.position: ItemPositions.BottomEnd
        width: root.listWidth
        height: Math.min(contentItem.implicitHeight + verticalPadding * 2, root.Window.height - topMargin - bottomMargin)
        transformOrigin: Item.Top
        topMargin: 10
        bottomMargin: 10
        verticalPadding: 8

        onClosed: root.accepted()

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: root.model
            delegate: root.delegate
            reuseItems: true

            ScrollIndicator.vertical: BasicScrollIndicator {
                color: Style.colorPrimaryLight
            }
        }
    }
}
