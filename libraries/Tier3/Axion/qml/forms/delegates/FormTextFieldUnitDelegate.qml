pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier2.Unit
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.TextFieldUnit
    defaultValue: ({
        "value": defaultUnitValue,
        "type": defaultUnitType
    })

    required property UnitCategoryObject unitOptions
    property string valueTargetProperty: ""
    property string typeTargetProperty: ""
    readonly property bool useDualProperty: valueTargetProperty!=="" || typeTargetProperty!==""

    property double defaultUnitValue: 0
    property alias currentUnitValue: valueItem.currentValue
    property alias unitValue: valueItem.value
    readonly property double currentDisplayValue: root.displayValue(currentUnitValue)
    readonly property string currentDisplay: unitOptions.display(currentUnitValue, currentUnitType)

    property int defaultUnitType: UnitTypes.NoUnit
    property alias currentUnitType: unitItem.currentValue
    property alias unitType: unitItem.value
    property bool unitEditable: true
    readonly property string currentUnitName: unitOptions.name(currentUnitType)
    readonly property string currentUnitAbbreviation: unitOptions.abbreviation(currentUnitType)

    signal unitValueAccepted(unitValue: var)
    signal unitTypeAccepted(unitType: var)

    warning: valueItem.warning || unitItem.warning

    FormObject {
        id: valueItem
        formType: FormTypes.None
        valueType: FormValueTypes.Double
        defaultValue: root.defaultUnitValue
        visible: root.visible
        enabled: root.enabled
        validator: root.validator
        selectPolicy: root.selectPolicy
        submitPolicy:  root.submitPolicy
        revertPolicy: root.revertPolicy
        target: root.target
        selectWhen: root.selectWhen
        submitWhen: root.submitWhen
        revertWhen: root.revertWhen
        targetProperty: root.valueTargetProperty
        onAccepted: (value) => root.unitValueAccepted(value)
        warning: !acceptableInput
    }

    FormObject {
        id: unitItem
        formType: FormTypes.None
        valueType: FormValueTypes.Integer
        defaultValue: root.defaultUnitType
        visible: root.visible
        enabled: root.enabled
        selectPolicy: root.selectPolicy
        submitPolicy:  root.submitPolicy
        revertPolicy: root.revertPolicy
        target: root.target
        selectWhen: root.selectWhen
        submitWhen: root.submitWhen
        revertWhen: root.revertWhen
        targetProperty: root.typeTargetProperty
        onAccepted: (value) => root.unitTypeAccepted(value)
        warning: !acceptableInput
    }

    onUnitTypeChanged: {
        unitItem.loadCurrentValue()
    }

    function changeUnitValueType(unitValue: double, unitType: int): bool {
        const unitValueType = ({
            "value": unitValue,
            "type": unitType
        })
        return root.changeValue(unitValueType);
    }

    function displayValue(value: double): double {
        return root.unitOptions.convertTo(value, root.currentUnitType)
    }

    delegate: FormTextFieldUnit {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable

        enabled: root.enabled
        warning: root.warning

        errorMessage: root.validator ? root.validator.ExtraValidator.errorMessage : ""

        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        value: root.currentDisplayValue

        unitCurrentIndex: root.unitOptions.ModelHelper.indexOf(unitValueRole,root.currentUnitType)
        unitOptions: root.unitOptions
        unitEditable: root.unitEditable

        onUnitActivated: (index) => {
            unitItem.changeValue(unitCurrentValue);
            root.changeUnitValueType(root.currentUnitValue, unitCurrentValue)
        }
        onValueEdited: (value) => {
            const rawValue = root.unitOptions.convertFrom(value, root.currentUnitType)
            valueItem.validateValue(rawValue)
        }
        onValueAccepted: (value) => {
            const rawValue = root.unitOptions.convertFrom(value, root.currentUnitType)
            valueItem.changeValue(rawValue);
            root.changeUnitValueType(rawValue, root.currentUnitType)
        }
    }
}
