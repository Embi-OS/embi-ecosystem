pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import Qt.labs.folderlistmodel
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    width: Math.min(640, parent.width)

    property url iconsFolder: "qrc:/images/"
    readonly property var reservedPropertyNames: [
        "await", "break", "case", "catch", "class", "const", "continue", "debugger", "default",
        "delete", "do", "else", "enum", "eval", "export", "extends", "false", "finally", "for",
        "function", "if", "import", "in", "instanceof", "let", "new", "null", "return", "static",
        "super", "switch", "this", "throw", "true", "try", "typeof", "var", "void", "while", "with",
        "yield", "arguments", "console", "id", "index", "model", "modelData", "package", "private",
        "protected", "public", "interface", "implements"
    ]

    function iconPropertyName(fileName: string): string {
        const name = fileName
            .replace(/([-_][a-z0-9])/g, group => group.toUpperCase().replace("-", "").replace("_", ""))
            .replace(/\.[^/.]+$/, "");
        return reservedPropertyNames.includes(name) ? name + "_" : name;
    }

    function openQrcFinder() {
        DialogManager.showFileTree({
            selectedPath: ":/images/",
            selectionType: FolderTreeTypes.Dir,
            showFiles: false,
            onPathSelected: function(path) {
                root.iconsFolder = "qrc" + path + "/";
            }
        });
    }

    function openFileFinder() {
        DialogManager.showFileTree({
            selectionType: FolderTreeTypes.Dir,
            showFiles: false,
            onPathSelected: function(path) {
                root.iconsFolder = Filesystem.urlFromPath(path);
            }
        });
    }

    RowLayout {
        Layout.fillWidth: true

        RawButton {
            icon.source: MaterialIcons.folder
            text: qsTr("Open qrc:/")
            onClicked: root.openQrcFinder()
        }

        RawButton {
            icon.source: MaterialIcons.folder
            text: qsTr("Open folder")
            onClicked: root.openFileFinder()
        }

        BasicLabel {
            Layout.fillWidth: true
            font: Style.textTheme.headline5
            text: qsTr("%1 icon(s)").arg(folderModel.count)
        }
    }

    FormTextField {
        id: searchField

        Layout.fillWidth: true
        placeholder: qsTr("Search an icon")
        onTextChanged: folderModel.nameFilters = text.length > 0 ? ["*" + text + "*.svg"] : ["*.svg"]
    }

    BasicSeparator {
        Layout.fillWidth: true
        orientation: Qt.Horizontal
    }

    GridView {
        id: grid

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 400

        clip: true
        reuseItems: true
        cellWidth: 64
        cellHeight: 64

        ScrollIndicator.vertical: BasicScrollIndicator {
            alwaysOn: true
        }

        model: FolderListModel {
            id: folderModel

            folder: root.iconsFolder
            showDirs: false
            caseSensitive: false
            nameFilters: ["*.svg"]
        }

        delegate: Item {
            id: icon

            width: grid.cellWidth
            height: grid.cellHeight

            required property string fileName
            required property url fileUrl

            Rectangle {
                anchors.centerIn: parent
                width: 40
                height: width
                radius: width / 2
                color: Style.colorPrimaryDarkest

                SvgColorImage {
                    anchors.centerIn: parent
                    icon: icon.fileUrl
                    size: 32
                    color: Style.colorWhite
                }
            }

            HoverHandler {
                id: hoverHandler
            }

            BasicToolTip {
                ExtraPosition.position: ItemPositions.Top
                text: "MaterialIcons." + root.iconPropertyName(icon.fileName)
                visible: hoverHandler.hovered
            }
        }
    }
}
