pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier3.Axion

RawButton {
    id: root

    property alias menu: menu
    required property PaneTableView tableView
    property double cellHeight: densityStandard.value

    readonly property bool opened: menu.opened
    readonly property StandardObject currentObject: densityModel.at(densityModel.indexOfValue(cellHeight))

    text: qsTr("Densité")
    icon.source: currentObject?.icon ?? ""
    onClicked: menu.open()

    SettingsMapper {
        settingsCategory: "TableView"
        property alias cellHeight: root.cellHeight
    }

    Binding {
        delayed: true
        target: root.tableView
        property: "cellHeight"
        value: root.cellHeight
    }

    StandardObjectModel {
        id: densityModel
        StandardObject { id: densityCompact; text: qsTr("Compact"); icon: MaterialIcons.viewHeadline; value: Style.tableCellHeightCompact}
        StandardObject { id: densityStandard; text: qsTr("Standard"); icon: MaterialIcons.viewSequential; value: Style.tableCellHeightStandard}
        StandardObject { id: densityMedium; text: qsTr("Moyen"); icon: MaterialIcons.viewStream; value: Style.tableCellHeightMedium}
        StandardObject { id: densityComfortable; text: qsTr("Confortable"); icon: MaterialIcons.viewAgenda; value: Style.tableCellHeightComfortable}
    }

    ButtonGroup {
        id: densityGroup
    }

    BasicMenu {
        id: menu
        ExtraPosition.position: ItemPositions.BottomEnd
        Instantiator {
            active: menu.opened
            model: densityModel
            onObjectAdded: (index, object) => menu.insertItem(index, object as Item)
            onObjectRemoved: (index, object) => menu.removeItem(object as Item)
            delegate: BasicRadioDelegate {
                required property StandardObject qtObject
                required property double value
                required text
                checked: value===root.cellHeight
                icon.source: qtObject?.icon ?? ""
                ButtonGroup.group: densityGroup
                onClicked: {
                    if(checked) {
                        root.cellHeight = value
                    }
                }
            }
        }
    }
}
