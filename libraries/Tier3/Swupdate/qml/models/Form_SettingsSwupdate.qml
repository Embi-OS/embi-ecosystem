import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import Eco.Tier3.Swupdate

FormObjectModel {
    id: root

    FormSwitchDelegate {
        label: "auto_reboot"
        targetProperty: "auto_reboot"
    }
    FormTextFieldDelegate {
        label: "device_id"
        targetProperty: "device_id"
    }
    FormComboBoxDelegate {
        label: "software_branch"
        options: StandardObjectModel {
            StandardObject {text: "Dev"; value: "dev"}
            StandardObject {text: "Release"; value: "release"}
        }
        textRole: "text"
        valueRole: "value"
        targetProperty: "software_branch"
    }
    FormComboBoxDelegate {
        id: suricattaService
        label: "suricatta_service"
        options: StandardObjectModel {
            StandardObject {text: "None"; value: ""}
            StandardObject {text: "General"; value: "general"}
            StandardObject {text: "Hawkbit"; value: "hawkbit"}
        }
        textRole: "text"
        valueRole: "value"
        targetProperty: "suricatta_service"
    }

    FormSwitchDelegate {
        id: mongooseEnabled
        label: "mongoose_enabled"
        targetProperty: "mongoose_enabled"
    }
    FormTextFieldDelegate {
        visible: mongooseEnabled.checked
        label: "mongoose_port"
        validator: SocketPortValidator{}
        valueType: FormValueTypes.Integer
        targetProperty: "mongoose_port"
    }

    FormTextFieldDelegate {
        visible: suricattaService.currentValue==="general"
        label: "gservice_url"
        validator: UrlValidator{}
        targetProperty: "gservice_url"
    }
    FormTextFieldDelegate {
        visible: suricattaService.currentValue==="general"
        label: "gservice_polldelay"
        valueType: FormValueTypes.Integer
        targetProperty: "gservice_polldelay"
    }

    FormTextFieldDelegate {
        visible: suricattaService.currentValue==="hawkbit"
        label: "hawkbit_url"
        validator: UrlValidator{}
        targetProperty: "hawkbit_url"
    }
    FormTextFieldDelegate {
        visible: suricattaService.currentValue==="hawkbit"
        label: "hawkbit_polldelay"
        valueType: FormValueTypes.Integer
        targetProperty: "hawkbit_polldelay"
    }
    FormTextFieldDelegate {
        visible: suricattaService.currentValue==="hawkbit"
        label: "hawkbit_targettoken"
        targetProperty: "hawkbit_targettoken"
    }
    FormTextFieldDelegate {
        visible: suricattaService.currentValue==="hawkbit"
        label: "hawkbit_gatewaytoken"
        targetProperty: "hawkbit_gatewaytoken"
    }
}
