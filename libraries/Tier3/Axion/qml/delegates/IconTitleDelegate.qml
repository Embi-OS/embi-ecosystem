import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.ItemDelegate {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double inset: 0
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: Math.max(radius, borderWidth) + inset
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    spacing: Style.sideButtonSpacing

    text: ""
    font: Style.menuItemFont //textTheme.title2
    property string hint: ""
    property font hintFont: Style.textTheme.hint2 //caption1
    property bool showIndicator: false

    opacity: enabled ? 1.0 : 0.5
    Behavior on opacity { NumberAnimation { duration: 150 } }

    icon.width: Style.sideButtonIconSize
    icon.height: Style.sideButtonIconSize
    icon.color: root.color
    icon.source: ""

    contentItem: RowLayout {
        spacing: root.spacing
        SvgColorImage {
            Layout.alignment: Qt.AlignVCenter
            icon: root.icon.source
            iconWidth: Style.roundToIconSize(root.icon.width)
            iconHeight: Style.roundToIconSize(root.icon.height)
            color: root.icon.color
            cache: root.icon.cache
        }

        LabelWithCaption {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            color: root.color
            text: root.text
            textFont: root.font
            caption: root.hint
            captionFont: root.hintFont
            captionElide: Text.ElideRight
        }

        SvgColorImage {
            visible: root.showIndicator
            Layout.alignment: Qt.AlignVCenter
            size: 24
            icon: MaterialIcons.chevronRight
            color: root.icon.color
        }
    }

    property color relativeBackgroundColor: Style.colorBackground

    property double elevation: 0
    property color color: {
        if(highlighted || checked)
            return Style.colorAccent
        return ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack
    }
    property color backgroundColor: Style.colorTransparent
    property color borderColor: Style.colorTransparent
    property double borderWidth: Style.containerBorderWidth
    property double radius: Style.sideButtonRadius

    property double backgroundImplicitWidth: Style.buttonMinWidth
    property double backgroundImplicitHeight: Style.buttonMinHeight

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight
        radius: root.radius
        color: root.backgroundColor
        border.width: root.borderWidth
        border.color: root.borderColor

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: 4

            relativeBackgroundColor: root.relativeBackgroundColor
            focussed: root.down || root.visualFocus || root.hovered
            pressed: root.pressed
        }
    }
}
