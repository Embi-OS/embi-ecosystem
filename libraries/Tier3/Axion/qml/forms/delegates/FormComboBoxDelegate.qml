pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.ComboBox
    defaultValue: ""

    placeholder: qsTr("Sélectionner un élément")

    required property AbstractItemModel options
    required property string textRole
    property string valueRole: textRole
    property bool isSorted: false
    property string emptyText: "N/A"
    property bool mandatory: true
    readonly property int currentIndex: root.options.ModelHelper.contentIsEmpty ? -1 : root.options.ModelHelper.indexOf(root.valueRole,root.currentValue,root.isSorted)
    readonly property string currentText: root.currentIndex>=0 ? root.options.ModelHelper.getProperty(root.currentIndex, root.textRole) : ""

    warning: mandatory && currentIndex<0

    delegate: FormComboBox {
        fitLabel: root.fitLabel
        labelWidthRatio: root.labelWidthRatio
        editable: root.editable
        authorizeEmpty: !root.mandatory
        emptyText: root.emptyText

        enabled: root.enabled
        warning: root.warning || !authorizeEmpty && (currentIndex<0 || currentIndex>=count)

        validator: root.validator
        label: root.label
        infos: root.infos
        placeholder: root.placeholder
        options: root.options
        textRole: root.textRole
        valueRole: root.valueRole!=="" ? root.valueRole : root.textRole
        currentIndex: root.currentIndex

        onTextEdited: (text) => root.validateValue(text)
        onActivated: root.changeValue(currentValue)
    }
}
