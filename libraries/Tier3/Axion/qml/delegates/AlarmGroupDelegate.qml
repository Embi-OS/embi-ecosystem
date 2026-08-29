import QtQuick
import Eco.Tier3.Axion

BasicItemDelegate {
    id: root

    property string name: ""
    property bool groupEnabled: true
    property int enabledAlarmCount: 0
    property int disabledAlarmCount: 0

    signal groupEnabledClicked()

    backgroundImplicitWidth: 120
    backgroundImplicitHeight: 120

    contentItem: ColumnLayout {
        spacing: 5

        RowLayout {
            spacing: 0
            Layout.fillWidth: true

            Item {
                Layout.fillWidth: true
                implicitHeight: 5
            }

            ClickableIcon {
                icon: root.groupEnabled ? MaterialIcons.alarm : MaterialIcons.alarmOff
                onClicked: root.groupEnabledClicked()
            }
        }

        BasicLabel {
            Layout.fillHeight: true
            Layout.fillWidth: true
            font: Style.textTheme.subtitle1
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
}
