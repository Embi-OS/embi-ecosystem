pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion
import Qt.labs.folderlistmodel

ColumnLayout {
    id: root

    width: Math.min(640, parent.width)

    property string path: "qrc:/images/"

    function openQrcFinder() {
        var settings = {
            selectedPath: ":/images/",
            selectionType: FolderTreeTypes.Dir,
            showFiles: false,
            onPathSelected: function(path) {
                root.path = "qrc"+path+"/";
            }
        }
        DialogManager.showFileTree(settings);
    }

    function openFileFinder() {
        var settings = {
            selectedPath: "/home/romain-dev/A_VOH/material-symbols/svg",
            selectionType: FolderTreeTypes.Dir,
            showFiles: false,
            onPathSelected: function(path) {
                root.path = Qt.resolvedUrl(path);
            }
        }
        DialogManager.showFileTree(settings);
    }

    RowLayout {
        Layout.fillWidth: true
        RawButton {
            icon.source: MaterialIcons.folder
            text: "Open qrc:/"
            onClicked: root.openQrcFinder()
        }

        RawButton {
            icon.source: MaterialIcons.folder
            text: "Open files:/"
            onClicked: root.openFileFinder()
        }

        BasicLabel {
            font: Style.textTheme.headline5
            text: folderModel.count+" results"
        }
    }

    FormTextField {
        id: textField
        Layout.fillWidth: true
        placeholder: "search"
        onTextChanged: delaySearch.start()
    }

    BasicSeparator {
        orientation: Qt.Horizontal
    }

    GridView {
        id: grid
        clip: true
        reuseItems: true

        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.preferredHeight: 400

        cellWidth: 50
        cellHeight: 50

        ScrollIndicator.horizontal: BasicScrollIndicator {
            alwaysOn: true
        }

        Timer {
            id: delaySearch
            interval: 100
            onTriggered: function () {
                var words = textField.text.split(' ')
                var filter = '*'
                for(const word of words)
                    filter += `${word}*`
                console.log(`filter: ${filter}`)
                folderModel.nameFilters = [filter]
            }
        } // Timer

        model: FolderListModel {
            id: folderModel
            folder: root.path
            showDirs: false
            caseSensitive: false
            nameFilters: ['*.svg']
        }

        delegate: Item {
            id: icon
            width: grid.cellWidth
            height: grid.cellHeight

            required property string fileName
            required property string filePath
            required property var model
            required property int index

            property bool highlighted: false
            readonly property list<string> forbiddenKeywords: ['id', 'index', 'model', 'modelData', 'console', 'do', 'if', 'in', 'for', 'let', 'new', 'try', 'var', 'case', 'else', 'enum', 'eval', 'null', 'this', 'true', 'void', 'with', 'await', 'break', 'catch', 'class', 'const', 'false', 'super', 'throw', 'while', 'yield', 'delete', 'export', 'import', 'public', 'return', 'static', 'switch', 'typeof', 'default', 'extends', 'finally', 'package', 'private', 'continue', 'debugger', 'function', 'arguments', 'interface', 'protected', 'implements', 'instanceof']

            function fileNameToProperty(str: string): string {
                // snake to camel
                str = str.replace(/([-_][a-z0-9])/g,
                                  group => group.toUpperCase().replace('-', '').replace('_', ''))

                // remove extension
                str = str.replace(/\.[^/.]+$/, "")

                // append _ to forbidden keywords
                if (forbiddenKeywords.includes(str))
                    return `${str}_`
                return str
            }

            Rectangle {
                color: Style.colorPrimaryDarkest
                height: 40
                width: height
                anchors.centerIn: parent
                radius: height/2

                SvgColorImage {
                    anchors.centerIn: parent
                    icon: "qrc"+icon.filePath
                    size: 32
                    color: icon.highlighted ? Style.colorAccent : Style.colorWhite
                }
            }

            TapHandler {
                cursorShape: Qt.PointingHandCursor
                onTapped: icon.highlighted = !icon.highlighted
            }

            HoverHandler {
                id: hoverHandler
                cursorShape: Qt.PointingHandCursor
            }

            BasicToolTip {
                ExtraPosition.position: ItemPositions.Top
                text: `${icon.fileNameToProperty(icon.fileName)}`
                visible: hoverHandler.hovered
            }
        }
    }
}
