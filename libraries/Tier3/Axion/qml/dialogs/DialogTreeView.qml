pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

BasicDialog {
    id: root

    required property StandardObjectModel treeModel

    title: ""
    message: ""
    property string buttonReject: ""
    property string buttonAccept: qsTr("Fermer")

    property bool rejectVisible: buttonReject!==""
    property bool acceptVisible: buttonAccept!==""

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; visible: root.rejectVisible; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; visible: root.acceptVisible; text: root.buttonAccept; highlighted: true }
    ]

    padding: Style.contentRectangleBorderWidth
    contentColor: Style.colorPrimaryDark

    contentItem: PaneTreeView {
        padding: Style.contentRectangleBorderWidth
        implicitHeight: 400
        implicitWidth: 800

        model: ProxyModel {
            delayed: true
            sourceModel: root.treeModel
            filterRoleName: "visible"
            filterValue: true
        }
    }
}
