import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion

BasicDialog {
    id: root

    signal formValidated(var formValues)

    title: qsTr("Formulaire")
    message: qsTr("Remplisser le formulaire")
    property string buttonReject: qsTr("Annuler")
    property string buttonAccept: qsTr("Valider")
    property string buttonHelp: ""

    property bool helpVisible: buttonHelp!==""

    required property FormObjectModel formModel
    required property QtObject formObject
    property var formValues: null

    property int selectPolicy: FormEditPolicies.Manual
    property int submitPolicy: FormEditPolicies.Manual

    readonly property bool warning: root.formModel.warning

    onAccepted: {
        root.formModel.submit()
        root.formValidated(root.formModel.currentValues())
    }

    onRejected: {
        root.formModel.revert()
    }

    onAboutToShow: {
        root.formModel.select()
        if(root.formValues)
            root.formModel.fillValues(root.formValues)
    }

    buttonsContainer: [
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole; text: root.buttonAccept; highlighted: true; enabled: !root.warning },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.RejectRole; text: root.buttonReject },
        ButtonDialog { DialogButtonBox.buttonRole: DialogButtonBox.HelpRole; text: root.buttonHelp; visible: root.helpVisible}
    ]

    Binding {
        when: root.formObject
        target: root.formModel
        property: "target"
        value: root.formObject
    }

    Binding {
        target: root.formModel
        property: "selectPolicy"
        value: root.selectPolicy
    }

    Binding {
        target: root.formModel
        property: "submitPolicy"
        value: root.submitPolicy
    }

    padding: Style.contentRectangleBorderWidth
    contentColor: Style.colorPrimaryDark

    contentItem: PaneTreeView {
        implicitHeight: 400
        implicitWidth: 800

        drawFrame: root.formModel.warning
        borderWidth: Style.contentRectangleBorderWidth
        borderColor: root.formModel.warning ? Style.colorWarning : Style.colorPrimary

        model: ProxyModel {
            delayed: true
            sourceModel: root.formModel
            filterRoleName: "visible"
            filterValue: true
        }
    }
}
