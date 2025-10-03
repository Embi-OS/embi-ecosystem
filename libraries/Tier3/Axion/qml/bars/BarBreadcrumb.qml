pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

RowContainer {
    id: root

    required property StandardObjectModel model
    property bool navigationLocked: false

    signal clicked(int navIndex)

    edge: Qt.LeftEdge
    drawFrame: true

    spacing: -5

    Repeater {
        id: repeater
        model: root.model
        delegate: IconTitleDelegate {
            required property StandardObject qtObject
            required property int index
            required enabled
            required text
            // icon.source: qtObject.icon
            showIndicator: index<(repeater.count-1)

            font: Style.textTheme.button
            icon.width: 24
            icon.height: 24

            onClicked: {
                if(root.navigationLocked)
                    return
                qtObject.emitAction()
                if(!highlighted)
                    root.clicked(index)
            }
        }
    }
}
