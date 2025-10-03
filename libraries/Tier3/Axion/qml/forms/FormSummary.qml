import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

Item {
    id: root

    property int spacing: 4
    property int iconSize: 64
    property font labelFont: Style.textTheme.body2
    property color color: root.warning ? Style.colorWarning : Style.colorWhite

    required property FormObject formObject

    enabled: formObject?.enabled ?? false
    readonly property int formType: formObject?.formType ?? FormTypes.None
    readonly property string label: formObject?.label ?? ""
    readonly property string icon: formObject?.icon ?? ""
    readonly property bool warning: formObject?.warning ?? false
    readonly property var currentValue: formObject?.currentValue ?? ""

    // FormTypes.Switch
    readonly property bool checked: formObject?.checked ?? false
    readonly property string leftLabel: formObject?.leftLabel ?? ""
    readonly property string rightLabel: formObject?.rightLabel ?? ""
    readonly property string currentSwitchLabel: checked ? rightLabel : leftLabel

    // FormTypes.ComboBox
    readonly property string currentComboBoxText: formObject?.currentText ?? ""

    // FormTypes.TextFieldUnit
    readonly property string currentUnitDisplay: formObject?.currentDisplay ?? ""

    implicitHeight: Style.formSummaryHeight
    implicitWidth: iconLabel.implicitContentWidth

    IconLabel {
        id: iconLabel
        anchors.centerIn: parent
        spacing: root.spacing
        color: root.color
        display: AbstractButton.TextUnderIcon
        font: root.labelFont
        text: root.formType===FormTypes.Switch ? root.currentSwitchLabel :
              root.formType===FormTypes.ComboBox ? root.currentComboBoxText :
              root.formType===FormTypes.TextFieldUnit ? root.currentUnitDisplay : root.currentValue
        icon.width: root.iconSize
        icon.height: root.iconSize
        icon.source: root.icon
    }
}
