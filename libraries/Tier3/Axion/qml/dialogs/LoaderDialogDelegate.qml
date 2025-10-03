pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

Item {
    id: root

    property alias asynchronous: loader.asynchronous

    required property DialogObject dialogObject
    property bool isVisible: false
    property bool fullscreen: false
    property var settings: null
    property int type: DialogTypes.None
    property int severity: DialogSeverities.None
    property string title: ""
    property string message: ""
    property string infos: ""
    property string traces: ""
    property int progress: 0

    property string buttonApply: ""
    property string buttonReject: ""
    property string buttonAccept: ""
    property string buttonDiscard: ""
    property string buttonHelp: ""

    property bool animation: false

    readonly property BasicDialog dialog: loader.item as BasicDialog

    Loader {
        id: loader
        active: root.isVisible
        onStatusChanged: {
            if (loader.status===Loader.Ready) {
                root.tryOpen()
            }
        }
        sourceComponent: root.type===DialogTypes.Action ? dialogAction :
                         root.type===DialogTypes.Text ? dialogText :
                         root.type===DialogTypes.Tree ? dialogTree :
                         root.type===DialogTypes.Busy ? dialogBusy :
                         root.type===DialogTypes.Date ? dialogDate :
                         root.type===DialogTypes.Time ? dialogTime :
                         root.type===DialogTypes.FileTree ? dialogFileTree :
                         root.type===DialogTypes.Input ? dialogInput :
                         root.type===DialogTypes.Form ? dialogForm :
                         root.type===DialogTypes.Alarm ? dialogAlarm : undefined
    }

    onIsVisibleChanged: tryOpen()

    function tryOpen() {
        //loader.active = true
        if (loader.status!==Loader.Ready)
            return;

        if(isVisible)
            root.dialog.open()
        else
            root.dialog.close();
    }

    function onAboutToHide() {
        root.dialogObject.aboutToHide();
        root.settings?.onAboutToHide();
    }
    function onAboutToShow() {
        root.dialogObject.aboutToShow();
        root.settings?.onAboutToShow();
    }
    function onClosed() {
        root.dialogObject.closed();
        root.settings?.onClosed();
    }
    function onOpened() {
        root.dialogObject.opened();
        root.settings?.onOpened();
    }

    function onAccepted() {
        if(dialogObject.diagnose)
            DialogManager.diagnose(root.severity, "Accepted")
        root.dialogObject.accepted();
        root.settings?.onAccepted();
    }
    function onApplied() {
        if(dialogObject.diagnose)
            DialogManager.diagnose(root.severity, "Applied")
        root.dialogObject.applied();
        root.settings?.onApplied();
    }
    function onDiscarded() {
        if(dialogObject.diagnose)
            DialogManager.diagnose(root.severity, "Discarded")
        root.dialogObject.discarded();
        root.settings?.onDiscarded();
    }
    function onHelpRequested() {
        if(dialogObject.diagnose)
            DialogManager.diagnose(root.severity, "HelpRequested")
        root.dialogObject.helpRequested();
        root.settings?.onHelpRequested();
    }
    function onRejected() {
        if(dialogObject.diagnose)
            DialogManager.diagnose(root.severity, "Rejected")
        root.dialogObject.rejected();
        root.settings?.onRejected();
    }
    function onReset() {
        if(dialogObject.diagnose)
            DialogManager.diagnose(root.severity, "Reset")
        root.dialogObject.reset();
        root.settings?.onReset();
    }

    Component {
        id: dialogAction
        DialogAction {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message
            infos: root.infos
            traces: root.traces

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept
            buttonApply: root.buttonApply

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAcceptedOverwrite: root.onAccepted()
            onAcceptedVersion: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()
        }
    }

    Component {
        id: dialogText
        DialogText {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message
            infos: root.infos

            buttonAccept: root.buttonAccept

            textFormat: root.settings?.textFormat ?? TextEdit.AutoText

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()
        }
    }

    Component {
        id: dialogTree
        DialogTreeView {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept

            treeModel: root.settings?.treeModel ?? null

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()
        }
    }

    Component {
        id: dialogBusy
        DialogBusy {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message
            infos: root.infos
            traces: root.traces

            buttonReject: root.buttonReject

            indicatorEnabled: root.settings?.indicatorEnabled ?? true
            progressEnabled: root.settings?.progressEnabled ?? root.progress>0
            progress: root.progress

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()
        }
    }

    Component {
        id: dialogDate
        DialogDatePicker {
            fullscreen: root.fullscreen
            animation: root.animation

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept

            selectedDate: root.settings?.selectedDate ?? new Date()
            dayOfWeekVisible: root.settings?.dayOfWeekVisible ?? true
            weekNumberVisible: root.settings?.weekNumberVisible ?? true

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()

            onDateSelected: (date) => {
                if(root.dialogObject.diagnose)
                    DialogManager.diagnose(root.severity, "DateSelected: "+date)
                root.dialogObject.dateSelected(date);
                root.settings?.onDateSelected(date);
            }
        }
    }

    Component {
        id: dialogTime
        DialogTimePicker {
            fullscreen: root.fullscreen
            animation: root.animation

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept

            selectedTime: root.settings?.selectedTime ?? new Date()
            showSeconds: root.settings?.showSeconds ?? false

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()

            onTimeSelected: (time) => {
                if(root.dialogObject.diagnose)
                    DialogManager.diagnose(root.severity, "TimeSelected: "+time)
                root.dialogObject.timeSelected(time);
                root.settings?.onTimeSelected(time);
            }
        }
    }

    Component {
        id: dialogFileTree
        DialogFileTree {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept

            selectionType: root.settings?.selectionType ?? FolderTreeTypes.All
            folderTreeModel.nameFilters: root.settings?.nameFilters ?? undefined
            folderTreeModel.caseSensitive: root.settings?.caseSensitive ?? false
            folderTreeModel.showRootDrives: root.settings?.showRootDrives ?? false
            folderTreeModel.showSnapPackageDrives: root.settings?.showSnapPackageDrives ?? false
            folderTreeModel.showUnmountedAutofsDrives: root.settings?.showUnmountedAutofsDrives ?? false
            folderTreeModel.showTmpfsDrives: root.settings?.showTmpfsDrives ?? false
            folderTreeModel.showOverlayDrives: root.settings?.showOverlayDrives ?? false
            folderTreeModel.showBootDrives: root.settings?.showBootDrives ?? false
            folderTreeModel.showConfigDrives: root.settings?.showConfigDrives ?? false
            folderTreeModel.showReadOnlyDrives: root.settings?.showReadOnlyDrives ?? false
            folderTreeModel.showQrcDrives: root.settings?.showQrcDrives ?? false
            folderTreeModel.showStandardPaths: root.settings?.showStandardPaths ?? true
            folderTreeModel.showApplicationDirPath: root.settings?.showApplicationDirPath ?? true
            folderTreeModel.showDirs: root.settings?.showDirs ?? true
            folderTreeModel.showEmptyDirs: root.settings?.showEmptyDirs ?? true
            folderTreeModel.showFiles: root.settings?.showFiles ?? true
            folderTreeModel.showDirsFirst: root.settings?.showDirsFirst ?? true
            folderTreeModel.showDot: root.settings?.showDot ?? false
            folderTreeModel.showDotDot: root.settings?.showDotDot ?? false
            folderTreeModel.showDotAndDotDot: root.settings?.showDotAndDotDot ?? false
            folderTreeModel.showHidden: root.settings?.showHidden ?? false
            folderTreeModel.showOnlyReadable: root.settings?.showOnlyReadable ?? false
            folderTreeModel.sortCaseSensitive: root.settings?.sortCaseSensitive ?? false
            folderTreeModel.sortReversed: root.settings?.sortReversed ?? false
            folderTreeModel.sortField: root.settings?.sortField ?? FolderTreeModelSortFields.Unsorted

            folderTreeModel.path: root.settings?.selectedPath ?? ""
            folderTreeModel.additionalPaths: root.settings?.additionalPaths ?? []

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()

            onPathSelected: (path) => {
                if(root.dialogObject.diagnose)
                    DialogManager.diagnose(root.severity, "PathSelected: "+path)
                root.dialogObject.pathSelected(path);
                root.settings?.onPathSelected(path);
            }
        }
    }

    Component {
        id: dialogInput
        DialogInput {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept
            buttonApply: root.buttonApply

            label: root.settings?.label ?? ""
            text: root.settings?.text ?? ""
            textType: root.settings?.textType ?? FormTextTypes.String
            validator: root.settings?.validator ?? null
            infos: root.settings?.infos ?? ""
            placeholder: root.settings?.placeholder ?? ""
            warning: root.settings?.warning ?? false

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()

            onInputAccepted: (value) => {
                if(root.dialogObject.diagnose)
                    DialogManager.diagnose(root.severity, "InputAccepted: "+value)
                root.dialogObject.inputAccepted(value);
                root.settings?.onInputAccepted(value);
            }
            onInputApplied: (value) => {
                if(root.dialogObject.diagnose)
                    DialogManager.diagnose(root.severity, "InputApplied: "+value)
                root.dialogObject.inputApplied(value);
                root.settings?.onInputApplied(value);
            }
        }
    }

    Component {
        id: dialogForm
        DialogForm {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept
            buttonHelp: root.buttonHelp

            formModel: root.settings?.formModel ?? null
            formObject: root.settings?.formObject ?? null
            formValues: root.settings?.formValues ?? null
            selectPolicy: root.settings?.selectPolicy ?? FormEditPolicies.Manual
            submitPolicy: root.settings?.submitPolicy ?? FormEditPolicies.Manual

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()

            onFormValidated: (formValues) => {
                if(root.dialogObject.diagnose)
                    DialogManager.diagnose(root.severity, "FormValidated: "+formValues)
                root.dialogObject.formValidated(formValues);
                root.settings?.onFormValidated(formValues);
            }
        }
    }

    Component {
        id: dialogAlarm
        DialogAlarm {
            fullscreen: root.fullscreen
            animation: root.animation

            title: root.title
            message: root.message

            buttonReject: root.buttonReject
            buttonAccept: root.buttonAccept

            hour: root.settings?.hour ?? 0
            minute: root.settings?.minute ?? 0
            date: root.settings?.date ?? new Date()
            name: root.settings?.name ?? ""
            repeat: root.settings?.repeat ?? false
            weekdays: root.settings?.weekdays ?? 0

            showRepeat: root.settings?.showRepeat ?? true
            showName: root.settings?.showName ?? true

            onAboutToHide: root.onAboutToHide()
            onAboutToShow: root.onAboutToShow()
            onClosed: root.onClosed()
            onOpened: root.onOpened()

            onAccepted: root.onAccepted()
            onApplied: root.onApplied()
            onDiscarded: root.onDiscarded()
            onHelpRequested: root.onHelpRequested()
            onRejected: root.onRejected()
            onReset: root.onReset()

            onAlarmValidated: (alarmMap) => {
                if(root.dialogObject.diagnose)
                    DialogManager.diagnose(root.severity, "AlarmValidated: "+alarmMap)
                root.dialogObject.alarmValidated(alarmMap);
                root.settings?.onAlarmValidated(alarmMap);
            }
        }
    }
}
