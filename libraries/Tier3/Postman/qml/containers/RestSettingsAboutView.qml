import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Postman

PaneTreeView {
    id: root

    signal editButtonClicked()
    signal authButtonClicked()

    title: qsTr("Réglages de la connexion à l'API globale")

    model: proxyModel

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    onEditButtonClicked: {
        var component = Qt.createComponent("Eco.Tier3.Postman", "Form_SettingsRest");
        var model = component.createObject(root) as FormObjectModel;
        var settings = {
            "title": qsTr("Paramètres Rest"),
            "formModel": model,
            "formObject": RestManager,
            "onClosed": function() {
                model.destroy();
                component.destroy();
            },
            "onFormValidated": function(formValues) {
                AxionHelper.warningRestart()
            }
        }

        DialogManager.showForm(settings);
    }

    onAuthButtonClicked: {
        var component = Qt.createComponent("Eco.Tier3.Postman", "Form_SettingsRestAuthentication");
        var model = component.createObject(root) as FormObjectModel;
        var settings = {
            "title": qsTr("Paramètres d'authentification Rest"),
            "formModel": model,
            "onClosed": function() {
                model.destroy();
                component.destroy();
            },
            "onFormValidated": function(formValues) {
                const authIdentifier = formValues.identifier
                const authPassword = formValues.password
                RestManager.authenticate(authIdentifier, authPassword)
            }
        }

        DialogManager.showForm(settings);
    }

    StandardObjectModel {
        id: treeModel
        InfoTreeDelegate {text: "API interne"; info: RestManager.localApiEnabled }
        InfoTreeDelegate {text: "API effective url"; info: RestManager.apiEffectiveUrl }
        FormLabelDelegate {label: "API global headers"; value: RestManager.client.globalHeaders; valueType: FormValueTypes.Map; visible: root.editable }
        FormLabelDelegate {label: "API global parameters"; value: RestManager.client.globalParameters; valueType: FormValueTypes.Map; visible: root.editable }
        FormButtonDelegate {
            visible: root.editable
            label: qsTr("Editer")
            icon: MaterialIcons.databaseEdit
            onClicked: root.editButtonClicked()
        }
        SeparatorTreeDelegate {}
        FormButtonDelegate {
            visible: root.editable
            label: qsTr("Authentifier")
            icon: MaterialIcons.lock
            onClicked: root.authButtonClicked()
        }
    }
}
