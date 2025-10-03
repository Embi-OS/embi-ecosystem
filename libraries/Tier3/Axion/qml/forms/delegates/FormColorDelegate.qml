pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.Color
    defaultValue: "#FF0000"

    required property AbstractItemModel colorsModel
    property bool displayName: false
    property bool mandatory: true
    readonly property int currentIndex: root.colorsModel.ModelHelper.contentIsEmpty ? -1 : root.colorsModel.ModelHelper.indexOf("color",root.currentValue)

    warning: mandatory && currentIndex<0

    delegate: FormColor {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning

        displayName: root.displayName
        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        colorsModel: root.colorsModel
        color: root.currentValue

        onAccepted: (color) => root.changeValue(color)
    }
}
