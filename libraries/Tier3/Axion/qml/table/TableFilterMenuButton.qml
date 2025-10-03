pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

RawButton {
    id: root

    property alias menu: menu
    property alias filters: filters
    required property TableViewColumnModel columnModel
    readonly property bool opened: menu.opened

    property var filterMap: ({})

    text: qsTr("Filtres (%1)").arg(filters.count)
    icon.source: MaterialIcons.filter
    onClicked: menu.open()

    signal accepted()

    onAccepted: root.filterMap = getFilterMap()

    function getFilterMap(): var {
        var map = {}
        for( var i = 0; i < filters.rowCount(); i++ ) {
            const filterColumn = filters.ModelHelper.getProperty(i, "filterColumn")
            const filterRoleName = filters.ModelHelper.getProperty(i, "filterRoleName")
            const filterOperator = filters.ModelHelper.getProperty(i, "filterOperator")
            const filterValue = filters.ModelHelper.getProperty(i, "filterValue")
            const filterEnabled = filters.ModelHelper.getProperty(i, "filterEnabled")

            if(!filterEnabled)
                continue

            const column = root.columnModel.at(filterableColumns.mapToSource(filterColumn)) as TableViewColumn
            if(!column)
                continue

            var name = column.formattedField(filterOperator)
            map[name] = filterValue
        }
        return map
    }

    function addFilter() {
        var filter = {
            "filterColumn": -1,
            "filterRoleName": "",
            "filterOperator": "",
            "filterValue": "",
            "filterEnabled": true
        }
        filters.append(filter)
    }

    function removeFilter(index: int) {
        filters.remove(index)
    }

    function clearFilter() {
        filters.clear()
    }

    ListModel {
        id: filters
    }

    ProxyModel {
        id: filterableColumns
        sourceModel: root.columnModel
        filterRoleName: "filterable"
        filterValue: true
        delayed: true
    }

    StandardObjectModel {
        id: operators
        StandardObject { text: qsTr("Egal à"); value: ""}
        StandardObject { text: qsTr("Différent de"); value: "neq"}
        StandardObject { text: qsTr("Plus petit que"); value: "lt"}
        StandardObject { text: qsTr("Plus petit ou égal à"); value: "lte"}
        StandardObject { text: qsTr("Plus grand que"); value: "gt"}
        StandardObject { text: qsTr("Plus grand ou égal à"); value: "gte"}
        StandardObject { text: qsTr("Contient"); value: "icontains"}
        StandardObject { text: qsTr("Commence par"); value: "istartswith"}
        StandardObject { text: qsTr("Termine par"); value: "iendswith"}
        StandardObject { text: qsTr("Expression régulière"); value: "regex"}
    }

    BasicMenu {
        id: menu
        ExtraPosition.position: ItemPositions.BottomEnd
        width: 800

        onClosed: root.accepted()

        RowContainer {
            padding: 4
            RawButton {
                icon.source: MaterialIcons.plus
                text: qsTr("Ajouter")
                highlighted: true
                onClicked: root.addFilter()
            }
            LayoutSpring {}
            RawButton {
                icon.source: MaterialIcons.deleteForever
                text: qsTr("Tout supprimer")
                highlighted: true
                onClicked: root.clearFilter()
            }
        }

        BasicMenuSeparator {}

        Instantiator {
            active: menu.opened
            model: filters
            onObjectAdded: (index, object) => menu.addItem(object as Item)
            onObjectRemoved: (index, object) => menu.removeItem(object as Item)
            delegate: RowContainer {
                id: filter
                padding: 4

                required property var model
                required property int index
                required property int filterColumn
                required property string filterOperator
                required property string filterRoleName
                required property string filterValue
                required property bool filterEnabled

                ClickableIcon {
                    icon: MaterialIcons.close
                    onClicked: root.removeFilter(filter.index)
                }
                FormComboBox {
                    Layout.fillWidth: true
                    enabled: filter.filterEnabled
                    placeholder: qsTr("Colonne")
                    options: filterableColumns
                    textRole: "title"
                    valueRole: "roleName"
                    borderColorDefault: Style.colorPrimaryLight
                    currentIndex: filter.filterColumn
                    onActivated: {
                        filters.setProperty(filter.index, "filterColumn", currentIndex)
                        filters.setProperty(filter.index, "filterRoleName", currentValue)
                    }
                }
                FormComboBox {
                    Layout.fillWidth: true
                    enabled: filter.filterEnabled
                    placeholder: qsTr("Opération")
                    options: operators
                    textRole: "text"
                    valueRole: "value"
                    borderColorDefault: Style.colorPrimaryLight
                    onActivated: filters.setProperty(filter.index, "filterOperator", currentValue)
                    Component.onCompleted: currentIndex = indexOfValue(filter.filterOperator)
                }
                FormTextField {
                    Layout.fillWidth: true
                    enabled: filter.filterEnabled
                    placeholder: qsTr("Valeur")
                    text: filter.filterValue
                    borderColorDefault: Style.colorPrimaryLight
                    onAccepted: (text) => filters.setProperty(filter.index, "filterValue", text)
                }
                BasicCheckBox {
                    checked: filter.filterEnabled
                    onClicked: filters.setProperty(filter.index, "filterEnabled", checked)
                }
            }
        }
    }
}
