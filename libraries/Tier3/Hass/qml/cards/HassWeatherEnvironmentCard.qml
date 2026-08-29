import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Hass

BasicPane {
    id: root

    required property HassStateMapper temperatureSensor
    required property HassStateMapper humiditySensor
    required property HassStateMapper weather

    title: qsTr("Environnement")
    comment: qsTr("Température, humidité et météo")
    drawFrame: true

    contentItem: GridLayout {
        columns: 2
        rowSpacing: 8
        columnSpacing: 8

        HassWeatherDelegate {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            entity_id: root.weather.entity_id
            state: root.weather.state
            attributes: root.weather.attributes
        }

        HassSensorDelegate {
            Layout.fillWidth: true
            entity_id: root.temperatureSensor.entity_id
            state: root.temperatureSensor.state
            attributes: root.temperatureSensor.attributes
        }

        HassSensorDelegate {
            Layout.fillWidth: true
            entity_id: root.humiditySensor.entity_id
            state: root.humiditySensor.state
            attributes: root.humiditySensor.attributes
        }
    }
}
