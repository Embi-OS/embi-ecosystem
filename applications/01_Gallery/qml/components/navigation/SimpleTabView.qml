import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import L01_Gallery

Item {
    id: root

    implicitWidth: Math.min(640, parent.width)
    implicitHeight: Math.min(400, parent.height)

    component ComplexDelegate: Rectangle {
        Grid{
            anchors.fill: parent
            columns: width/5
            Repeater {
                model: 6000
                delegate: Rectangle {
                    height: 5
                    width: 5
                    color:  Qt.rgba(Math.random(),
                                    Math.random(),
                                    Math.random(), 1);
                }
            }
        }
    }

    PaneTabView {
        anchors.fill: parent
        tabsModel: StandardObjectModel {
            StandardObject {
                text: ("About")
                icon: MaterialIcons.informationOutline
                delegate: ComplexDelegate {
                    color: "red"
                }
            }
            StandardObject {
                text: ("Tabs")
                icon: MaterialIcons.tab

                asynchronous: false
                delegate: BasicTabView {
                    interactive: true

                    ViewDelegateLoader {
                        StandardObject.icon: MaterialIcons.abacus
                        StandardObject.text: "red"
                        ComplexDelegate {
                            color: "red"
                        }
                    }

                    ViewDelegateLoader {
                        StandardObject.visible: false
                        StandardObject.icon: MaterialIcons.lightbulb
                        StandardObject.text: "yellow"
                        ComplexDelegate {
                            color: "yellow"
                        }
                    }

                    ComplexDelegate {
                        StandardObject.icon: MaterialIcons.rocketLaunch
                        StandardObject.text: "green"
                        color: "green"
                    }

                    ComplexDelegate {
                        StandardObject.icon: MaterialIcons.account
                        StandardObject.text: "blue"
                        color: "blue"
                    }

                    ViewDelegateLoader {
                        StandardObject.icon: MaterialIcons.clock
                        StandardObject.text: "categorizer"
                        Categorizer {
                            textFont: Style.textTheme.headline5
                        }
                    }
                }
            }
            StandardObject {
                text: ("Browser")
                icon: MaterialIcons.folderSearchOutline
                delegate: ComplexDelegate {
                    color: "yellow"
                }
            }
        }
    }
}
