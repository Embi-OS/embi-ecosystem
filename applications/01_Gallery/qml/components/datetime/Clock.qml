import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    implicitWidth: Math.min(800, parent.width)
    implicitHeight: Math.min(10 + sizeSlider.height + weightSlider.height + sizeSlider.to, parent.height)

    BasicSlider {
        id: sizeSlider
        from: 12
        to: 200
        stepSize: 1
        value: 32
    }
    BasicSlider {
        id: weightSlider
        anchors.topMargin: 5
        anchors.top: sizeSlider.bottom
        from: 100
        to: 900
        stepSize: 100
        snapMode: Slider.SnapAlways
        value: 300
    }

    NumericClock {
        id: clock
        anchors.topMargin: 5
        anchors.top: weightSlider.bottom
        height: sizeSlider.value
        fontWeight: weightSlider.value
        hour: DateTimeUtils.hour
        minute: DateTimeUtils.minute
        second: DateTimeUtils.second
        secondeVisible: true
    }
}
