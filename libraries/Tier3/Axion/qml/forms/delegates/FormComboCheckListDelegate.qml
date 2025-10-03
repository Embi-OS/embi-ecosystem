pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.ComboCheckList
    defaultValue: ""

    placeholder: qsTr("Sélectionner un/des élément(s)")

    required property AbstractItemModel options
    required property string textRole
    property string valueRole: textRole
    property bool isSorted: false
    property bool authorizeGlobal: true
    property list<var> editableValues: []
    property string globalText: qsTr("Select all")
    property string separator: "; "
    property bool mandatory: false
    readonly property bool hasSelection: currentIndexes.length>0
    readonly property list<var> currentValues: options.ModelHelper.getProperties(currentIndexes, valueRole)
    readonly property list<var> currentTexts: options.ModelHelper.getProperties(currentIndexes, textRole)
    readonly property list<int> currentIndexes: {
        var indexes=[]
        if(root.options.ModelHelper.contentIsEmpty) {
            return indexes;
        }
        for(const value of root.currentValue) {
            indexes.push(root.options.ModelHelper.indexOf(root.valueRole,value,root.isSorted))
        }
        return indexes
    }

    warning: mandatory && !hasSelection

    delegate: FormComboCheckList {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable
        authorizeEmpty: !root.mandatory
        authorizeGlobal: root.authorizeGlobal
        editableValues: root.editableValues
        globalText: root.globalText
        separator: root.separator

        enabled: root.enabled
        warning: root.warning

        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        options: root.options
        textRole: root.textRole
        valueRole: root.valueRole!=="" ? root.valueRole : root.textRole
        currentIndexes: root.currentIndexes

        onAccepted: root.changeValue(currentValues)
    }
}
