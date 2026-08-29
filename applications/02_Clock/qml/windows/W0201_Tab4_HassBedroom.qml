pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier1.ProxyModel
import Eco.Tier1.Utils
import Eco.Tier2.Rest
import Eco.Tier3.Axion
import Eco.Tier3.Hass
import L02_Clock

BasicPane {
    id: root

    padding: 20

    function editInputDateTime(entityId: string, hasDate: bool, hasTime: bool, valueDateTime: date) {
        if (hasDate && hasTime) {
            DialogManager.showDate({
                "selectedDate": valueDateTime,
                "onDateSelected": function(date) {
                    DialogManager.showTime({
                        "selectedTime": valueDateTime,
                        "onTimeSelected": function(time) {
                            HassManager.setInputDateTime(entityId, DateTimeUtils.dateTime(date, time))
                        }
                    })
                }
            })
        } else if (hasDate) {
            DialogManager.showDate({
                "selectedDate": valueDateTime,
                "onDateSelected": function(date) {
                    HassManager.setInputDate(entityId, date)
                }
            })
        } else if (hasTime) {
            DialogManager.showTime({
                "selectedTime": valueDateTime,
                "onTimeSelected": function(time) {
                    HassManager.setInputTime(entityId, time)
                }
            })
        }
    }

    HassSocket {
        id: homeAssistantSocket

        connection: "hass"
        accessToken: HassManager.apiToken
        enabled: root.visible && accessToken.length > 0
    }

    HassEntityModel {
        id: entityModel

        HassSocketAttached.socket: homeAssistantSocket
        connection: "hass"
        selectWhen: root.visible
    }

    ProxyModel {
        id: entityProxyModel
        sourceModel: entityModel
        sortRoleName: "entity_id"
        sortOrder: Qt.AscendingOrder
        filterRoleName: "domain"
        filterValue: "light"
    }

    HassStateMapper {
        id: dodoSceneMapper
        model: entityModel
        value: "scene.chambre_chevet_1_dodo"
    }

    HassStateMapper {
        id: weatherMapper
        model: entityModel
        value: "weather.forecast_maison"
    }


    contentItem: ColumnLayout {
        RowLayout {
            Layout.fillWidth: true

            HassAutomationDatetimeCard {
                Layout.fillWidth: true
                color: "palevioletred"
                timeMapper: HassStateMapper {
                    model: entityModel
                    value: "input_datetime.reveil_lisiane"
                }
                automationMapper: HassStateMapper {
                    model: entityModel
                    value: "automation.reveil_lisiane_automatisation"
                }
                onClicked: root.editInputDateTime(time_entity_id, hasDate, hasTime, valueDateTime)
                onPressAndHold: HassManager.setEntityEnabled(automation_entity_id, !isAutomationOn)
            }

            HassAutomationDatetimeCard {
                Layout.fillWidth: true
                color: "lightseagreen"
                timeMapper: HassStateMapper {
                    model: entityModel
                    value: "input_datetime.reveil_romain"
                }
                automationMapper: HassStateMapper {
                    model: entityModel
                    value: "automation.reveil_romain_automatisation"
                }
                onClicked: root.editInputDateTime(time_entity_id, hasDate, hasTime, valueDateTime)
                onPressAndHold: HassManager.setEntityEnabled(automation_entity_id, !isAutomationOn)
            }

            HassAutomationDatetimeCard {
                Layout.fillWidth: true
                icon.source: MaterialIcons.bedClock
                color: "salmon"
                timeMapper: HassStateMapper {
                    model: entityModel
                    value: "input_datetime.heure_dodo"
                }
                automationMapper: HassStateMapper {
                    model: entityModel
                    value: "automation.dodo_automatisation"
                }
                onClicked: root.editInputDateTime(time_entity_id, hasDate, hasTime, valueDateTime)
                onPressAndHold: HassManager.setEntityEnabled(automation_entity_id, !isAutomationOn)
            }
        }

        RowLayout {
            Layout.fillWidth: true

            HassLightCard {
                Layout.fillWidth: true
                mapper: HassStateMapper {
                    model: entityModel
                    value: "light.chambre_chevet"
                }
                onClicked: HassManager.setEntityEnabled(entity_id, !isOn)
            }
            HassSceneDelegate {
                entity_id: dodoSceneMapper.entity_id
                state: dodoSceneMapper.state
                attributes: dodoSceneMapper.attributes
                iconSource: isUnavailable ? MaterialIcons.alertCircleOutline : MaterialIcons.lightbulbNight
                onClicked: HassManager.activateScene(entity_id)
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        BasicListView {
            Layout.fillWidth: true
            implicitHeight: 80
            orientation: ListView.Horizontal
            model: entityProxyModel
            delegate: HassLightDelegate {
                height: (ListView.view as BasicListView).viewHeight
                required entity_id
                required state
                required attributes
                onClicked: HassManager.setEntityEnabled(entity_id, !isOn)
            }
        }

        BasicSeparator {
            orientation: Qt.Horizontal
        }

        RowLayout {
            Layout.fillWidth: true

            HassTemperatureHumidityCard {
                primaryText: qsTr("Chambre")
                temperatureMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.chambre_xiaomi_lywsd03mmc_temperature"
                }
                humidityMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.chambre_xiaomi_lywsd03mmc_humidity"
                }
                onClicked: console.log(FormatUtils.variantToLog(temperature_attributes))
            }
            HassTemperatureHumidityCard {
                primaryText: qsTr("Cuisine")
                temperatureMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.cuisine_xiaomi_lywsd03mmc_temperature"
                }
                humidityMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.cuisine_xiaomi_lywsd03mmc_humidity"
                }
                onClicked: console.log(FormatUtils.variantToLog(temperature_attributes))
            }
            HassTemperatureHumidityCard {
                primaryText: qsTr("Salon")
                temperatureMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.salon_xiaomi_lywsd03mmc_temperature"
                }
                humidityMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.salon_xiaomi_lywsd03mmc_humidity"
                }
                onClicked: console.log(FormatUtils.variantToLog(temperature_attributes))
            }
            HassTemperatureHumidityCard {
                primaryText: qsTr("Bureau")
                temperatureMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.bureau_xiaomi_lywsd03mmc_temperature"
                }
                humidityMapper: HassStateMapper {
                    model: entityModel
                    value: "sensor.bureau_xiaomi_lywsd03mmc_humidity"
                }
                onClicked: console.log(FormatUtils.variantToLog(temperature_attributes))
            }
        }

        HassWeatherDelegate {
            Layout.fillWidth: true
            entity_id: weatherMapper.entity_id
            state: weatherMapper.state
            attributes: weatherMapper.attributes
            onClicked: console.log(FormatUtils.variantToLog(attributes))
        }
    }
}
