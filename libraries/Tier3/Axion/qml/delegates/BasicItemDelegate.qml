import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ItemDelegate {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double inset: Style.itemDelegateInset
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Style.itemDelegatePadding + inset
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    spacing: Style.itemDelegateSpacing

    font: Style.textTheme.body1

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property bool clickable: true
    property color color: Style.colorPrimary
    property color relativeBackgroundColor: root.backgroundColor.a>0 ? root.backgroundColor : Style.colorBackground
    property color foregroundColor: ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    property color backgroundColor: root.color
    property int wrapMode: Text.NoWrap
    property int elide: Text.ElideRight
    property int textFormat: Text.PlainText

    property int radius: Style.itemDelegateRadius

    readonly property double backgroundImplicitWidth: Style.itemDelegateMinWidth
    readonly property double backgroundImplicitHeight: Style.itemDelegateMinHeight

    icon.width: 32
    icon.height: 32
    icon.color: root.foregroundColor
    icon.source: ""

    contentItem: IconLabel {
        opacity: 1.0
        spacing: root.spacing

        icon.source: root.icon.source
        icon.width: root.icon.width
        icon.height: root.icon.height
        icon.color: root.icon.color
        icon.cache: root.icon.cache

        alignment: Qt.AlignLeft
        font: root.font
        text: root.text
        color: root.foregroundColor
        wrapMode: root.wrapMode
        elide: root.elide
        textFormat: root.textFormat
    }

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight

        radius: root.radius
        color: root.backgroundColor

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: root.radius
            enabled: root.clickable

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
