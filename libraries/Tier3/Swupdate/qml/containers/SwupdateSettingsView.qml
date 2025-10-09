import QtQuick
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier3.Axion
import Eco.Tier3.Swupdate

PaneTreeView {
    id: root

    signal editButtonClicked()
    signal restartButtonClicked()

    title: qsTr("Configuration")

    model: proxyModel

    onEditButtonClicked: {
        var component = Qt.createComponent("Eco.Tier3.Swupdate", "Form_SettingsSwupdate");
        var model = component.createObject(root) as FormObjectModel
        var settings = {
            "title": qsTr("Paramètres SWUpdate"),
            "formModel": model,
            "formObject": swupdateSettings,
            "onClosed": function () {
                model.destroy()
            },
            "onFormValidated": function(formValues) {
                Swupdate.restart()
            }
        }

        DialogManager.showForm(settings);
    }

    onRestartButtonClicked: {
        Swupdate.restart();
    }

    SettingsMapper {
        id: swupdateSettings
        selectPolicy: QVariantMapperPolicies.Delayed
        submitPolicy: QVariantMapperPolicies.Delayed
        settingsPath: "/etc/swupdate"
        baseName: "swupdate.ini"

        property bool auto_reboot
        property string device_id
        property string software_branch
        property string suricatta_service

        property bool mongoose_enabled
        property int mongoose_port

        property string gservice_url
        property int gservice_polldelay

        property string hawkbit_url
        property int hawkbit_polldelay
        property string hawkbit_targettoken
        property string hawkbit_gatewaytoken
    }

    ProxyModel {
        id: proxyModel
        delayed: true
        sourceModel: treeModel
        filterRoleName: "visible"
        filterValue: true
    }

    StandardObjectModel {
        id: treeModel
        FormButtonDelegate {
            enabled: root.editable && Swupdate.isReady
            label: qsTr("Relancer SWUpdate")
            icon: MaterialIcons.restart
            onClicked: root.restartButtonClicked()
        }

        SeparatorTreeDelegate {}
        InfoTreeDelegate {text: "auto_reboot";info: swupdateSettings.auto_reboot}
        InfoTreeDelegate {text: "device_id";info: swupdateSettings.device_id}
        InfoTreeDelegate {text: "software_branch";info: swupdateSettings.software_branch}
        InfoTreeDelegate {text: "suricatta_service";info: swupdateSettings.suricatta_service}
        InfoTreeDelegate {text: "mongoose_enabled";info: swupdateSettings.mongoose_enabled}
        InfoTreeDelegate {text: "mongoose_port";info: swupdateSettings.mongoose_port}
        InfoTreeDelegate {text: "gservice_url";info: swupdateSettings.gservice_url}
        InfoTreeDelegate {text: "gservice_polldelay";info: swupdateSettings.gservice_polldelay}
        InfoTreeDelegate {text: "hawkbit_url";info: swupdateSettings.hawkbit_url}
        InfoTreeDelegate {text: "hawkbit_polldelay";info: swupdateSettings.hawkbit_polldelay}
        InfoTreeDelegate {text: "hawkbit_targettoken";info: swupdateSettings.hawkbit_targettoken}
        InfoTreeDelegate {text: "hawkbit_gatewaytoken";info: swupdateSettings.hawkbit_gatewaytoken}

        FormButtonDelegate {
            visible: root.editable && Swupdate.isReady
            label: qsTr("Editer")
            icon: MaterialIcons.pen
            onClicked: root.editButtonClicked()
        }
    }
}
