pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import L01_Gallery

Placeholder {
    id: root

    implicitWidth: Math.min(640, parent.width)
    implicitHeight: Math.min(400, parent.height)

    // icon.source: MaterialIcons.loading
    // text: ("Loading")

    // Repeater {
    //     model: 100
    //     delegate: Item {
    //         StandardObjectModel {
    //             StandardObject {icon: MaterialIcons.deleteSweep; text: ("Reset"); hint: ("Vider le buffer d'envoi de la machine")}
    //             StandardObject {icon: MaterialIcons.calendarClock; text: ("Time/Date"); hint: ("Envoyer l'heure et la date à la machine")}
    //             StandardObject {icon: MaterialIcons.earth; text: ("Locale"); hint: ("Envoyer la langue à la machine")}
    //             StandardObject {icon: MaterialIcons.connection; text: ("Enquiry"); hint: ("Envoyer un enquiry à la machine")}

    //             StandardObject {icon: MaterialIcons.power; text: ("Off"); hint: ("Envoyer un Off à la machine")}
    //             StandardObject {icon: MaterialIcons.replay; text: ("Start"); hint: ("Envoyer un Start à la machine")}
    //             StandardObject {icon: MaterialIcons.restart; text: ("On"); hint: ("Envoyer un On à la machine")}
    //             StandardObject {icon: MaterialIcons.sleep; text: ("Sleep"); hint: ("Envoyer un Sleep à la machine")}

    //             StandardObject {icon: MaterialIcons.informationOutline; text: ("Start-up"); hint: ("Lire les informations")}
    //             StandardObject {icon: MaterialIcons.accountOutline; text: ("User"); hint: ("Lire l'utilisateur")}
    //         }
    //     }
    // }

    // StandardObjectModel {
    //     StandardObject { text: "Hello there"; value: 1 }
    //     StandardObject { text: "Hello there"; value: 2 }
    //     StandardObject { text: "Hello there"; value: 3 }
    //     StandardObject { text: "Hello there"; value: 4; visible: false }
    //     StandardObject { text: "Hello there"; value: 5; visible: false }
    //     StandardObject { text: "General kenobi"; value: 6 }
    //     StandardObject { text: "Hello there"; value: 7 }
    //     StandardObject { text: "Hello there"; value: 8
    //         StandardObject { text: "Hello there"; value: 11 }
    //         StandardObject { text: "Hello there"; value: 12 }
    //         StandardObject { text: "General kenobi"; value: 13 }
    //         StandardObject { text: "Hello there"; value: 14 }
    //         StandardObject { text: "General kenobi"; value: 15 }
    //         StandardObject { text: "Hello there"; value: 16; visible: false }
    //         StandardObject { text: "Hello there"; value: 17; visible: false }
    //         StandardObject { text: "Hello there"; value: 18
    //             StandardObject { text: "Hello there"; value: 21 }
    //             StandardObject { text: "Hello there"; value: 22
    //                 StandardObject { text: "General kenobi"; value: 31 }
    //                 StandardObject { text: "Hello there"; value: 32
    //                     StandardObject { text: "Hello there"; value: 41; visible: false }
    //                     StandardObject { text: "Hello there"; value: 42 }
    //                 }
    //                 StandardObject { text: "General kenobi"; value: 33 }
    //                 StandardObject { text: "General kenobi"; value: 34 }
    //             }
    //         }
    //         StandardObject { text: "Hello there"; value: 19 }
    //     }
    //     StandardObject { text: "Hello there"; value: 91 }
    //     StandardObject { text: "Hello there"; value: 92 }
    //     StandardObject { text: "Hello there"; value: 93 }
    //     StandardObject { text: "Hello there"; value: 94 }
    // }

    // Repeater {
    //     anchors.fill: parent
    //     model: 100
    //     delegate: Item {
    //         anchors.fill: parent
    //         SimpleNestView {}
    //         SimpleTreeView {}
    //     }
    // }

    // Instantiator {
    //     model: 100
    //     delegate: QtObject {
    //         // readonly property FormObjectModel formDemoModel: Form_DemoModel {}
    //         readonly property TableViewColumnModel columnModel: TableViewColumnModel {}
    //         readonly property FormObjectModel formModel: FormObjectModel {}
    //         readonly property StandardObjectModel standardTreeModel: StandardObjectModel {}
    //         readonly property StandardObject standardObject: StandardObject {}
    //         readonly property ObjectNestModel nestModel: ObjectNestModel {}
    //         readonly property ObjectListModel objectModel: ObjectListModel {}
    //         readonly property ObjectTreeModel treeModel: ObjectTreeModel {}
    //         readonly property StandardObject treeObject: StandardObject {}
    //         readonly property Rectangle rectangle: Rectangle {}
    //         readonly property Item item: Item {}
    //         readonly property QtObject object: QtObject {}
    //         readonly property ItemContainer itemContainer: ItemContainer {}
    //         readonly property VariantListModel variantModel: VariantListModel {}
    //     }
    // }

    enum Parameters {
        Length = 1024,
        Iterations = 32768,

        Category0 = 0xf0f,
        Category1 = 0xf0f0,
        Category2 = 0xf0f0f,
        Maximum   = 0xf0f0f0,
        Mask      = 0xabcdef
    }

    function randomNumber(): int {
        return (Math.random() * Categorizer.Maximum);
    }

    property list<double> numbers: {
        var result = [];
        for (var i = 0; i < Categorizer.Length; ++i)
            result[i] = randomNumber();
        return result;
    }

    function sum(): list<double> {
        var cat1Sum = 0;
        var cat2Sum = 0;
        var cat3Sum = 0;
        var huge = 0;
        for (var i = 0; i < Categorizer.Iterations; ++i) {
            for (var j = 0; j < Categorizer.Length; ++j) {
                var num = root.numbers[j] & Categorizer.Mask;
                if (num < Categorizer.Category0)
                    cat1Sum += num;
                else if (num < Categorizer.Category1)
                    cat2Sum += num;
                else if (num < Categorizer.Category2)
                    cat3Sum += num;
                else
                    huge += num;
            }
        }

        return [cat1Sum, cat2Sum, cat3Sum, huge];
    }

    Component.onCompleted: {
        Log.startElapsed()
        var result = sum();
        var text = ("Categorizer took %1 ms").arg(Log.elapsed())

        root.text = text
        root.icon.source = MaterialIcons.check
    }
}
