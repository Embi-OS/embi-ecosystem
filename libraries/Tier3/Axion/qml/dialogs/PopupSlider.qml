import QtQuick
import Eco.Tier3.Axion

BasicPopup {
    id: root

    signal moved(real value)

    property alias from: slider.from
    property alias to: slider.to
    property alias stepSize: slider.stepSize
    property alias value: slider.value
    property alias snapMode: slider.snapMode
    property alias readOnly: slider.readOnly

    contentItem: RowLayout {
        spacing: root.spacing
        BasicLabel {
            Layout.alignment: Qt.AlignVCenter
            padding: slider.padding
            font: root.font
            text: FormatUtils.realToString(root.value,1)
        }
        BasicSlider {
            id: slider
            Layout.alignment: Qt.AlignVCenter
            onMoved: root.moved(value)
        }
    }
}
