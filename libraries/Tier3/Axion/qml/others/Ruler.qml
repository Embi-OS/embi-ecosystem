pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion
import Qt.labs.qmlmodels

Item {
    id: root

    implicitWidth: 60
    implicitHeight: 60

    property alias from: rulerModel.from
    property alias to: rulerModel.to
    property alias decimal: rulerModel.decimal

    property font font: Style.textTheme.body2
    property color valueColor: Style.colorWhite

    RulerModel {
        id: rulerModel
        delayed: true
        pixelSize: root.height
        from: 0
        to: 1
        decimal: 3
    }

    Item {
        id: layout
        anchors.fill: parent

        readonly property int bigTickHeight: 3
        readonly property int mediumTickHeight: 1
        readonly property int smallTickHeight: 1
        readonly property int bigTickWidth: 15
        readonly property int mediumTickWidth: 15
        readonly property int smallTickWidth: 7

        component BasicTickItem: Item {
            id: tick
            y: position
            implicitWidth: root.width
            implicitHeight: 0

            property alias tickRect: tickRect

            required property double position
            required property int type

            readonly property bool isBigTick: type===RulerTickTypes.Big
            readonly property bool isMediumTick: type===RulerTickTypes.Medium
            readonly property bool isSmallTick: type===RulerTickTypes.Small

            Rectangle {
                id: tickRect
                anchors.right: parent.right
                anchors.verticalCenter: parent.top

                color: root.valueColor
                height: tick.isBigTick ? layout.bigTickHeight :
                        tick.isMediumTick ? layout.mediumTickHeight :
                        tick.isSmallTick ? layout.smallTickHeight : 5
                width: tick.isBigTick ? layout.bigTickWidth :
                       tick.isMediumTick ? layout.mediumTickWidth :
                       tick.isSmallTick ? layout.smallTickWidth : 5
            }
        }

        Repeater {
            id: repeater
            model: rulerModel
            delegate: DelegateChooser {
                role: "type"

                DelegateChoice {
                    roleValue: RulerTickTypes.Big
                    BasicTickItem {
                        id: bigTick
                        required property string display
                        Text {
                            parent: bigTick.tickRect
                            anchors.right: parent.left
                            anchors.rightMargin: 5
                            anchors.verticalCenter: parent.verticalCenter
                            font: root.font
                            color: root.valueColor
                            horizontalAlignment: Qt.AlignRight
                            text: bigTick.display
                        }
                    }
                }
                DelegateChoice {
                    BasicTickItem {}
                }
            }
        }
    }
}
