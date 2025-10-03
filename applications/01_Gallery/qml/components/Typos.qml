pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    width: Math.min(640, parent.width)

    FormSwitch {
        id: enabledSwitch
        Layout.fillWidth: true
        rightLabel: "Enabled"
        leftLabel: "Disabled"
        checked: true
        onActivated: (checked) => enabledSwitch.checked=checked
    }

    FormComboBox {
        Layout.fillWidth: true
        options: Utils.fontFamilies()
        Component.onCompleted: currentIndex = indexOfValue(Theme.textTheme.primaryFont)

        onActivated: Theme.textTheme.primaryFont = currentValue
    }

    BasicSeparator {
        orientation: Qt.Horizontal
    }

    BasicListView {
        id: listView

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 400

        model: [
            "headline1",
            "headline2",
            "headline3",
            "headline4",
            "headline5",
            "headline6",
            "headline7",
            "headline8",
            "title1",
            "title2",
            "subtitle1",
            "subtitle2",
            "body1",
            "body2",
            "button",
            "capital",
            "caption1",
            "caption2",
            "overline",
            "hint1",
            "hint2",
            "code"
        ]
        delegate: RowLayout {
            id: typoEntry
            width: (ListView.view as BasicListView).viewWidth

            required property string modelData
            readonly property font font: Style.textTheme[modelData]

            enabled: enabledSwitch.checked

            BasicLabel {
                Layout.preferredWidth: 65
                font: Style.textTheme.caption1
                text: typoEntry.modelData
            }

            BasicLabel {
                Layout.fillWidth: true
                font: typoEntry.font
                text: typoEntry.modelData + " " + font.pixelSize + "sp"
            }
        }

        ScrollIndicator.vertical: BasicScrollIndicator {
            alwaysOn: true
            visible: listView.contentHeight>listView.height
        }
    }
}
