import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

RawButton {
    id: root

    property int index: -1

    property string primaryText: ""
    property string secondaryText: ""
    property string iconSource: ""
    property color iconColor: Style.amber
    property bool active: false

    implicitWidth: 220
    implicitHeight: 80

    round: false
    radius: 18
    color: Style.colorPrimary
    highlighted: false
    padding: 12

    contentItem: RowLayout {
        spacing: 12

        BasicRectangularGlow {
            Layout.preferredWidth: 48
            Layout.preferredHeight: 48
            color: root.active ? ColorUtils.transparent(root.iconColor, 0.2) : Style.colorPrimaryDark
            glowRadius: 1
            spread: 0.75
            cornerRadius: (height/2) + glowRadius

            SvgColorImage {
                anchors.centerIn: parent
                size: 28
                icon: root.iconSource
                color: root.active ? root.iconColor : root.foregroundColor
            }
        }

        LabelWithCaption {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            spacing: 2
            textFont: Style.textTheme.subtitle2
            textColor: root.foregroundColor
            text: root.primaryText
            captionFont: Style.textTheme.caption1
            captionColor: ColorUtils.transparent(root.foregroundColor, 0.7)
            caption: root.secondaryText
        }
    }
}
