import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    spacing: 10

    BasicTabBar {
        BasicTabButton {
            text: "Text 1"
        }
        BasicTabButton {
            text: "Text 2"
        }
        BasicTabButton {
            text: "Text 3"
        }
    }

    BasicTabBar {
        BasicTabButton {
            icon.source: MaterialIcons.rocket
            text: "Icon 1"
        }
        BasicTabButton {
            icon.source: MaterialIcons.car
            text: "Icon 2"
        }
        BasicTabButton {
            icon.source: MaterialIcons.bike
            text: "Icon 3"
        }
    }

    BasicTabBar {
        BasicTabButton {
            icon.source: MaterialIcons.rocket
        }
        BasicTabButton {
            icon.source: MaterialIcons.car
        }
        BasicTabButton {
            icon.source: MaterialIcons.bike
        }
    }


    BasicTabBar {
        position: TabBar.Footer
        BasicTabButton {
            icon.source: MaterialIcons.rocket
            text: "Footer 1"
        }
        BasicTabButton {
            icon.source: MaterialIcons.car
            text: "Footer 2"
        }
        BasicTabButton {
            icon.source: MaterialIcons.bike
            text: "Footer 3"
        }
    }
}
