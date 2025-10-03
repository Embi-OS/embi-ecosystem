import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    width: 300

    component SimpleImage: Image {
        id: image
        fillMode: Image.PreserveAspectFit
        antialiasing: true
        asynchronous: true

        RowLayout {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height/10
            spacing: 0

            Repeater {
                model: imageColorsHelper.colors
                delegate: Rectangle {
                    id: colorRectangle
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    required property color modelData
                    color: modelData
                    BasicLabel {
                        anchors.centerIn: parent
                        relativeBackgroundColor: colorRectangle.color
                        font: Style.textTheme.caption2
                        text: colorRectangle.color
                    }
                }
            }
        }

        BasicBusyIndicator {
            anchors.centerIn: parent
            visible: !imageColorsHelper.isReady
        }

        ImageColorsHelper {
            id: imageColorsHelper
            source: image.source
            backgroundColor: Style.colorBackground
            textColor: Style.colorWhite
            fallbackDominant: "#BC1142"
            fallbackDominantComplementary: "#00B1FF"
        }
    }

    SimpleImage {
        sourceSize.height: 150
        sourceSize.width: 200
        source: "qrc:/images/wallpaper/wavy.png"
    }

    SimpleImage {
        sourceSize.height: 150
        sourceSize.width: 200
        source: "qrc:/images/wallpaper/gradient.png"
    }

    SimpleImage {
        sourceSize.height: 150
        sourceSize.width: 200
        source: "qrc:/images/wallpaper/neonSunset.png"
    }

    SimpleImage {
        sourceSize.height: 150
        sourceSize.width: 200
        source: "qrc:/images/wallpaper/fallingIntoWater.png"
    }
}
