import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    property color selectedColor: Style.colorAccent

    StandardObjectModel {
        id: colorsModel

        StandardObject { text: "Red"; color: Style.red }
        StandardObject { text: "Pink"; color: Style.pink }
        StandardObject { text: "Purple"; color: Style.purple }
        StandardObject { text: "Blue"; color: Style.blue }
        StandardObject { text: "Cyan"; color: Style.cyan }
        StandardObject { text: "Teal"; color: Style.teal }
        StandardObject { text: "Green"; color: Style.green }
        StandardObject { text: "Amber"; color: Style.amber }
    }

    ColorPicker {
        id: colorPicker

        selectedColor: root.selectedColor
        colorModel: colorsModel
        alphaEnabled: alphaEnabled.checked
        swatchEnabled: swatchEnabled.checked

        onColorEdited: (color) => console.log("ColorPicker", ColorUtils.name(color, true))
    }

    BasicCheckBox {
        id: alphaEnabled
        text: "alphaEnabled"
        checked: true
    }

    BasicCheckBox {
        id: swatchEnabled
        text: "swatchEnabled"
        checked: true
    }

    BasicLabel {
        text: "selectedColor: " + ColorUtils.name(root.selectedColor, true)
    }

    BasicLabel {
        text: "solidColor: " + ColorUtils.name(colorPicker.solidColor, true)
    }

    BasicLabel {
        text: "hueColor: " + ColorUtils.name(colorPicker.hueColor, true)
    }
}
