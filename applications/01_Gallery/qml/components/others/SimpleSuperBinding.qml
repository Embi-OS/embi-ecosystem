import QtQuick
import Eco.Tier3.Axion

import Qt.labs.animation

Item {
    width: 640
    height: 480

    Column {
        anchors.centerIn: parent
        spacing: 20
        Slider {
            id: slider
        }
        Slider {
            SuperBinding on value {
                value: slider.value
                policy: SuperBindingPolicies.Direct
                when: true
                inhibitTime: 0
            }
        }
        Slider {
            SuperBinding on value {
                value: slider.value
                policy: SuperBindingPolicies.Delayed
                when: true
                inhibitTime: 0
            }
        }
        Slider {
            SuperBinding on value {
                value: slider.value
                policy: SuperBindingPolicies.Delayed
                when: true
                inhibitTime: 200
            }
        }
    }
}
