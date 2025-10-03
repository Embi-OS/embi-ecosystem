pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ComboBox {
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

    property bool authorizeEmpty: false
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

    readonly property bool opened: popup.visible

    signal textEdited(string text)

    property string emptyText: "N/A"
    currentIndex: 0

    font: Style.textTheme.headline5

    displayText: currentIndex<0 ? emptyText : currentText

    Binding {
        target: root.popup.contentItem
        property: "header"
        when: root.authorizeEmpty
        value: Component {
            ComboBoxItem {
                text: root.emptyText
                width: parent ? parent.width : 0
                onClicked: {
                    root.popup.close()
                    root.activated(-1)
                }
            }
        }
    }

    component ComboBoxItem: BasicLabelDelegate {
        font: Style.menuItemFont

        topInset: Style.menuItemTopInset
        bottomInset: Style.menuItemBottomInset
        leftInset: Style.menuItemLeftInset
        rightInset: Style.menuItemRightInset

        leftPadding: Style.menuItemIconPadding + leftInset
        rightPadding: Style.menuItemIconPadding + rightInset
        topPadding: topInset
        bottomPadding: bottomInset
        spacing: Style.menuItemSpacing
        radius: Style.menuItemCornerRadius
    }

    delegate: ComboBoxItem {
        required property var model
        required property int index

        width: ListView.view.width
        text: model[root.textRole]
        foregroundColor: root.currentIndex===index ? Style.colorAccent : root.colorValue
        highlighted: root.highlightedIndex === index
        hoverEnabled: root.hoverEnabled
    }

    indicator: SvgColorImage {
        visible: root.enabled
        x: root.mirrored ? root.padding : root.width - width - root.padding
        y: root.topPadding + (root.availableHeight - height) / 2
        icon: MaterialIcons.chevronDown
        size: 24
        color: root.pressed ? Style.colorPrimaryLight : Style.colorWhite
    }

    contentItem: BasicTextField {
        text: root.editable ? root.editText : root.displayText

        enabled: root.editable
        autoScroll: root.editable
        readOnly: root.down
        inputMethodHints: root.inputMethodHints
        validator: root.validator
        selectByMouse: root.selectTextByMouse
        font: root.font
        placeholderText: root.placeholderText
        placeholderTextColor: root.placeholderTextColor

        color: root.colorValue
        verticalAlignment: Text.AlignVCenter
        onTextEdited: root.textEdited(text)
    }

    background: Rectangle {
        implicitWidth: 40
        implicitHeight: 40
        color: root.color
        border.color: root.borderColor
        border.width: root.borderWidth
    }

    popup: BasicPopup {
        ExtraPosition.margins: 10
        ExtraPosition.position: ItemPositions.BottomEnd
        width: root.listWidth
        height: Math.min(contentItem.implicitHeight + verticalPadding * 2, root.Window.height - topMargin - bottomMargin)
        transformOrigin: Item.Top
        padding: Style.menuPadding
        topMargin: 10
        bottomMargin: 10

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: root.delegateModel
            currentIndex: root.highlightedIndex
            highlightMoveDuration: 0
            reuseItems: true

            ScrollIndicator.vertical: BasicScrollIndicator {
                color: Style.colorPrimaryLight
            }
        }
    }
}
