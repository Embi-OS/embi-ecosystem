pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Media
import Eco.Tier3.Axion

BasicPopup {
    id: root

    required property MediaItemModel mediaPlaylist

    property bool transitionsEnabled: true
    property double transitionsDuration: 150

    function showFileTree() {
        var settings = {
            "selectionType": FolderTreeTypes.Dir,
            "showBootDrives": true,
            "showConfigDrives": true,
            "showRootDrives": true,
            "onPathSelected": function(path) {
                mediaPlaylist.addPath(path)
            }
        }
        DialogManager.showFileTree(settings);
    }

    padding: 10
    contentItem: BasicListView {
        implicitWidth: 380
        implicitHeight: 380
        spacing: 10
        model: root.mediaPlaylist

        headerPositioning: ListView.PullBackHeader
        header: RowContainer {
            inset: 5
            width: ListView.view.width
            BasicLabel {
                text: qsTr("Playlist")
                Layout.fillWidth: true
                font: Style.textTheme.title2
            }
            ClickableIcon {
                icon: MaterialIcons.trashCan
                onClicked: root.mediaPlaylist.additionalPaths = []
            }
            ClickableIcon {
                icon: MaterialIcons.plus
                onClicked: root.showFileTree()
            }
        }

        currentIndex: root.mediaPlaylist.currentIndex
        highlightMoveDuration: 250
        highlightResizeDuration: 0
        highlightFollowsCurrentItem: true

        delegate: RowLayout {
            id: row
            width: (ListView.view as BasicListView).viewWidth

            required property string path
            required property string fileName
            required property int index
            readonly property bool isCurrent: root.mediaPlaylist.currentIndex===index

            ClickableIcon {
                implicitWidth: size*1.5
                highlighted: row.isCurrent
                icon: MaterialIcons.music
                onClicked: root.mediaPlaylist.currentIndex = row.index
            }

            BasicLabel {
                Layout.fillWidth: true
                elide: Text.ElideRight
                highlighted: row.isCurrent
                text: row.fileName
            }

            ClickableIcon {
                implicitWidth: size*1.5
                highlighted: row.isCurrent
                icon: MaterialIcons.trashCan
                onClicked: root.mediaPlaylist.remove(row.index)
            }
        }

        add: Transition {
            enabled: root.transitionsEnabled
            ParallelAnimation {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: root.transitionsDuration }
                NumberAnimation { property: "scale"; from: 0.8; to: 1; duration: root.transitionsDuration }
            }
        }

        move: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { properties: "x,y"; duration: root.transitionsDuration }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }

        displaced: Transition {
            enabled: root.transitionsEnabled
            NumberAnimation { properties: "x,y"; duration: root.transitionsDuration }

            // ensure opacity and scale values return to 1.0
            NumberAnimation { property: "opacity"; to: 1.0 }
            NumberAnimation { property: "scale"; to: 1.0 }
        }
    }
}
