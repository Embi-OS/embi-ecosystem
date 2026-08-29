pragma ComponentBehavior: Bound
import QtQuick
import Qt.labs.qmlmodels
import Eco.Tier3.Hass

DelegateChooser {
    id: root

    role: "domain"

    signal entityClicked(int index)

    DelegateChoice {
        roleValue: "light"
        HassLightDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }

    DelegateChoice {
        roleValue: "scene"

        HassSceneDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }

    DelegateChoice {
        roleValue: "sensor"
        HassSensorDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }

    DelegateChoice {
        roleValue: "weather"
        HassWeatherDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }

    DelegateChoice {
        roleValue: "update"
        HassUpdateDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }

    DelegateChoice {
        roleValue: "input_datetime"
        HassInputDatetimeDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }

    DelegateChoice {
        roleValue: "automation"
        HassAutomationDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }

    DelegateChoice {
        HassEntityDelegate {
            required index
            required entity_id
            required state
            required attributes
            onClicked: root.entityClicked(index)
        }
    }
}
