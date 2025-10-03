pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

RowContainer {
    id: root

    required property StandardObjectModel model
    property double buttonHeight: root.drawFrame ? 50 : 70

    backgroundVisible: visibleItems>0

    fillWidth: true
    fillHeight: true
    drawFrame: false
    spacing: 10
    color: Style.colorTransparent
    borderWidth: drawFrame ? Style.contentRectangleBorderWidth : 0

    Instantiator {
        onObjectAdded: (index, object) => root.addItem(object as Item)
        onObjectRemoved: (index, object) => root.removeItem(object as Item)

        model: root.model
        delegate: RawButton {
            implicitHeight: root.buttonHeight
            implicitWidth: 1000

            inset: 0
            radius: root.drawFrame ? root.radius-root.borderWidth : Style.buttonRadius

            required property StandardObject qtObject
            required visible
            required text
            required enabled
            icon.source: qtObject?.icon ?? ""
            onClicked: qtObject.emitAction()
        }
    }
}
