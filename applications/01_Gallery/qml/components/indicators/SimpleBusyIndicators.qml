import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    spacing: 20

    RowLayout {
        spacing: 20

        BasicBusyIndicator {}

        BasicProgressBar {
            from: 0
            to: 10000
            NumberAnimation on value {
                running: true
                loops: NumberAnimation.Infinite
                from: 0
                to: 10000
                duration: 5000
            }
        }
    }

    RowLayout {
        spacing: 20

        BasicBusyIndicator {}

        BasicProgressBar {
            indeterminate: true
        }
    }

    RowLayout {
        spacing: 20

        BasicBusyIndicator {
            lineWidth: 4
        }

        BasicProgressBar {
            lineWidth: 4
            indeterminate: true
        }
    }

    RowLayout {
        spacing: 20

        BasicBusyIndicator {
            capStyle: Qt.FlatCap
        }

        BasicProgressBar {
            capStyle: Qt.FlatCap
            indeterminate: true
        }
    }
}
