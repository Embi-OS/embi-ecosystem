import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.Utils
import Eco.Tier2.Rest
import Eco.Tier3.Axion
import L02_Clock
import Eco.Tier3.Hass

BasicPane {
    id: root

    property string ip_address: "192.168.1.22"
    property int port: 8123
    property string token: ""
    readonly property bool loading: homeAssistantClient.rootClass.loading
    readonly property string apiBaseUrl: ("http://%1:%2").arg(ip_address).arg(port)
    readonly property string statesUrl: ("%1/api/states").arg(apiBaseUrl)
    property int lastHttpStatus: 0
    property int entityCount: 0
    property string lastUpdate: ""
    property string lastError: ""
    property string statusText: qsTr("En attente de configuration")

    padding: 20
    topPadding: 0

    Component.onCompleted: Qt.callLater(root.reload)
    onTokenChanged: Qt.callLater(root.reload)
    onIp_addressChanged: Qt.callLater(root.reload)
    onPortChanged: Qt.callLater(root.reload)

    SettingsMapper {
        id: homeAssistantSettings

        settingsCategory: "homeAssistant"
        property alias ip_address: root.ip_address
        property alias port: root.port
        property alias token: root.token
    }

    property Component formModel: FormObjectModel {
        FormTextFieldDelegate {
            label: qsTr("Adresse IPv4")
            validator: Ipv4Validator {}
            targetProperty: "ip_address"
        }
        FormTextFieldDelegate {
            label: qsTr("Port")
            validator: SocketPortValidator{}
            valueType: FormValueTypes.Integer
            targetProperty: "port"
        }
        FormTextFieldPasswordDelegate {
            label: qsTr("Jeton d'accès longue durée")
            targetProperty: "token"
        }
    }

    RestClient {
        id: homeAssistantClient

        name: "homeAssistant"
        baseUrl: ("http://%1").arg(root.ip_address)
        port: root.port
        trailingSlash: false
        globalHeaders: root.token.length > 0
                       ? ({ "Authorization": ("Bearer %1").arg(root.token) })
                       : ({})
    }

    VariantListModel {
        id: lightModel

        roles: ["entityId", "name", "state", "brightness", "supportsBrightness", "lightColor"]
        selectPolicy: QVariantListModelPolicies.Disabled
        submitPolicy: QVariantListModelPolicies.Disabled
    }

    function reload() {
        if (root.token.length === 0) {
            root.statusText = qsTr("Jeton Home Assistant manquant")
            return
        }
        if (homeAssistantClient.rootClass.loading)
            return

        root.lastError = ""
        root.statusText = qsTr("Lecture de %1").arg(root.statesUrl)
        console.info("[HomeAssistant] GET " + root.statesUrl)

        const reply = homeAssistantClient.rootClass.get("api/states")
        reply.finished.connect(root.onReplyFinished)
        reply.succeeded.connect(root.onStatesLoaded)
        reply.failed.connect(root.onReplyFailed)
    }

    function rgbToColor(rgb: var): color {
        if (!rgb || rgb.length !== 3)
            return "transparent"

        return Qt.rgba(rgb[0] / 255, rgb[1] / 255, rgb[2] / 255, 1)
    }

    function lightEntry(entityId: string, state: string, attributes: var): var {
        return {
            "entityId": entityId,
            "name": attributes.friendly_name ?? entityId.slice("light.".length),
            "state": state,
            "brightness": attributes.brightness ?? 0,
            "supportsBrightness": attributes.brightness !== undefined,
            "lightColor": root.rgbToColor(attributes.rgb_color)
        }
    }

    function updateLightState(entityId: string, state: var) {
        if (!entityId.startsWith("light."))
            return

        const index = lightModel.ModelHelper.indexOf("entityId", entityId)
        if (index < 0) {
            root.reload()
            return
        }

        const attributes = state.attributes ?? ({})
        lightModel.ModelHelper.set(index, root.lightEntry(entityId, state.state, attributes))
        root.lastUpdate = Qt.formatTime(new Date(), "HH:mm:ss")
        root.statusText = qsTr("Synchronisation temps réel active")
    }

    function onStatesLoaded(httpStatus: int, reply: var) {
        lightModel.clear()
        root.entityCount = reply.length

        for (const entity of reply) {
            if (!entity.entity_id.startsWith("light."))
                continue

            lightModel.append(root.lightEntry(entity.entity_id, entity.state, entity.attributes ?? ({})))
        }

        root.statusText = qsTr("%1 entités reçues, %2 lumière(s)").arg(root.entityCount).arg(lightModel.count)
        console.info("[HomeAssistant] HTTP " + httpStatus + ": " + root.statusText)
    }

    HassSocket {
        id: homeAssistantSocket

        connection: "homeAssistant"
        accessToken: root.token
        enabled: root.visible && root.token.length > 0
        onSubscribedChanged: {
            if (subscribed) {
                root.statusText = qsTr("Synchronisation temps réel active")
                console.info("[HomeAssistant] WebSocket abonné aux changements d'état")
                root.reload()
            }
        }
        onAuthenticationFailed: (error) => {
            root.lastError = error
            root.statusText = qsTr("Authentification WebSocket refusée")
            console.warn("[HomeAssistant] " + root.statusText + ": " + error)
        }
        onEntityStateChanged: (entityId, state) => root.updateLightState(entityId, state)
    }

    function onReplyFailed(httpStatus: int, reply: var) {
        root.lastError = RestHelper.parseBody(reply, false)
        root.statusText = qsTr("Échec HTTP %1").arg(httpStatus)
        console.warn("[HomeAssistant] " + root.statusText + ": " + root.lastError)

        const settings = {
            "caption": root.statusText,
            "details": root.lastError,
            "diagnose": false
        }
        SnackbarManager.showCritical(settings)
    }

    function onReplyFinished(result: bool, httpStatus: int, reply: var) {
        root.lastHttpStatus = httpStatus
        root.lastUpdate = Qt.formatTime(new Date(), "HH:mm:ss")
        if (!result && root.lastError.length === 0)
            root.lastError = homeAssistantClient.rootClass.errorString
        console.info("[HomeAssistant] Réponse HTTP " + httpStatus + ", succès=" + result)
    }

    function setLightState(entityId: string, enabled: bool) {
        const service = enabled ? "turn_on" : "turn_off"
        root.statusText = qsTr("Commande %1 pour %2").arg(service).arg(entityId)
        console.info("[HomeAssistant] POST api/services/light/" + service + " pour " + entityId)
        const reply = homeAssistantClient.rootClass.post(("api/services/light/%1").arg(service), {
            "entity_id": entityId
        })
        reply.finished.connect(root.onReplyFinished)
        reply.succeeded.connect(root.reload)
        reply.failed.connect(root.onReplyFailed)
    }

    function setBrightness(entityId: string, brightness: int) {
        root.statusText = qsTr("Réglage de la luminosité pour %1").arg(entityId)
        console.info("[HomeAssistant] POST api/services/light/turn_on pour " + entityId)
        const reply = homeAssistantClient.rootClass.post("api/services/light/turn_on", {
            "entity_id": entityId,
            "brightness": Math.round(Math.max(0, Math.min(255, brightness)))
        })
        reply.finished.connect(root.onReplyFinished)
        reply.succeeded.connect(root.reload)
        reply.failed.connect(root.onReplyFailed)
    }

    contentItem: PaneGridView {
        rowSpacing: 20
        columnSpacing: 20
        itemWidth: 320
        itemHeight: 132
        model: lightModel
        canRefresh: true
        onRefreshTriggered: root.reload()

        header: ColumnLayout {
            width: parent.width

            RowLayout {
                Layout.fillWidth: true

                FabButton {
                    icon.source: MaterialIcons.sync
                    hint: qsTr("Actualiser")
                    enabled: !root.loading
                    onClicked: root.reload()
                }
                BasicBusyIndicator {
                    visible: root.loading
                    backgroundColor: "transparent"
                    size: 40
                }
                LayoutSpring {}
                LabelWithCaption {
                    horizontalAlignment: Qt.AlignRight
                    textFont: Style.textTheme.title1
                    text: qsTr("Home Assistant")
                    captionFont: Style.textTheme.subtitle2
                    caption: qsTr("%1 lumière(s)").arg(lightModel.count)
                }
                FabButton {
                    icon.source: MaterialIcons.pen
                    hint: qsTr("Configurer")
                    onClicked: {
                        const model = root.formModel.createObject(root) as FormObjectModel
                        DialogManager.showForm({
                            "formModel": model,
                            "formObject": homeAssistantSettings,
                            "onClosed": function() {
                                model.destroy()
                            }
                        })
                    }
                }
            }

            Label {
                Layout.fillWidth: true
                text: root.statusText
                font: Style.textTheme.subtitle2
                elide: Text.ElideRight
            }

            Label {
                Layout.fillWidth: true
                visible: root.lastError.length > 0
                text: root.lastError
                color: Style.colorError
                font: Style.textTheme.caption1
                elide: Text.ElideRight
            }

            Label {
                Layout.fillWidth: true
                text: qsTr("Dernière réponse : HTTP %1 à %2").arg(root.lastHttpStatus).arg(root.lastUpdate)
                font: Style.textTheme.caption1
                visible: root.lastUpdate.length > 0
            }
        }

        delegate: MushroomLightCard {
            onStateChangeRequested: (enabled) => root.setLightState(entityId, enabled)
            onBrightnessChangeRequested: (brightness) => root.setBrightness(entityId, brightness)
        }
    }
}
