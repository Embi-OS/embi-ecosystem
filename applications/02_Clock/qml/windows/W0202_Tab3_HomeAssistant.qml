import QtQuick
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier2.Solid
import Eco.Tier3.Axion
import Eco.Tier2.Rest
import L02_Clock

BasicPane {
    id: root

    property string ip_address: "192.168.1.22"
    property string token: "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI4ODM0YmZkMzRmMGI0ZTA5YmUxMmE2NzkyNTNjMmVlZiIsImlhdCI6MTc2MjYyMDAyMiwiZXhwIjoyMDc3OTgwMDIyfQ.epKbncSfea5XXLPAosRkvYphK17KBsViD5A9MrXlR78"

    Component.onCompleted: Qt.callLater(root.authenticate)

    SettingsMapper {
        id: homeAssistantSettings
        settingsCategory: "homeAssistant"
        property alias ip_address: root.ip_address
        property alias token: root.token
    }

    property Component formModel: FormObjectModel {
        FormTextFieldDelegate {
            label: qsTr("Adresse IPv4")
            validator: Ipv4Validator {}
            targetProperty: "ip_address"
        }
        FormInfoDelegate {
            label: qsTr("Token")
            targetProperty: "token"
        }
    }

    RestClient {
        id: homeAssistantClient
        name: "homeAssistant"
        baseUrl: ("http://%1").arg(root.ip_address)
        port: 8123
        trailingSlash: false
        globalHeaders: ({"Authorization": "Bearer %1".arg(root.token)})
    }

    function authenticate() {
        console.log(1)
        // var reply = homeAssistantClient.rootClass.post("api/services/scene/turn_on", ({"entity_id": "scene.cheminee"}))
        // var reply = homeAssistantClient.rootClass.get("api/services")
        // var reply = homeAssistantClient.rootClass.get("api/config")
        var reply = homeAssistantClient.rootClass.get("api")
        reply.succeeded.connect(root.onReplySucceeded)
        reply.failed.connect(root.onReplyFailed)
    }

    function onReplySucceeded(httpStatus: int, reply: var) {
        console.log(1)
        var settings = {
            "caption": ("Http status: %1").arg(httpStatus),
            "details": RestHelper.parseBody(reply, false),
            "diagnose": false
        }
        SnackbarManager.showSuccess(settings)
    }
    function onReplyFailed(httpStatus: int, reply: var) {
        console.log(1)
        var settings = {
            "caption": ("Http status: %1").arg(httpStatus),
            "details": RestHelper.parseBody(reply, false),
            "diagnose": false
        }
        SnackbarManager.showError(settings)
    }
}
