import QtQuick
import Eco.Tier3.Axion

FormTextField {
    id: root

    signal pathAccepted(string path)

    property alias path: root.text

    property int folderTreeType: FolderTreeTypes.Dir
    property list<string> additionalPaths: []

    placeholder: "Path/to/file"

    onAccepted: (text) => {
        root.pathAccepted(text)
    }

    function showPathDialog() {
        DialogManager.showFileTree({
            showApplicationDirPath: true,
            showRootDrives: true,
            selectionType: root.folderTreeType,
            additionalPaths: root.additionalPaths,
            onPathSelected: function(path) {
                root.path = path;
                root.pathAccepted(root.path);
                root.textInputFocus = false;
            }
        });
    }

    icons: [
        ClickableIcon {
            icon: MaterialIcons.folder
            onClicked: root.showPathDialog()
        }
    ]
}
