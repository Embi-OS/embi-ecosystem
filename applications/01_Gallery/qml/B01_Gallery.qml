pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.Utils
import Eco.Tier3.Axion
import L01_Gallery

BasicPane {
    id: root

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants globaux
//──────────────────────────────────────────────────────────────────────

    header: RowContainer {
        drawFrame: true
        spacing: 0
        radius: 0
        fillHeight: true
        LayoutSpring {}

        RawButton {
            highlighted: true
            text: "Clear"
            onClicked: loader.clear()
        }

        RawButton {
            highlighted: true
            text: "Reload"
            onClicked: loader.reload()
        }

        BasicSeparator { color: Style.colorPrimaryLight }

        BasicToolButton {
            id: asynchronousLoad
            highlighted: checked
            icon.source: MaterialIcons.sync
            text: "Async"
            onClicked: loader.reload()
        }

        BasicSeparator { color: Style.colorPrimaryLight }

        BasicToolButton {
            id: formatHorizontalAlignCenter
            checked: true
            highlighted: checked
            enabled: !fullscreen.checked && !formatHorizontalAlignLeft.checked && !formatHorizontalAlignRight.checked
            icon.source: MaterialIcons.formatHorizontalAlignCenter

            BasicToolTip {
                text: "Align Horizontal Center"
                visible: formatHorizontalAlignCenter.hovered
                ExtraPosition.position: ItemPositions.Bottom
            }

            onClicked: loader.reload()
        }

        BasicToolButton {
            id: formatVerticalAlignCenter
            checked: true
            highlighted: checked
            enabled: !fullscreen.checked && !formatVerticalAlignBottom.checked && !formatVerticalAlignTop.checked
            icon.source: MaterialIcons.formatVerticalAlignCenter

            BasicToolTip {
                text: "Align Vertical Center"
                visible: formatVerticalAlignCenter.hovered
                ExtraPosition.position: ItemPositions.Bottom
            }

            onClicked: loader.reload()
        }

        BasicSeparator { color: Style.colorPrimaryLight }

        BasicToolButton {
            id: formatHorizontalAlignLeft
            highlighted: checked
            enabled: !fullscreen.checked && !formatHorizontalAlignCenter.checked
            icon.source: MaterialIcons.formatHorizontalAlignLeft

            BasicToolTip {
                text: "Align Left"
                visible: formatHorizontalAlignLeft.hovered
                ExtraPosition.position: ItemPositions.Bottom
            }

            onClicked: loader.reload()
        }

        BasicToolButton {
            id: formatHorizontalAlignRight
            highlighted: checked
            enabled: !fullscreen.checked && !formatHorizontalAlignCenter.checked
            icon.source: MaterialIcons.formatHorizontalAlignRight

            BasicToolTip {
                text: "Align Right"
                visible: formatHorizontalAlignRight.hovered
                ExtraPosition.position: ItemPositions.Bottom
            }

            onClicked: loader.reload()
        }

        BasicToolButton {
            id: formatVerticalAlignBottom
            highlighted: checked
            enabled: !fullscreen.checked && !formatVerticalAlignCenter.checked
            icon.source: MaterialIcons.formatVerticalAlignBottom

            BasicToolTip {
                text: "Align Bottom"
                visible: formatVerticalAlignBottom.hovered
                ExtraPosition.position: ItemPositions.Bottom
            }

            onClicked: loader.reload()
        }

        BasicToolButton {
            id: formatVerticalAlignTop
            highlighted: checked
            enabled: !fullscreen.checked && !formatVerticalAlignCenter.checked
            icon.source: MaterialIcons.formatVerticalAlignTop

            BasicToolTip {
                text: "Align Top"
                visible: formatVerticalAlignTop.hovered
                ExtraPosition.position: ItemPositions.Bottom
            }

            onClicked: loader.reload()
        }

        BasicSeparator { color: Style.colorPrimaryLight }

        BasicToolButton {
            id: fullscreen
            highlighted: checked
            enabled: !formatHorizontalAlignCenter.checked && !formatVerticalAlignCenter.checked && !formatHorizontalAlignLeft.checked && !formatHorizontalAlignRight.checked
                     && !formatVerticalAlignBottom.checked && !formatVerticalAlignTop.checked
            icon.source: checked ? MaterialIcons.fullscreen : MaterialIcons.fullscreenExit

            BasicToolTip {
                text: fullscreen.checked ? "Fullscreen" : "Fullscreen Exit"
                visible: fullscreen.hovered
                ExtraPosition.position: ItemPositions.Bottom
            }

            onClicked: loader.reload()
        }
    }

    contentItem: BasicSplitView {
        BasicTreeView {
            SplitView.fillHeight: true
            SplitView.preferredWidth: root.width/4
            SplitView.minimumWidth: root.width/4
            SplitView.maximumWidth: root.width/2

            topMargin: 5
            bottomMargin: 5

            model: FolderTreeModel {
                delayed: true
                nameFilters: ["*.qml"]
                path: ":/qt/qml/L01_Gallery/qml/components"
            }

            delegate: FolderTreeDelegate {
                required property TreeView treeView
                required property int index
                required property int depth
                required property string fileBaseName
                required property bool isFile
                required hasChildren
                required expanded
                required info

                leftPadding: 5 + depth * 20
                rightPadding: 5

                icon.color: color
                icon.source: isFile ? MaterialIcons.fileOutline :
                             expanded ? MaterialIcons.folderOpen : MaterialIcons.folder
                highlighted: loader.file===fileBaseName
                text: fileBaseName

                onClicked: {
                    if(isFile)
                        loader.load(fileBaseName)
                    if(expanded)
                        treeView.collapseRecursively(index)
                    else
                        treeView.expand(index)
                }
            }
        }

        Item {
            id: loader
            SplitView.fillHeight: true
            SplitView.fillWidth: true
            clip: true

            property bool isLoading: false
            property string file
            readonly property string loadedFile: file || "Placeholder"
            property Item userItem

            DebugRectangle {
                anchors.fill: loader.userItem
            }

            onLoadedFileChanged: doLoad(loadedFile)

            function doLoad(file: string) {
                if (userItem)
                    userItem.destroy()
                var component = Qt.createComponent("L01_Gallery", file, Component.PreferSynchronous, loader);
                if(component.status!==Component.Ready) {
                    console.error("Failed to load:", file)
                    console.warn(component.errorString())
                    return;
                }
                isLoading=true
                var incubator = component.incubateObject(loader, {
                    "visible": true,
                    "anchors.fill": fullscreen.checked ? loader : undefined,
                    "anchors.horizontalCenter": formatHorizontalAlignCenter.checked ? loader.horizontalCenter : undefined,
                    "anchors.verticalCenter": formatVerticalAlignCenter.checked ? loader.verticalCenter : undefined,
                    "anchors.left": formatHorizontalAlignLeft.checked ? loader.left : undefined,
                    "anchors.right": formatHorizontalAlignRight.checked ? loader.right : undefined,
                    "anchors.bottom": formatVerticalAlignBottom.checked ? loader.bottom : undefined,
                    "anchors.top": formatVerticalAlignTop.checked ? loader.top : undefined,
                }, asynchronousLoad.checked ? Qt.Asynchronous : Qt.Synchronous);
                if (incubator.status !== Component.Ready) {
                    incubator.onStatusChanged = function(status) {
                        if (status === Component.Ready) {
                            userItem = incubator.object as Item
                            isLoading=false
                        }
                    };
                } else {
                    userItem = incubator.object as Item
                    isLoading=false
                }
            }

            function reload() {
                doLoad(loadedFile)
            }

            function load(file: string) {
                loader.file = file
            }

            function clear() {
                loader.file = ""
            }

            BasicBusyIndicator {
                anchors.centerIn: parent
                visible: loader.isLoading
            }
        }
    }
}
