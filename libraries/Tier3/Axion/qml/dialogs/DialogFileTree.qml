pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal pathSelected(string path)

    title: qsTr("Fichiers")
    message: selectionType===FolderTreeTypes.File ? qsTr("Sélectionner un fichier") :
             selectionType===FolderTreeTypes.Dir ? qsTr("Sélectionner un dossier") : ""
    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")

    property FolderTreeObject selectedElement: null
    property int selectionType: FolderTreeTypes.All

    property alias folderTreeModel: folderTreeModel
    FolderTreeModel {
        id: folderTreeModel
        delayed: true
        enabled: root.visible
        path: ""
    }

    onAccepted: {
        if(root.selectedElement)
            root.pathSelected(root.selectedElement.filePath)
        else
            root.pathSelected(folderTreeModel.path)
    }

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; text: root.buttonReject },
        ButtonDialog {
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; text: root.buttonAccept; highlighted: true;
            enabled: !root.selectedElement ? (folderTreeModel.path!=="" && root.selectionType!==FolderTreeTypes.File)  :
                     root.selectionType===FolderTreeTypes.All ? true :
                     root.selectionType===FolderTreeTypes.Dir ? root.selectedElement.isDir :
                     root.selectionType===FolderTreeTypes.File ? !root.selectedElement.isDir : false
        }
    ]

    padding: Style.contentRectangleBorderWidth
    contentColor: Style.colorPrimaryDark

    contentItem: PaneTreeView {
        title: root.folderTreeModel.path

        padding: Style.contentRectangleBorderWidth
        spacing: 0
        implicitHeight: 400
        implicitWidth: 800

        model: root.folderTreeModel
        delegate: FolderTreeDelegate {
            required property TreeView treeView
            required property int index
            required property int depth
            required property FolderTreeObject qtObject
            required property string fileUrl
            required hasChildren
            required expanded
            required text
            required info

            leftPadding: depth * 20
            icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl, expanded)
            highlighted: root.selectedElement===qtObject

            onClicked: {
                root.selectedElement = qtObject
                if(expanded)
                    treeView.collapseRecursively(index)
                else
                    treeView.expand(index)
            }
        }
    }
}
