import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.MenuItem {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                    Math.max(implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding))

    display: AbstractButton.TextBesideIcon

    topInset: Style.menuItemTopInset
    bottomInset: Style.menuItemBottomInset
    leftInset: Style.menuItemLeftInset
    rightInset: Style.menuItemRightInset

    readonly property double arrowPadding: root.arrow && root.arrow.visible ? root.arrow.width : root.spacing
    readonly property double indicatorPadding: root.indicator && root.indicator.visible ? root.indicator.width + root.spacing : 0

    leftPadding: (!mirrored ? Style.menuItemIconPadding : indicatorPadding + arrowPadding) + leftInset
    rightPadding: (mirrored ? Style.menuItemIconPadding : indicatorPadding + arrowPadding) + rightInset
    topPadding: 0
    bottomPadding: 0
    spacing: Style.menuItemSpacing

    property bool outlined: false
    property color foregroundColor: {
        if(outlined)
            return Style.colorAccent
        return enabled ? Style.colorWhite : ColorUtils.transparent(Style.colorWhite, 0.6)
    }
    property color backgroundColor: "transparent"
    property double radius: Style.menuItemCornerRadius

    property string hint: ""

    font: Style.menuItemFont

    icon.width: Style.menuItemIconWidth
    icon.height: Style.menuItemIconWidth
    icon.color: foregroundColor
    icon.source: checkable && checked ? MaterialIcons.check : ""

    indicator: Text {
        x: root.mirrored ? root.leftPadding : root.width - width - root.rightPadding
        anchors.right: !root.mirrored ? root.right : undefined
        anchors.left: root.mirrored ? root.left : undefined
        anchors.rightMargin: !root.mirrored ? root.arrowPadding : 0
        anchors.leftMargin: root.mirrored ? root.arrowPadding : 0
        y: root.topPadding + (root.availableHeight - height) / 2

        color: root.foregroundColor
        font: Style.textTheme.hint2
        text: root.hint ? root.hint : dummyShortcut.nativeText
        visible: root.hint || (root.action && dummyShortcut.nativeText !== "")
        horizontalAlignment: root.mirrored ? Qt.AlignRight : Qt.AlignLeft
        enabled: false

        Shortcut {
            id: dummyShortcut
            enabled: false
            sequence: root.action ? root.action.shortcut : ""
        }
    }

    arrow: SvgColorImage {
        x: root.mirrored ? root.padding : root.width - width - root.padding
        y: root.topPadding + (root.availableHeight - height) / 2

        size: 24
        visible: root.subMenu
        mirror: root.mirrored
        color: root.foregroundColor
        icon: MaterialIcons.chevronRight
    }

    contentItem: IconLabel {
        alignment: Qt.AlignVCenter | Qt.AlignLeft
        elide: Text.ElideRight
        maximumLineCount: 1

        spacing: root.spacing
        display: root.display
        mirrored: root.mirrored
        font: root.font
        icon.source: root.icon.source
        icon.width: root.icon.width
        icon.height: root.icon.height
        icon.color: root.icon.color
        icon.cache: root.icon.cache
        text: root.text
        color: root.foregroundColor
    }

    property double backgroundImplicitWidth: Style.menuItemMinWidth
    property double backgroundImplicitHeight: Style.menuItemMinHeight

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight
        radius: root.radius
        color: root.backgroundColor

        BasicVeil {
            anchors.fill: parent
            radius: root.radius

            relativeBackgroundColor: root.backgroundColor
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
