import QtQuick
import Eco.Tier1.Models
import Eco.Tier2.Unit
import Eco.Tier3.Axion
import L01_Gallery

Item {
    id: root

    implicitWidth: Math.min(layout.width, parent.width)
    implicitHeight: Math.min(layout.height, parent.height)

    GridContainer {
        id: layout
        columns: 3

        RawButton {onClicked: basicDialog1.open();    text: ("Dialog basic");        color: Style.blueGrey;    icon.source: MaterialIcons.import_;       Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showAwait();       text: ("Dialog await");        color: Style.oxfordBlue;  icon.source: MaterialIcons.export_;       Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showWarning();     text: ("Dialog warning");      color: Style.blueGrey;    icon.source: MaterialIcons.import_;       Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showCancel();      text: ("Dialog cancel");       color: Style.brown;       icon.source: MaterialIcons.cancel;        Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showQuit();        text: ("Dialog quit");         color: Style.orange;      icon.source: MaterialIcons.pageFirst;     Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showValidate();    text: ("Dialog validate");     color: Style.oxfordBlue;  icon.source: MaterialIcons.check;         Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: qrcodePopup.open();     text: ("Dialog qrCode");       color: Style.yellow;      icon.source: MaterialIcons.qrcode;        Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: busyPopup.open();       text: ("Dialog busy");         color: Style.lime;        icon.source: MaterialIcons.sync;          Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: busyCancelPopup.open(); text: ("Dialog cancel busy");  color: Style.lime;        icon.source: MaterialIcons.sync;          Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showForm();        text: ("Dialog formulaire");   color: Style.teal;        icon.source: MaterialIcons.formTextbox;   Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showInput();       text: ("Dialog input");        color: Style.cyan;        icon.source: MaterialIcons.formTextbox;   Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showLogout();      text: ("Dialog logout");       color: Style.lightBlue;   icon.source: MaterialIcons.logout;        Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showDate();        text: ("Dialog calendar");     color: Style.blue;        icon.source: MaterialIcons.calendar;      Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showTime();        text: ("Dialog time");         color: Style.indigo;      icon.source: MaterialIcons.clock;         Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showFileTree();    text: ("Dialog file tree");    color: Style.deepPurple;  icon.source: MaterialIcons.folder;        Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: dialogForm.open();      text: ("Form no manager");     color: Style.pink;        icon.source: MaterialIcons.formTextbox;   Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: dialogInput.open();     text: ("Input no manager");    color: Style.cyan;        icon.source: MaterialIcons.formTextbox;   Layout.fillWidth: true; Layout.fillHeight: true }
        RawButton {onClicked: root.showAlarm();       text: ("Dialog alarm");        color: Style.purple;      icon.source: MaterialIcons.alarm;         Layout.fillWidth: true; Layout.fillHeight: true }
    }

    function showForm() {
        var component = Qt.createComponent("L01_Gallery", "Form_DemoModel");
        var model = component.createObject(root);

        var formValues = {
            "textField": "",
            "textArea": "",
            "textFieldHex": 0x7FFF,
            "ipv4Address": "192.168.0.100",
            "socketPort": 3306,
            "textFieldDate": new Date(),
            "textFieldTime": new Date(),
            "textFieldPath": "",
            "textFieldUnitValue": 15,
            "textFieldUnitValue": UnitTypes.Torque_mNm,
            "password": "Secret1234.",
            "switchVal": false,
            "dropDownList": 0
        }

        DialogManager.showForm({
            formModel: model,
            formValues: formValues,
            onClosed: function() {
                model.destroy();
            },
            onFormValidated: function(formValues) {
                console.log(formValues.textField)
                console.log(formValues.textArea)
                console.log(formValues.ipv4Address)
            }
        });
    }

    function showInput() {
        DialogManager.showInput({
            label: qsTr("Entrée"),
            placeholder: qsTr("Placeholder"),
            infos: qsTr("Entrer un text dans le champs ci-dessus"),
            onInputAccepted: function(value) {
                console.log(value)
            }
        });
    }

    function showLogout() {
        DialogManager.showWarning({
            message: qsTr("Êtes-vous sûr de vouloir vous déconnecter ?"),
            buttonAccept: qsTr("Déconnexion"),
            buttonReject: qsTr("Annuler"),
            onAccepted: function() {
                console.log("Accept loggout")
            },
            onRejected: function() {
                console.log("Reject loggout")
            }
        });
    }

    function showDate() {
        DialogManager.showDate({
            selectedDate: new Date(),
            onDateSelected: function(date) {
                console.log(date)
            }
        });
    }

    function showTime() {
        DialogManager.showTime({
            selectedTime: new Date(),
            onTimeSelected: function(time) {
                console.log(time)
            }
        });
    }

    function showFileTree() {
        DialogManager.showFileTree({
            showRootDrives: true,
            showSnapPackageDrives: false,
            showUnmountedAutofsDrives: false,
            showTmpfsDrives: false,
            showOverlayDrives: false,
            showBootDrives: false,
            showConfigDrives: false,
            showReadOnlyDrives: false,
            showStandardPaths: true,
            showQrcDrives: true,
            showApplicationDirPath: true,
            onPathSelected: function(path) {
                console.log(path)
            }
        });
    }

    function showQuit() {
        DialogManager.showQuit();
    }

    function showAwait() {
        DialogManager.showWarning({
            buttonAccept: qsTr("Await"),
            buttonReject: qsTr("Annuler"),
            onAccepted: function() {
                Utils.await(2000)
            }
        });
    }

    function showWarning() {
        DialogManager.showWarning();
    }

    function showValidate() {
        DialogManager.showValidate({
            infos: qsTr("Vous pouvez créer une nouvelle version du programme ou écraser la version actuelle"),
            buttonAccept: qsTr("Ecraser"),
            buttonApply: qsTr("Nouvelle version"),
            onAccepted: function () {
                console.log("Ecraser")
            },
            onApplied: function () {
                console.log("Nouvelle version")
            }
        });
    }

    function showCancel() {
        DialogManager.showCancel();
    }

    function showAlarm() {
        DialogManager.showAlarm({
            showRepeat: true,
            onAlarmValidated: function(alarmObject) {
                console.log(alarmObject)
            }
        });
    }

    BasicDialog {
        id: basicDialog1
        title: "Basic"
        message: "comment"
        standardButtons: Dialog.Cancel | Dialog.Yes | Dialog.Help
        onHelpRequested: SnackbarManager.show("HAHAHA")
    }

    DialogQrCode {
        id: qrcodePopup
        dataToEncode: "www.voh.ch"
    }

    DialogBusy {
        id: busyPopup
        closePolicy: Popup.CloseOnPressOutside

        SequentialAnimation on progress {
            running: busyPopup.visible
            loops: NumberAnimation.Infinite
            NumberAnimation { from: 0; to: 10000; duration: 5000 }
        }
    }

    DialogBusy {
        id: busyCancelPopup
        closePolicy: Popup.CloseOnPressOutside
        buttonReject: "Cancel"

        SequentialAnimation on progress {
            running: busyCancelPopup.visible
            loops: NumberAnimation.Infinite
            NumberAnimation { from: 0; to: 10000; duration: 5000 }
        }
    }

    DialogForm {
        id: dialogForm
        formObject: null
        formValues: ({
            "textField": "",
            "textArea": "",
            "textFieldHex": 0x7FFF,
            "ipv4Address": "192.168.0.100",
            "socketPort": 3306,
            "textFieldDate": new Date(),
            "textFieldTime": new Date(),
            "textFieldPath": "",
            "textFieldUnit": {
                "value": 0,
                "type": UnitTypes.Torque_mNm
            },
            "password": "Secret1234.",
            "switchVal": false,
            "dropDownList": 0
        })
        formModel: Form_DemoModel{}
        onFormValidated: (formValues) => {
            console.log(formValues.textField)
            console.log(formValues.textArea)
            console.log(formValues.ipv4Address)
            console.log(formValues.textFieldDate)
            console.log(formValues.textFieldTime)
            console.log(formValues.textFieldPath)
        }
    }

    DialogInput {
        id: dialogInput
        label: qsTr("Entrée")
        placeholder: qsTr("Placeholder")
        infos: qsTr("Entrer un text dans le champs ci-dessus")
        onInputAccepted: (value) => {
            console.log(value)
        }
    }
}
