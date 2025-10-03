import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    implicitWidth: Math.min(layout.width, parent.width)
    implicitHeight: Math.min(layout.height, parent.height)

    SystemColorDialog {
        id: dialogsColor
        title: "ColorDialog"

        options: dontUseNativeDialog.checked ? SystemColorDialog.DontUseNativeDialog : 0
    }

    SystemFileDialog {
        id: dialogsFile
        title: "FileDialog"

        options: dontUseNativeDialog.checked ? SystemFileDialog.DontUseNativeDialog : 0
    }

    SystemFolderDialog {
        id: dialogsFolder
        title: "FolderDialog"

        options: dontUseNativeDialog.checked ? SystemFolderDialog.DontUseNativeDialog : 0
    }

    SystemFontDialog {
        id: dialogsFont
        title: "FontDialog"

        options: dontUseNativeDialog.checked ? SystemFontDialog.DontUseNativeDialog : 0
    }

    SystemMessageDialog {
        id: dialogsMessage
        title: "MessageDialog"
    }

    ColumnLayout {
        id: layout
        anchors.centerIn: parent

        BasicSwitch {
            id: dontUseNativeDialog
            text: "Don't use native dialog"
        }

        RawButton {
            Layout.fillWidth: true
            text: ("Dialog color")
            color: Style.blue
            icon.source: MaterialIcons.invertColors
            onClicked: dialogsColor.open()
        }
        RawButton {
            Layout.fillWidth: true
            text: ("Dialog file")
            color: Style.green
            icon.source: MaterialIcons.file
            onClicked: dialogsFile.open()
        }
        RawButton {
            Layout.fillWidth: true
            text: ("Dialog folder")
            color: Style.orange
            icon.source: MaterialIcons.folder
            onClicked: dialogsFolder.open()
        }
        RawButton {
            Layout.fillWidth: true
            text: ("Dialog font")
            color: Style.pink
            icon.source: MaterialIcons.formatLetterCase
            onClicked: dialogsFont.open()
        }
        RawButton {
            Layout.fillWidth: true
            text: ("Dialog message")
            color: Style.cyan
            icon.source: MaterialIcons.message
            onClicked: dialogsMessage.open()
        }
        RawButton {
            Layout.fillWidth: true
            text: ("Dialog saveFileContent")
            color: Style.indigo
            icon.source: MaterialIcons.message
            onClicked: DialogManager.saveFileContent("Hello world!", "test.txt")
        }
    }
}
