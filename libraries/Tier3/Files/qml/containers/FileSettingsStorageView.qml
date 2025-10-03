pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Files

PaneListView {
    id: root

    property alias showRootDrives: driveModel.showRootDrives
    property alias showSnapPackageDrives: driveModel.showSnapPackageDrives
    property alias showUnmountedAutofsDrives: driveModel.showUnmountedAutofsDrives
    property alias showTmpfsDrives: driveModel.showTmpfsDrives
    property alias showOverlayDrives: driveModel.showOverlayDrives
    property alias showBootDrives: driveModel.showBootDrives
    property alias showConfigDrives: driveModel.showConfigDrives
    property alias showReadOnlyDrives: driveModel.showReadOnlyDrives

    signal mountButtonClicked()
    signal editButtonClicked(FilesystemDrive drive)
    signal ejectButtonClicked(string mountPoint)
    signal plusButtonClicked()
    signal deleteButtonClicked()
    signal cancelButtonClicked()

    model: proxyModel

    onMountButtonClicked: {
        fstabModel.mount();
    }

    onEditButtonClicked: (drive) => {
        var mountPath = drive.filePath
        var mountParams = fstabModel.smbParams(mountPath)
        root.showForm(mountParams, function(formValues) {
            fstabModel.updateSmbShare(mountPath, formValues)
        });
    }

    onPlusButtonClicked: {
        var mountParams = {
            hostName: "",
            port: 445,
            mountPath: "",
            name: "",
            nofail: true,
            smbVersion: SMBVersions.Vers_3_0,
            smbIdentification: SMBIdentifications.Password,
            userName: "",
            password: "",
            domain: "",
            advancedOptions: "file_mode=0777,dir_mode=0777"
        }
        root.showForm(mountParams, function(formValues) {
            fstabModel.addSmbShare(formValues)
        });
    }

    onEjectButtonClicked: (mountPoint) => {
        fstabModel.umount(mountPoint)
    }

    onCancelButtonClicked: {
        checkableModel.clear();
        root.state = "";
    }

    onDeleteButtonClicked: {
        var rowsSelected=checkableModel.selectedRows;
        var mountPointSelected=checkableModel.ModelHelper.getProperties(rowsSelected,"filePath")

        for(const mountPoint of mountPointSelected) {
            fstabModel.removeSmbShare(mountPoint)
        }

        checkableModel.clear();
        root.state = "";
    }

    function showForm (params: var, onFormValidated: var) {
        var component = Qt.createComponent("Eco.Tier3.Files", "Form_SettingsSambaShare");
        var model = component.createObject(root) as FormObjectModel;
        var settings = {
            "title": qsTr("Connexion à un partage SMB"),
            "formModel": model,
            "buttonHelp": qsTr("Tester"),
            "formValues": params,
            "onHelpRequested": function() {
                var tempParams = model.currentValues();
                fstabModel.testSmbShare(tempParams)
            },
            "onClosed": function() {
                model.destroy();
            },
            "onFormValidated": function(formValues) {
                onFormValidated(formValues)
            }
        }

        DialogManager.showForm(settings);
    }

    function showFileTree (url: string) {
        var settings = {
            "selectedPath": url
        }

        DialogManager.showFileTree(settings);
    }

    FstabModel {
        id: fstabModel
        onUpdated: driveModel.markDirty()
        onMounted: (hostName, result) => {
            driveModel.markDirty()
            if(result)
                SnackbarManager.showSuccess(qsTr("Successfully mounted: %1").arg(hostName))
        }
        onUmounted: (hostName, result) => {
            driveModel.markDirty()
            if(result)
                SnackbarManager.showSuccess(qsTr("Successfully unmounted: %1").arg(hostName))
        }
        onSmbShareTested: (hostName, result) => {
            driveModel.markDirty()
            if(result)
                SnackbarManager.showSuccess(qsTr("Successfully mounted: %1").arg(hostName))
        }
        onError: (error) => SnackbarManager.showError(error)

        property DialogObject processDialog: null
        onProcessRunningChanged: (processRunning) => {
            if(processRunning) {
                processDialog = DialogManager.showBusy({infos: currentProcess});
                processDialog.infos = Qt.binding(() => fstabModel.currentProcess)
            }
            else
                processDialog.hide()
        }
    }

    CheckableProxyModel {
        id: checkableModel
        sourceModel: driveModel
    }

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: checkableModel
        sortRoleName: "driveIsMounted"
        sortOrder: Qt.DescendingOrder
    }

    FilesystemDriveModel {
        id: driveModel
        showRootDrives: showRootDrives.checked
        showSnapPackageDrives: showSnapPackageDrives.checked
        showUnmountedAutofsDrives: showUnmountedAutofsDrives.checked
        showTmpfsDrives: showTmpfsDrives.checked
        showOverlayDrives: showOverlayDrives.checked
        showBootDrives: showBootDrives.checked
        showConfigDrives: showConfigDrives.checked
        showReadOnlyDrives: showReadOnlyDrives.checked
        showFsTabDrives: showFsTabDrives.checked
    }

    header: RowContainer {
        drawFrame: true
        BasicLabel {
            Layout.fillWidth: true
            text: ("%1 élément(s)").arg(driveModel.count)
        }
        BasicBusyIndicator {
            visible: driveModel.processing
            size: 24
            lineWidth: 0
        }
        ClickableIcon {
            icon: MaterialIcons.refresh
            onClicked: {
                fstabModel.refresh();
                driveModel.refresh();
            }
        }
        ClickableIcon {
            visible: root.state===""
            icon: MaterialIcons.dotsVertical
            onClicked: settingsMenu.open()

            BasicMenu {
                id: settingsMenu
                width: 400
                ExtraPosition.position: ItemPositions.BottomEnd
                BasicMenuItem {text: qsTr("Monter les éléments de stockage"); onClicked: root.mountButtonClicked()}
                BasicMenuItem {text: qsTr("Recharger les éléments de stockage"); onClicked: driveModel.markDirty()}
                BasicMenuSeparator {}
                BasicCheckDelegate {id: showRootDrives; text: qsTr("Voir les disques root"); checked: true}
                BasicCheckDelegate {id: showSnapPackageDrives; text: qsTr("Voir les disques snap"); checked: false}
                BasicCheckDelegate {id: showUnmountedAutofsDrives; text: qsTr("Voir les disques non montés"); checked: false}
                BasicCheckDelegate {id: showTmpfsDrives; text: qsTr("Voir les disques tmpfs"); checked: false}
                BasicCheckDelegate {id: showOverlayDrives; text: qsTr("Voir les disques overlay"); checked: false}
                BasicCheckDelegate {id: showBootDrives; text: qsTr("Voir les disques boot"); checked: true}
                BasicCheckDelegate {id: showConfigDrives; text: qsTr("Voir les disques config"); checked: true}
                BasicCheckDelegate {id: showReadOnlyDrives; text: qsTr("Voir les disques en lectures seules"); checked: false}
                BasicCheckDelegate {id: showFsTabDrives; text: qsTr("Voir les disques fstab"); checked: true}
            }
        }
    }

    viewSection.property: "driveIsMounted"
    viewSection.delegate: BasicLabel {
        width: (ListView.view as BasicListView).viewWidth
        height: contentHeight + 2*Style.contentRectangleBorderWidth
        verticalAlignment: Text.AlignVCenter
        required property bool section
        text: section ? qsTr("Disque(s) monté(s):") : qsTr("Disque(s) non monté(s):")
        font: Style.textTheme.title2
    }

    delegate: StorageDelegate {
        width: (ListView.view as BasicListView).viewWidth
        required property FilesystemDrive qtObject
        required property int index
        required property string fileUrl
        required property string filePath
        required property string driveFileSystemType
        required property bool driveIsRoot
        required property bool driveIsBoot
        required property bool driveIsConfig
        required property bool driveIsOverlay
        required text
        required info
        icon.source: MimeIconHelper.getSvgIconPathForUrl(fileUrl, false)

        enabled: !(driveIsRoot || driveIsBoot || driveIsConfig || driveIsOverlay)
        canEdit: fstabModel.ModelHelper.contains("mountPoint", filePath) && root.editable
        canEject: enabled && !canEdit && root.editable

        onFolderClicked: root.showFileTree(filePath)
        onEjectClicked: root.ejectButtonClicked(filePath)

        required checkState
        checkable: root.state==="selection" && canEdit
        onClicked: {
            if(!canEdit)
                return
            if(root.state==="selection")
                checkableModel.toggleRow(proxyModel.mapToSource(index));
            else
                root.editButtonClicked(qtObject)
        }
        onPressAndHold: {
            if(!canEdit)
                return
            if(root.state!=="selection")
                checkableModel.clear();
            root.state="selection"
            checkableModel.toggleRow(proxyModel.mapToSource(index));
        }
    }

    viewFooter: RowLayout {
        z: 2
        width: ListView.view.width
        spacing: 10
        visible: root.editable

        LayoutSpring {}

        FabButton {
            visible: root.state===""
            highlighted: true
            hint: qsTr("Ajouter un support de stockage")
            icon.source: MaterialIcons.plus
            onClicked: root.plusButtonClicked()
        }
        FabButton {
            visible: root.state==="selection" && !checkableModel.isEmpty
            highlighted: true
            text: qsTr("Supprimer")
            hint: qsTr("Supprimer les éléments sélectionnés")
            icon.source: MaterialIcons.delete_
            onClicked: root.deleteButtonClicked()
        }
        FabButton {
            visible: root.state==="selection"
            text: qsTr("Quitter")
            hint: qsTr("Quitter le mode sélection")
            icon.source: MaterialIcons.cancel
            onClicked: root.cancelButtonClicked()
        }

        LayoutSpring {}
    }
}
