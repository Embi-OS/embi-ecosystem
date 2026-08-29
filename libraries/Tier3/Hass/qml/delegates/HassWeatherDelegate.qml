import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

HassDelegate {
    id: root

    property string entity_id: ""
    property string state: ""
    property var attributes: ({})

    readonly property bool isUnavailable: state === "unavailable"
    readonly property bool isUnknown: state === "unknown"
    readonly property double temperature: attributes?.temperature ?? 0
    readonly property string temperatureUnit: attributes?.temperature_unit ?? ""
    readonly property bool hasTemperature: temperature !== undefined && temperature !== null

    function conditionText() {
        switch(state) {
        case "clear-night": return qsTr("Ciel dégagé (nuit)")
        case "cloudy": return qsTr("Nuageux")
        case "fog": return qsTr("Brouillard")
        case "hail": return qsTr("Grêle")
        case "lightning": return qsTr("Orage")
        case "lightning-rainy": return qsTr("Orage et pluie")
        case "partlycloudy": return qsTr("Partiellement nuageux")
        case "pouring": return qsTr("Fortes pluies")
        case "rainy": return qsTr("Pluie")
        case "snowy": return qsTr("Neige")
        case "snowy-rainy": return qsTr("Neige et pluie")
        case "sunny": return qsTr("Ensoleillé")
        case "windy": return qsTr("Venteux")
        case "windy-variant": return qsTr("Nuageux et venteux")
        case "exceptional": return qsTr("Conditions exceptionnelles")
        default: return state
        }
    }

    primaryText: attributes?.friendly_name ?? entity_id
    secondaryText: {
        if(isUnavailable)
            return qsTr("Indisponible")
        if(isUnknown)
            return qsTr("État inconnu")
        if(hasTemperature)
            return temperatureUnit.length > 0
                    ? qsTr("%1 %2 · %3").arg(temperature).arg(temperatureUnit).arg(conditionText())
                    : qsTr("%1 · %2").arg(temperature).arg(conditionText())
        return conditionText()
    }
    active: !isUnavailable && !isUnknown
    iconColor: {
        switch(state) {
        case "sunny": return Style.amber
        case "clear-night": return Style.indigo
        case "cloudy":
        case "partlycloudy":
        case "fog": return Style.blueGrey
        case "hail":
        case "rainy":
        case "pouring":
        case "snowy":
        case "snowy-rainy": return Style.blue
        case "lightning":
        case "lightning-rainy": return Style.orange
        case "windy":
        case "windy-variant": return Style.teal
        case "exceptional": return Style.red
        default: return Style.cyan
        }
    }
    iconSource: {
        if(isUnavailable)
            return MaterialIcons.alertCircleOutline
        if(isUnknown)
            return MaterialIcons.helpCircleOutline

        switch(state) {
        case "clear-night": return MaterialIcons.weatherNight
        case "cloudy": return MaterialIcons.weatherCloudy
        case "fog": return MaterialIcons.weatherFog
        case "hail": return MaterialIcons.weatherHail
        case "lightning": return MaterialIcons.weatherLightning
        case "lightning-rainy": return MaterialIcons.weatherLightningRainy
        case "partlycloudy": return MaterialIcons.weatherPartlyCloudy
        case "pouring": return MaterialIcons.weatherPouring
        case "rainy": return MaterialIcons.weatherRainy
        case "snowy": return MaterialIcons.weatherSnowy
        case "snowy-rainy": return MaterialIcons.weatherSnowyRainy
        case "sunny": return MaterialIcons.weatherSunny
        case "windy": return MaterialIcons.weatherWindy
        case "windy-variant": return MaterialIcons.weatherWindyVariant
        case "exceptional": return MaterialIcons.alertCircleOutline
        default: return MaterialIcons.weatherCloudy
        }
    }
}
