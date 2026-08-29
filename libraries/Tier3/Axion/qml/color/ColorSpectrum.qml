import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property real inset: 6
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset
    padding: 6

    signal moved(real saturation, real value)

    property real hue: 0.0
    property real saturation: 1.0
    property real value: 1.0
    readonly property color hueColor: ColorUtils.fromHsvF(hue, 1.0, 1.0, 1.0)

    property int radius: 4

    function clamp(value: real, minimum: real, maximum: real): real {
        return Math.max(minimum, Math.min(maximum, value))
    }

    function moveAt(posX: real, posY: real) {
        let w = contentItem.width;
        let h = contentItem.height;
        if (w <= 0 || h <= 0)
            return
        let x = clamp(posX, 0.0, w);
        let y = clamp(posY, 0.0, h);
        let saturation = x / w;
        let value = 1.0 - (y / h);

        root.moved(saturation, value)
    }

    background: Item {
        implicitWidth: 150
        implicitHeight: 150
    }

    contentItem: Rectangle {
        radius: root.radius
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 1.0; color: root.hueColor }
        }

        Rectangle {
            anchors.fill: parent
            radius: root.radius
            border.width: Style.containerBorderWidth
            border.color: Style.colorPrimaryLight
            gradient: Gradient {
                orientation: Gradient.Vertical
                GradientStop { position: 0.0; color: "transparent" }
                GradientStop { position: 1.0; color: "black" }
            }
        }

        ColorPickerHandle {
            x: root.clamp(root.saturation, 0.0, 1.0) * parent.width - width / 2
            y: (1.0 - root.clamp(root.value, 0.0, 1.0)) * parent.height - height / 2
            implicitWidth: 18
            implicitHeight: 18
        }

        MouseArea {
            anchors.fill: parent
            onPressed: (mouse) => root.moveAt(mouse.x, mouse.y)
            onPositionChanged: (mouse) => {
                if(pressed)
                    root.moveAt(mouse.x, mouse.y)
            }
        }
    }
}
