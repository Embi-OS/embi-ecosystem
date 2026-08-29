pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.Color
    defaultValue: "#000000"

    required property AbstractItemModel colorsModel
    property bool mandatory: true
    readonly property color currentColor: currentValue
    readonly property int currentIndex: root.colorsModel.ModelHelper.contentIsEmpty ? -1 : root.colorsModel.ModelHelper.indexOf("color",root.currentColor)

    property bool paletteOnly: false

    property bool paletteEnabled: true
    property bool saturationEnabled: !paletteOnly
    property bool hueEnabled: !paletteOnly
    property bool alphaEnabled: false
    property bool swatchEnabled: false
    property bool hexEnabled: !paletteOnly

    warning: mandatory && paletteOnly && currentIndex<0

    delegate: FormColor {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning

        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        colorsModel: root.colorsModel
        color: root.currentValue

        paletteEnabled: root.paletteEnabled
        saturationEnabled: root.saturationEnabled
        hueEnabled: root.hueEnabled
        alphaEnabled: root.alphaEnabled
        swatchEnabled: root.swatchEnabled
        hexEnabled: root.hexEnabled

        onAccepted: (color) => {root.changeValue(color); console.log(root.currentValue, root.currentColor, root.currentIndex)}
    }
}
