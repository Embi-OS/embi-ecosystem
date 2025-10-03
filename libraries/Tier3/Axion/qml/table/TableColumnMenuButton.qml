pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

RawButton {
    id: root

    property alias menu: menu
    property alias matcherRoleName: visibleMatcher.roleName
    property alias matcherValue: visibleMatcher.value

    required property AbstractItemModel model
    readonly property bool opened: menu.opened
    property string title: qsTr("Colonnes")

    property Component delegate: BasicCheckDelegate {
        required property TableViewColumn qtObject
        required property string title
        text: title
        icon.source: qtObject?.icon ?? ""
        checked: qtObject?.visible ?? false
        onClicked: qtObject.visible=checked
    }

    text: qsTr("%1 (%2/%3)").arg(title).arg(visibleMatcher.count).arg(model.ModelHelper.count)
    icon.source: MaterialIcons.viewColumn
    onClicked: menu.open()

    ModelMatcher {
        id: visibleMatcher
        delayed: true
        sourceModel: root.model
        roleName: "visible"
        value: true
    }

    BasicMenu {
        id: menu
        ExtraPosition.position: ItemPositions.BottomEnd
        Instantiator {
            active: menu.opened
            model: root.model
            onObjectAdded: (index, object) => menu.insertItem(index, object as Item)
            onObjectRemoved: (index, object) => menu.removeItem(object as Item)
            delegate: root.delegate
        }
    }
}
