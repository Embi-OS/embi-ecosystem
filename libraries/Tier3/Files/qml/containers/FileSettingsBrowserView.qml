pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Files

import Qt.labs.folderlistmodel

PaneSplitView {
    id: root

    model: null

    readonly property FolderTreeObject currentFolder: standardPathsModel.at(currentIndex) as FolderTreeObject
    readonly property url currentRootUrl: currentFolder?.fileUrl ?? ""
    property alias folderUrl: folderListModel.folder
    readonly property string folderPath: Filesystem.pathFromUrl(root.folderUrl)
    property url currentUrl
    property string currentPath

    onFolderUrlChanged: root.currentPath = ""

    FolderListModel {
        id: folderListModel

        // rootFolder: root.currentRootUrl
        caseSensitive: true
        showDirs: true
        showDirsFirst: showDirsFirst.checked
        showDotAndDotDot: false
        showFiles: true
        showHidden: showHidden.checked
        showOnlyReadable: showOnlyReadable.checked
        sortField: sortName.checked ? FolderListModel.Name :
                   sortType.checked ? FolderListModel.Time :
                   sortSize.checked ? FolderListModel.Size :
                   sortTime.checked ? FolderListModel.Type : FolderListModel.Unsorted
        sortCaseSensitive: sortCaseSensitive.checked
        sortReversed: !sortAscending.checked
    }

    FolderTreeModel {
        id: standardPathsModel
        delayed: true
        path: ""
        showRootDrives: true
        showSnapPackageDrives: false
        showUnmountedAutofsDrives: false
        showTmpfsDrives: false
        showOverlayDrives: false
        showBootDrives: false
        showConfigDrives: false
        showReadOnlyDrives: false
        showQrcDrives: true
        showStandardPaths: true
        showApplicationDirPath: true
    }

    onCurrentFolderChanged: {
        if (!currentFolder)
            return
        var url = currentFolder.fileUrl
        folderChangeTransition.direction = "down"
        root.folderUrl = url
    }

    function clear() {
        folderChangeTransition.direction = "up"
        root.folderUrl = root.currentRootUrl
    }

    function down(url: url) {
        folderChangeTransition.direction = "down"
        root.folderUrl = url
    }

    function up(url: url) {
        if(root.folderUrl===root.currentRootUrl)
            return;
        folderChangeTransition.direction = "up"
        root.folderUrl = url
    }

    header: RowContainer {
        drawFrame: true
        ClickableIcon {
            icon: MaterialIcons.arrowUpBold
            onClicked: root.up(folderListModel.parentFolder)
        }
        BasicLabel {
            Layout.fillWidth: true
            text: root.folderUrl
            elide: Text.ElideLeft
        }
        BasicLabel {
            visible: folderListModel.status===FolderListModel.Ready
            text: ("%1 élément(s)").arg(folderListModel.count)
        }
        BasicBusyIndicator {
            visible: folderListModel.status===FolderListModel.Loading
            size: 24
            lineWidth: 0
        }

        ClickableIcon {
            icon: MaterialIcons.refresh
            onClicked: standardPathsModel.select()
        }
        ClickableIcon {
            icon: MaterialIcons.sortVariant
            onClicked: sortMenu.open()

            BasicMenu {
                id: sortMenu
                ExtraPosition.position: ItemPositions.BottomEnd

                ButtonGroup { id: typeGroup }
                ButtonGroup { id: sortGroup }

                BasicRadioDelegate {id: typeList; text: qsTr("Liste"); ButtonGroup.group: typeGroup; checked: true}
                BasicRadioDelegate {id: typeGrid; text: qsTr("Grille"); ButtonGroup.group: typeGroup}
                BasicMenuSeparator {}
                BasicRadioDelegate {id: sortName; text: qsTr("Nom"); ButtonGroup.group: sortGroup; checked: true}
                BasicRadioDelegate {id: sortType; text: qsTr("Type"); ButtonGroup.group: sortGroup}
                BasicRadioDelegate {id: sortSize; text: qsTr("Taille"); ButtonGroup.group: sortGroup}
                BasicRadioDelegate {id: sortTime; text: qsTr("Dernière modification"); ButtonGroup.group: sortGroup}
                BasicMenuSeparator {}
                BasicCheckDelegate {id: sortAscending; text: qsTr("Ascendant"); checked: true}
                BasicCheckDelegate {id: sortCaseSensitive; text: qsTr("Sensible à la casse")}
                BasicCheckDelegate {id: showDirsFirst; text: qsTr("Dossier en premier"); checked: true}
            }
        }
        ClickableIcon {
            icon: MaterialIcons.dotsVertical
            onClicked: settingsMenu.open()

            BasicMenu {
                id: settingsMenu
                ExtraPosition.position: ItemPositions.BottomEnd

                BasicMenuItem {text: qsTr("Aller en haut"); onClicked: root.clear()}
                BasicMenuItem {text: qsTr("Copier le chemin"); onClicked: Filesystem.pathClipboard=root.folderPath}
                BasicMenuSeparator {}
                BasicCheckDelegate {id: showHidden; text: qsTr("Voir les fichier cachés")}
                BasicCheckDelegate {id: showOnlyReadable; text: qsTr("Voir les fichier en lecture seul")}
                BasicMenuSeparator {}
                BasicMenuItem {text: qsTr("Propriétés")}
            }
        }
    }

    Transition {
        id: folderChangeTransition
        property string direction: "up"
        ParallelAnimation {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 250 }
            NumberAnimation { property: "x"; from: folderChangeTransition.direction==="up" ? -20 : 20; duration: 250 }
        }
    }

    TextFile {
        id: textFile
    }

    function open(filePath: string) {
        textFile.open(filePath, TextFileOpenModes.Read)
        var settings = {
            "textFormat": textFile.fileType==="md" ? TextEdit.MarkdownText :
                          textFile.fileType==="html" ? TextEdit.RichText : TextEdit.PlainText,
            "title": textFile.fileName,
            "message": textFile.error ? textFile.error : textFile.fileUrl,
            "infos": textFile.content,
            "fullscreen": true,
            "diagnose": false
        }
        DialogManager.showText(settings);
    }

    function remove(filePath: string) {
        var settings = {
            "message": qsTr("Êtes-vous sûr de vouloir supprimer le fichier suivant ?"),
            "infos": filePath,
            "buttonAccept": qsTr("Supprimer"),
            "buttonReject": qsTr("Annuler"),
            "onAccepted": function() {
                Filesystem.remove(filePath);
            }
        }
        DialogManager.showWarning(settings);
    }

    currentComponent: PaneListView {
        visible: folderListModel.status!==FolderListModel.Null
        model: folderListModel
        delegate: FolderTreeDelegate {
            width: (ListView.view as BasicListView).viewWidth
            required property int index
            required property string fileName
            required property string filePath
            required property url fileUrl
            required property int fileSize
            required property bool fileIsDir

            icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl)
            text: fileName
            info: fileIsDir ? "" : FormatUtils.bytes(fileSize)
            highlighted: root.currentUrl===fileUrl
            onClicked: {
                root.currentUrl = fileUrl
                if (fileIsDir)
                    root.down(fileUrl)
                else
                    root.currentPath = filePath
            }
        }

        viewPopulate: folderChangeTransition
        viewFooter: RowLayout {
            z: 2
            width: ListView.view.width
            LayoutSpring {}
            FabButton {
                highlighted: true
                hint: qsTr("Ajouter un élément")
                icon.source: MaterialIcons.plus
                onClicked: creationMenu.open()

                BasicMenu {
                    id: creationMenu
                    width: 200
                    ExtraPosition.position: ItemPositions.Top
                    BasicMenuItem {
                        icon.source: MaterialIcons.filePlusOutline
                        text: qsTr("Fichier")
                        onClicked: Filesystem.createFile(root.folderPath)
                    }
                    BasicMenuItem {
                        icon.source: MaterialIcons.folderPlusOutline
                        text: qsTr("Dossier")
                        onClicked: Filesystem.createDir(root.folderPath)
                    }
                }
            }
            FabButton {
                visible: root.currentPath!==""
                hint: qsTr("Ouvrir l'élément")
                icon.source: MaterialIcons.fileEyeOutline
                onClicked: root.open(root.currentPath)
            }
            FabButton {
                visible: root.currentPath!==""
                hint: qsTr("Supprimer l'élément")
                icon.source: MaterialIcons.trashCan
                onClicked: root.remove(root.currentPath)
            }
            LayoutSpring {}
        }
    }

    proxyModel.sourceModel: standardPathsModel
    proxyModel.filterRoleName: ""
    proxyModel.filterEnabled: false
    proxyModel.sortRoleName: "group"
    proxyModel.sortOrder: Qt.AscendingOrder
    listDelegate: IconTitleDelegate {
        ButtonGroup.group: root.buttonGroup
        ItemContainer.container: root.itemContainer
        opacity: 1.0
        width: (ListView.view as BasicListView).viewWidth
        required property string fileUrl
        required property string name
        required property string info
        required property int index
        text: name
        hint: info
        icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl, false)
        checkable: true
        highlighted: ListView.isCurrentItem
        onClicked: root.setCurrentIndex(index)
    }
}
