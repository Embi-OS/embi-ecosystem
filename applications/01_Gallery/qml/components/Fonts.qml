import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    width: Math.min(640, parent.width)

    property string currentFontFamily: "Roboto"

    FormComboBox {
        Layout.fillWidth: true
        options: Utils.fontFamilies()
        Component.onCompleted: currentIndex = indexOfValue(Theme.textTheme.primaryFont)

        onActivated: root.currentFontFamily = currentValue
    }

    BasicSeparator {
        orientation: Qt.Horizontal
    }

    RowLayout {
        Layout.alignment: Qt.AlignHCenter
        ColumnLayout {
            id: controls
            RowLayout {
                Text {
                    Layout.preferredWidth: 100
                    text: "Weight:"
                    horizontalAlignment: Text.AlignRight
                    font: Style.textTheme.body1
                    color: Style.colorWhite
                }
                BasicSlider {
                    id: weightSlider
                    from: 100
                    to: 900
                    stepSize: 100
                    snapMode: Slider.SnapAlways
                    value: 300
                }
            }
            RowLayout {
                Text {
                    Layout.preferredWidth: 100
                    text: "Size:"
                    horizontalAlignment: Text.AlignRight
                    font: Style.textTheme.body1
                    color: Style.colorWhite
                }
                BasicSlider {
                    id: sizeSlider
                    from: 6
                    to: 128
                    stepSize: 1
                    snapMode: Slider.SnapAlways
                    value: 16
                }
            }
        }

        ColumnLayout {
            BasicSwitch {
                id: boldSwitch
                text: "Bold"
            }

            BasicSwitch {
                id: italicSwitch
                text: "Italic"
            }

            BasicSwitch {
                id: underlineSwitch
                text: "Underline"
            }
        }
    }

    BasicSeparator {
        orientation: Qt.Horizontal
    }

    BasicSeparator {
        orientation: Qt.Horizontal
    }

    BasicLabel {
        Layout.alignment: Qt.AlignHCenter
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredWidth: 600
        Layout.preferredHeight: 300
        font.family: root.currentFontFamily
        font.weight: weightSlider.value
        font.pixelSize: Style.roundToIconSize(sizeSlider.value)
        font.bold: boldSwitch.checked
        font.italic: italicSwitch.checked
        font.underline: underlineSwitch.checked
        color: Style.colorWhite
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        text: "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum."
    }
}
