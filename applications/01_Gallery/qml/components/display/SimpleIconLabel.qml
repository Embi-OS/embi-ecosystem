import QtQuick
import Eco.Tier3.Axion

Column {
    id: root

    spacing: 8

    property int display: {
        if (iconOnlyRadio.checked)
            return AbstractButton.IconOnly
        if (textOnlyRadio.checked)
            return AbstractButton.TextOnly
        if (textBesideIconRadio.checked)
            return AbstractButton.TextBesideIcon
        if (textUnderIconRadio.checked)
            return AbstractButton.TextUnderIcon
    }

    Row {
        id: customizationRow

        spacing: 8

        GroupBox {
            title: "Icon Label"

            width: 200
            height: customizationBox.height

            Column {
                anchors.centerIn: parent
                width: parent.width

                spacing: 16

                IconLabel {
                    id: iconLabel

                    mirrored: mirroredButton.checked

                    enabled: enabledButton.checked
                    text: `Icon Label`
                    icon.source: MaterialIcons.rocketLaunchOutline

                    icon.width: iconSizeSlider.value
                    icon.height: iconSizeSlider.value

                    spacing: spacingSlider.value
                    display: root.display

                    DebugRectangle {}
                }

                IconLabel {
                    mirrored: mirroredButton.checked

                    enabled: enabledButton.checked
                    text: `Icon Label`
                    icon.source: MaterialIcons.rocketLaunchOutline

                    icon.width: iconSizeSlider.value
                    icon.height: iconSizeSlider.value

                    width: Math.min(100, implicitWidth)

                    spacing: spacingSlider.value
                    display: root.display

                    DebugRectangle {}
                }

                IconLabel {
                    id: imageLabel

                    mirrored: mirroredButton.checked

                    enabled: enabledButton.checked
                    text: "Custom Image"

                    icon.width: iconSizeSlider.value
                    icon.height: iconSizeSlider.value
                    icon.source: MaterialIcons.abacus

                    spacing: spacingSlider.value
                    display: root.display

                    DebugRectangle {
                        anchors.fill: parent
                    }
                }

                IconLabel {
                    id: customLabel

                    mirrored: mirroredButton.checked

                    enabled: enabledButton.checked
                    text: "Custom Label"
                    font: Style.textTheme.overline

                    icon.width: iconSizeSlider.value
                    icon.height: iconSizeSlider.value

                    spacing: spacingSlider.value
                    display: root.display

                    DebugRectangle {}
                }
            }
        }

        GroupBox {
            id: customizationBox
            title: "Customization"

            Row {
                spacing: 8

                Frame {
                    Column {
                        BasicRadioButton {
                            id: iconOnlyRadio
                            text: "IconOnly"
                        }
                        BasicRadioButton {
                            id: textOnlyRadio
                            text: "TextOnly"
                        }
                        BasicRadioButton {
                            id: textBesideIconRadio
                            text: "TextBesideIcon"
                            checked: true
                        }
                        BasicRadioButton {
                            id: textUnderIconRadio
                            text: "TextUnderIcon"
                        }
                    }
                }

                Frame {
                    Column {
                        BasicCheckBox {
                            id: enabledButton

                            text: "enabled"
                            checked: true
                        }

                        BasicCheckBox {
                            id: mirroredButton

                            text: "mirrored"
                            checked: false
                        }

                        BasicLabel {
                            font: Style.textTheme.body2
                            text: "Spacing"
                        }

                        BasicSlider {
                            id: spacingSlider
                            from: 0
                            to: 16
                            stepSize: 1
                            value: 8
                        }

                        BasicLabel {
                            font: Style.textTheme.body2
                            text: "icon size"
                        }

                        BasicSlider {
                            id: iconSizeSlider
                            from: 0
                            to: 64
                            stepSize: 1
                            value: 24
                        }
                    }
                }
            }
        }
    }

    GroupBox {
        width: customizationRow.width
        title: "Showcase Implicit Size"

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 8
            IconLabel {
                anchors.verticalCenter: parent.verticalCenter
                mirrored: false

                text: `IconOnly`
                icon.source: MaterialIcons.rocketLaunchOutline
                display: AbstractButton.IconOnly

                DebugRectangle {
                    anchors.fill: parent
                }
            }

            IconLabel {
                anchors.verticalCenter: parent.verticalCenter
                mirrored: false

                text: `TextOnly`
                icon.source: MaterialIcons.rocketLaunchOutline
                display: AbstractButton.TextOnly

                DebugRectangle {
                    anchors.fill: parent
                }
            }

            IconLabel {
                anchors.verticalCenter: parent.verticalCenter
                mirrored: false

                text: `TextBesideIcon`
                icon.source: MaterialIcons.rocketLaunchOutline
                display: AbstractButton.TextBesideIcon

                DebugRectangle {
                    anchors.fill: parent
                }
            }

            IconLabel {
                anchors.verticalCenter: parent.verticalCenter
                mirrored: false

                text: `TextUnderIcon`
                icon.source: MaterialIcons.rocketLaunchOutline
                display: AbstractButton.TextUnderIcon

                DebugRectangle {
                    anchors.fill: parent
                }
            }
        }
    }
    GroupBox {
        width: customizationRow.width
        title: "Showcase Explicit Size"

        Column {
            anchors.horizontalCenter: parent.horizontalCenter

            BasicSlider {
                id: widthSlider

                anchors.horizontalCenter: parent.horizontalCenter

                from: 10
                to: 150
                value: 100
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter

                spacing: 8
                IconLabel {
                    width: widthSlider.value
                    anchors.verticalCenter: parent.verticalCenter
                    mirrored: true

                    text: `IconOnly`
                    icon.source: MaterialIcons.rocketLaunchOutline
                    display: AbstractButton.IconOnly

                    DebugRectangle {
                        anchors.fill: parent
                    }
                }

                IconLabel {
                    width: widthSlider.value
                    anchors.verticalCenter: parent.verticalCenter
                    mirrored: true

                    text: `TextOnly`
                    icon.source: MaterialIcons.rocketLaunchOutline
                    display: AbstractButton.TextOnly

                    DebugRectangle {
                        anchors.fill: parent
                    }
                }

                IconLabel {
                    width: widthSlider.value
                    anchors.verticalCenter: parent.verticalCenter
                    mirrored: true

                    text: `TextBesideIcon`
                    icon.source: MaterialIcons.rocketLaunchOutline
                    display: AbstractButton.TextBesideIcon

                    DebugRectangle {
                        anchors.fill: parent
                    }
                }

                IconLabel {
                    width: widthSlider.value
                    anchors.verticalCenter: parent.verticalCenter
                    mirrored: true

                    text: `TextUnderIcon`
                    icon.source: MaterialIcons.rocketLaunchOutline
                    display: AbstractButton.TextUnderIcon

                    DebugRectangle {
                        anchors.fill: parent
                    }
                }
            }
        }
    }
}
