pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

PaneTreeView {
    id: root

    width: Math.min(640, parent.width)
    height: Math.min(400, parent.height)

    drawFrame: true

    property string selectedUrl

    model: FolderTreeModel {
        id: folderTreeModel
        delayed: true
        // path: ":/"

        path: ""
        showRootDrives: true
        showSnapPackageDrives: false
        showUnmountedAutofsDrives: false
        showTmpfsDrives: false
        showOverlayDrives: false
        showBootDrives: true
        showConfigDrives: true
        showReadOnlyDrives: false
        showQrcDrives: true
        showStandardPaths: true
        showApplicationDirPath: true
    }
    delegate: FolderTreeDelegate {
        required property TreeView treeView
        required property int index
        required property int depth
        required property string fileUrl
        required hasChildren
        required expanded
        required text
        required info

        implicitWidth: treeView.width
        leftPadding: depth * 20

        icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl, expanded)
        highlighted: root.selectedUrl===fileUrl

        onClicked: {
            root.selectedUrl = fileUrl
            if(expanded)
                treeView.collapseRecursively(index)
            else
                treeView.expand(index)
        }
    }
}
