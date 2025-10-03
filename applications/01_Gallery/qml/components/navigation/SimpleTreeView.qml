pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

BasicTreeView {
    id: root

    implicitWidth: Math.min(640, parent.width)
    implicitHeight: Math.min(400, parent.height)

    property int value1: 0
    Timer {
        repeat: true
        running: true
        interval: 1000
        onTriggered: root.value1++
    }

    property int value2: 0
    Timer {
        repeat: true
        running: true
        interval: 800
        onTriggered: root.value2++
    }
    onValue2Changed: {
        if(value2==15)
            treeModel.insertObject(object, treeObject)
    }

    property int value3: 0
    Timer {
        repeat: true
        running: true
        interval: 600
        onTriggered: root.value3++
    }

    StandardObject {
        id: object
        text: "Hell yeah"
        value: 1000
        StandardObject { text: "Hell yeah"; value: 1 }
        StandardObject { text: "Hell yeah"; value: 2 }
        StandardObject { text: "Hell yeah"; value: root.value3 }
    }

    StandardObjectModel {
        id: treeModel
        StandardObject { text: "Hello there"; value: 1 }
        StandardObject { text: "Hello there"; value: root.value1 }
        StandardObject { text: "Hello there"; value: 3 }
        StandardObject { text: "Hello there"; value: 4 }
        StandardObject { text: "Hello there"; value: 5 }
        StandardObject { text: "General kenobi"; value: 6 }
        StandardObject { text: "Hello there"; value: 7 }
        StandardObject { text: "Hello there"; value: 8
            StandardObject { text: "Hello there"; value: 11 }
            StandardObject { text: "Hello there"; value: 12 }
            StandardObject { text: "General kenobi"; value: 13 }
            StandardObject { text: "Hello there"; value: root.value1 }
            StandardObject { id: treeObject; text: "General kenobi"; value: 15 }
            StandardObject { text: "Hello there"; value: 16 }
            StandardObject { text: "Hello there"; value: 17 }
            StandardObject { text: "Hello there"; value: 18
                StandardObject { text: "Hello there"; value: 21 }
                StandardObject { text: "Hello there"; value: root.value2
                    StandardObject { text: "General kenobi"; value: 31 }
                    StandardObject { text: "Hello there"; value: 32
                        StandardObject { text: "Hello there"; value: 41 }
                        StandardObject { text: "Hello there"; value: 42 }
                    }
                    StandardObject { text: "General kenobi"; value: 33 }
                    StandardObject { text: "General kenobi"; value: 34 }
                }
            }
            StandardObject { text: "Hello there"; value: 19 }
        }
        StandardObject { text: "Hello there"; value: 91 }
        StandardObject { text: "Hello there"; value: 92 }
        StandardObject { text: "Hello there"; value: 93 }
        StandardObject { text: "Hello there"; value: 94 }
    }

    property StandardObject selectedObject
    model: treeModel
    delegate: BasicTreeDelegate {
        required property StandardObject qtObject
        required property TreeView treeView
        required property int index
        required property int depth
        required property int value
        required hasChildren
        required expanded
        text: ("%1: %2").arg(qtObject.text).arg(value)

        // implicitWidth: (treeView as BasicTreeView).viewWidth
        leftPadding: depth * 20
        highlighted: root.selectedObject===qtObject

        onClicked: {
            root.selectedObject = qtObject
            if(expanded)
                treeView.collapseRecursively(index)
            else
                treeView.expand(index)
        }
    }

    ScrollBar.vertical: BasicScrollBar {
        visible: root.contentHeight>root.height
    }
}
