import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    property bool filled: tabBar.currentIndex===1
    property bool outlined: tabBar.currentIndex===2

    BasicTabBar {
        id: tabBar
        Layout.alignment: Qt.AlignHCenter

        BasicTabButton {
            text: "Standard"
            width: implicitWidth
        }
        BasicTabButton {
            text: "Filled"
            width: implicitWidth
        }
        BasicTabButton {
            text: "Outlined"
            width: implicitWidth
        }
    }

    ColumnLayout {
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredWidth: Style.snackbarWidth

        Snackbar {
            Layout.fillWidth: true

            filled: root.filled
            outlined: root.outlined

            severity: SnackbarSeverities.None
            title: "Message"
            caption: "Un message peut apparaître ici, il est possible qu'il soit extrèmement long."
            closable: true
        }

        Snackbar {
            Layout.fillWidth: true

            filled: root.filled
            outlined: root.outlined

            severity: SnackbarSeverities.Info
            title: "Hello there"
        }

        Snackbar {
            Layout.fillWidth: true

            filled: root.filled
            outlined: root.outlined

            severity: SnackbarSeverities.Warning
            title: "Warning"
            caption: "Un message avec un bouton pour effectuer une action"
            button: "OK"
        }

        Snackbar {
            Layout.fillWidth: true

            filled: root.filled
            outlined: root.outlined

            severity: SnackbarSeverities.Error
            title: "Error"
            caption: "Un message d'erreur"
        }

        Snackbar {
            Layout.fillWidth: true

            filled: root.filled
            outlined: root.outlined

            severity: SnackbarSeverities.Fatal
            caption: "Un message d'erreur critique"
        }

        Snackbar {
            Layout.fillWidth: true

            filled: root.filled
            outlined: root.outlined

            severity: SnackbarSeverities.Success
            caption: "Un message de succès"
        }
    }
}
