import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    width: 2 * bottomLeft.width + rectangle.width + 2 * bottomRight.width
    height: 2 * top.height + rectangle.height + 2 * bottom.height + label.height + 20 + layout.height + 20

    BasicLabel {
        id: label
        x: (parent.width - width) / 2
        y: 5
        width: parent.width
        horizontalAlignment: Text.AlignHCenter

        font: Style.textTheme.caption1
        wrapMode: Text.Wrap
        elide: Text.ElideRight
        text: "ToolTip are displayed spaced apart by \"margins/2\" pixels from their parent's border (default: margins = 12)"
    }

    Rectangle {
        id: rectangle
        anchors.centerIn: parent
        width: 370
        height: 150
        color: "transparent"
        border.color: Style.colorPrimary
        radius: 4

        FlatButton {
            id: topLeft
            x: -width
            y: -height
            text: "TopLeft"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "TopLeft"
                visible: topLeft.hovered || topLeft.pressed
                ExtraPosition.position: ItemPositions.TopLeft
            }
        }

        FlatButton {
            id: topStart
            x: 0
            y: -height
            text: "TopStart"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "TopStart"
                visible: topStart.hovered || topStart.pressed
                ExtraPosition.position: ItemPositions.TopStart
            }
        }

        FlatButton {
            id: top
            x: (parent.width - width) / 2
            y: -height
            text: "Top"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "Top"
                visible: top.hovered || top.pressed
                ExtraPosition.position: ItemPositions.Top
            }
        }

        FlatButton {
            id: topEnd
            x: parent.width - width
            y: -height
            text: "TopEnd"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "TopEnd"
                visible: topEnd.hovered || topEnd.pressed
                ExtraPosition.position: ItemPositions.TopEnd
            }
        }

        FlatButton {
            id: topRight
            x: parent.width
            y: -height
            text: "TopRight"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "TopRight"
                visible: topRight.hovered || topRight.pressed
                ExtraPosition.position: ItemPositions.TopRight
            }
        }

        FlatButton {
            id: rightStart
            x: parent.width
            y: 0
            text: "RightStart"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "RightStart"
                visible: rightStart.hovered || rightStart.pressed
                ExtraPosition.position: ItemPositions.RightStart
            }
        }

        FlatButton {
            id: right
            x: parent.width
            y: (parent.height - height) / 2
            text: "Right"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "Right"
                visible: right.hovered || right.pressed
                ExtraPosition.position: ItemPositions.Right
            }
        }

        FlatButton {
            id: rightEnd
            x: parent.width
            y: parent.height - height
            text: "RightEnd"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "RightEnd"
                visible: rightEnd.hovered || rightEnd.pressed
                ExtraPosition.position: ItemPositions.RightEnd
            }
        }

        FlatButton {
            id: bottomRight
            x: parent.width
            y: parent.height
            text: "BottomRight"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "BottomRight"
                visible: bottomRight.hovered || bottomRight.pressed
                ExtraPosition.position: ItemPositions.BottomRight
            }
        }

        FlatButton {
            id: bottomEnd
            x: parent.width - width
            y: parent.height
            text: "BottomEnd"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "BottomEnd"
                visible: bottomEnd.hovered || bottomEnd.pressed
                ExtraPosition.position: ItemPositions.BottomEnd
            }
        }

        FlatButton {
            id: bottom
            x: (parent.width - width) / 2
            y: parent.height
            text: "Bottom"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "Bottom"
                visible: bottom.hovered || bottom.pressed
                ExtraPosition.position: ItemPositions.Bottom
            }
        }

        FlatButton {
            id: bottomStart
            x: 0
            y: parent.height
            text: "BottomStart"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "BottomStart"
                visible: bottomStart.hovered || bottomStart.pressed
                ExtraPosition.position: ItemPositions.BottomStart
            }
        }

        FlatButton {
            id: bottomLeft
            x: -width
            y: parent.height
            text: "BottomLeft"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "BottomLeft"
                visible: bottomLeft.hovered || bottomLeft.pressed
                ExtraPosition.position: ItemPositions.BottomLeft
            }
        }

        FlatButton {
            id: leftEnd
            x: -width
            y: parent.height - height
            text: "LeftEnd"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "LeftEnd"
                visible: leftEnd.hovered || leftEnd.pressed
                ExtraPosition.position: ItemPositions.LeftEnd
            }
        }

        FlatButton {
            id: left
            x: -width
            y: (parent.height - height) / 2
            text: "Left"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "Left"
                visible: left.hovered || left.pressed
                ExtraPosition.position: ItemPositions.Left
            }
        }

        FlatButton {
            id: leftStart
            x: -width
            y: 0
            text: "LeftStart"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "LeftStart"
                visible: leftStart.hovered || leftStart.pressed
                ExtraPosition.position: ItemPositions.LeftStart
            }
        }

        FlatButton {
            id: center
            anchors.centerIn: parent
            text: "Center"
            highlighted: false
            flat: true

            BasicToolTip {
                text: "Center"
                visible: center.hovered || center.pressed
                ExtraPosition.position: ItemPositions.Center
            }
        }
    }

    RowLayout {
        id: layout
        anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height-height - 5
        FlatButton {
            id: button1
            text: "Simple"
            highlighted: false

            BasicToolTip {
                ExtraPosition.position: ItemPositions.Top
                text: "Simple ToolTip"
                visible: button1.hovered || button1.pressed
            }
        }
        FlatButton {
            id: button2
            text: "Delayed"
            highlighted: false

            BasicToolTip {
                ExtraPosition.position: ItemPositions.Top
                text: "ToolTip Delayed"
                delay: 300
                visible: button2.hovered || button2.pressed
            }
        }
        FlatButton {
            id: button3
            text: "Timeout"
            highlighted: false

            BasicToolTip {
                ExtraPosition.position: ItemPositions.Top
                text: "ToolTip Timeout"
                visible: button3.hovered || button3.pressed
                timeout: 800
            }
        }
        FlatButton {
            id: button4
            text: "Customized"
            highlighted: false

            BasicToolTip {
                id: customizedTooltip
                ExtraPosition.position: ItemPositions.Top
                text: "Customized ToolTip"
                visible: button4.hovered || button4.pressed
                font: Style.textTheme.hint2

                leftPadding: 4
                rightPadding: 6

                contentItem: IconLabel {
                    text: customizedTooltip.text
                    font: customizedTooltip.font
                    icon.source: MaterialIcons.accountCircle
                    color: Style.colorBlack
                }

                background: Rectangle {
                    implicitHeight: 24
                    color: Style.toolTipColor
                    radius: height / 2
                }
            }
        }
    }
}
