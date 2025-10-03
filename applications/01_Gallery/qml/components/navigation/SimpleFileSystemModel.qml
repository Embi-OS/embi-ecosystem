pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

PaneTreeView {
    id: root

    implicitWidth: Math.min(640, parent.width)
    implicitHeight: Math.min(400, parent.height)

    drawFrame: true

    property string selectedUrl

    model: FileSystemModel {}
    delegate: FolderTreeDelegate {
        required property TreeView treeView
        required property int index
        required property int depth
        required property string fileUrl
        required property string fileName
        required property string fileSize
        required hasChildren
        required expanded

        implicitWidth: treeView.width
        leftPadding: depth * 20

        icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl, expanded)
        text: fileName
        info: fileSize
        highlighted: root.selectedUrl===fileUrl

        onClicked: {
            root.selectedUrl = fileUrl
            if(expanded)
                treeView.collapse(index)
            else
                treeView.expand(index)
        }
    }
}
