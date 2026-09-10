import QtQuick
import Eco.Tier3.Axion

Gradient {
    id: root

    property int minKelvin: 2000
    property int maxKelvin: 6500

    GradientStop {
        position: 0
        color: ColorUtils.colorTemperatureToColor(root.minKelvin)
    }
    GradientStop {
        position: 0.5
        color: ColorUtils.colorTemperatureToColor((root.minKelvin + root.maxKelvin) / 2)
    }
    GradientStop {
        position: 1
        color: ColorUtils.colorTemperatureToColor(root.maxKelvin)
    }
}
