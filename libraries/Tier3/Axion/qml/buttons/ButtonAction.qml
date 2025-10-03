import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.AbstractButton {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property bool highlighted: false

    property double inset: 5
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: inset

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    property color colorHighlight: Style.colorAccent
    property color color: Style.colorBlack
    property color foregroundColor: highlighted ? colorHighlight : Style.colorWhite

    property double backgroundImplicitSize: Style.buttonNavigationCircleSize

    icon.width: Style.buttonNavigationIconSize
    icon.height: Style.buttonNavigationIconSize
    icon.color: foregroundColor
    icon.source: ""

    contentItem: SvgColorImage {
        icon: root.icon.source
        iconWidth: root.icon.width
        iconHeight: root.icon.height
        color: root.icon.color
        cache: root.icon.cache
    }

    background: BasicRectangularGlow {
        implicitWidth: root.backgroundImplicitSize
        implicitHeight: root.backgroundImplicitSize

        color: root.foregroundColor
        glowRadius: Style.buttonNavigationGlow
        spread: 0.5
        cornerRadius: (height/2) + glowRadius
        center.color: root.color

        BasicToolTip {
            delay: 500
            visible: root.pressed && root.text!==""
            text: root.text
            ExtraPosition.position: ItemPositions.Top
            ExtraPosition.margins: 5
        }
    }
}
