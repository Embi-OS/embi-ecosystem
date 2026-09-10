import QtQuick
import Eco.Tier3.Axion

BasicItemDelegate {
    id: root

    property string name: ""
    property bool groupEnabled: true
    property int enabledAlarmCount: 0
    property int disabledAlarmCount: 0

    signal groupEnabledClicked()

    backgroundImplicitWidth: 200
    backgroundImplicitHeight: 100

    icon.width: 82
    icon.height: 82
    icon.color: Style.colorBlack
    icon.source: MaterialIcons.alarm

    contentItem: ColumnLayout {
        spacing: 5

        Item {
            Layout.fillHeight: true
            implicitWidth: 5
        }

        BasicLabel {
            Layout.fillHeight: true
            Layout.fillWidth: true
            font: Style.textTheme.title2
            color: root.foregroundColor
            text: root.name
            elide: Text.ElideRight
            verticalAlignment: Text.AlignBottom
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 0

            SvgColorImage {
                size: 20
                icon: MaterialIcons.alarm
                color: root.foregroundColor
            }

            BasicLabel {
                text: root.enabledAlarmCount
                color: root.foregroundColor
                font: Style.textTheme.subtitle1
            }

            Item {
                implicitHeight: 5
                implicitWidth: 5
            }

            SvgColorImage {
                size: 20
                icon: MaterialIcons.alarmOff
                color: ColorUtils.transparent(root.foregroundColor, 0.5)
            }

            BasicLabel {
                text: root.disabledAlarmCount
                color: ColorUtils.transparent(root.foregroundColor, 0.5)
                font: Style.textTheme.subtitle1
            }

            Item {
                Layout.fillWidth: true
                implicitHeight: 5
            }
        }
    }

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight

        radius: root.radius
        color: root.backgroundColor
        border.width: root.borderWidth
        border.color: root.borderColor

        SvgColorImage {
            anchors.top: parent.top
            anchors.topMargin: root.inset
            anchors.right: parent.right
            anchors.rightMargin: root.inset
            iconWidth: root.icon.width
            iconHeight: root.icon.height
            icon: root.groupEnabled ? MaterialIcons.alarm : MaterialIcons.alarmOff
            color: root.icon.color
            opacity: 0.3
        }

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
