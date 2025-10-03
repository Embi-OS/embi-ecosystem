pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.Slider
    defaultValue: 0.0

    required property double from
    required property double to
    property double stepSize: 0
    property int snapMode: Slider.NoSnap

    delegate: FormSlider {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning

        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        from: root.from
        to: root.to
        stepSize: root.stepSize
        snapMode: root.snapMode
        value: root.currentValue

        onMoved: (value) => root.changeValue(value)
    }
}
