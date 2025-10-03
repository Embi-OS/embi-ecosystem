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
            treeObject.append(object)
    }

    property int value3: 0
    Timer {
        repeat: true
        running: true
        interval: 600
        onTriggered: root.value3++
    }

    component NestObject: ObjectNestModel {
        // property string text: ""
        // property int value: 0
    }

    NestObject {
        id: object
        text: "Hell yeah"
        value: 1000
        NestObject { text: "Hell yeah"; value: 1 }
        NestObject { text: "Hell yeah"; value: 2 }
        NestObject { text: "Hell yeah"; value: root.value3 }
    }

    ObjectNestModel {
        id: treeModel
        NestObject { text: "Hello there"; value: 1 }
        NestObject { text: "Hello there"; value: root.value1 }
        NestObject { text: "Hello there"; value: 3 }
        NestObject { text: "Hello there"; value: 4 }
        NestObject { text: "Hello there"; value: 5 }
        NestObject { text: "General kenobi"; value: 6 }
        NestObject { text: "Hello there"; value: 7 }
        NestObject { text: "Hello there"; value: 8
            NestObject { text: "Hello there"; value: 11 }
            NestObject { text: "Hello there"; value: 12 }
            NestObject { text: "General kenobi"; value: 13 }
            NestObject { text: "Hello there"; value: root.value1 }
            NestObject { id: treeObject; text: "General kenobi"; value: 15 }
            NestObject { text: "Hello there"; value: 16 }
            NestObject { text: "Hello there"; value: 17 }
            NestObject { text: "Hello there"; value: 18
                NestObject { text: "Hello there"; value: 21 }
                NestObject { text: "Hello there"; value: root.value2
                    NestObject { text: "General kenobi"; value: 31 }
                    NestObject { text: "Hello there"; value: 32
                        NestObject { text: "Hello there"; value: 41 }
                        NestObject { text: "Hello there"; value: 42 }
                    }
                    NestObject { text: "General kenobi"; value: 33 }
                    NestObject { text: "General kenobi"; value: 34 }
                }
            }
            NestObject { text: "Hello there"; value: 19 }
        }
        NestObject { text: "Hello there"; value: 91 }
        NestObject { text: "Hello there"; value: 92 }
        NestObject { text: "Hello there"; value: 93 }
        NestObject { text: "Hello there"; value: 94 }
    }

    property NestObject selectedObject
    model: treeModel
    delegate: BasicTreeDelegate {
        required property NestObject qtObject
        required property TreeView treeView
        required property int index
        required property int depth
        required hasChildren
        required expanded
        required property int value
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
