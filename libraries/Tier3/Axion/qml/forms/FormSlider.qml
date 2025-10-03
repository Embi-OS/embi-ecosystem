import QtQuick
import Eco.Tier3.Axion

BasicFormBackground {
    id: root

    signal moved(real value)

    property alias from: slider.from
    property alias to: slider.to
    property alias stepSize: slider.stepSize
    property alias value: slider.value
    property alias snapMode: slider.snapMode
    property alias readOnly: slider.readOnly

    Item {
        parent: root.controlRectangle
        anchors.fill: parent

        BasicSlider {
            id: slider
            anchors.fill: parent
            anchors.margins: Style.formInnerMargin
            readOnly: !root.editable
            onMoved: root.moved(value)
        }
    }
}
