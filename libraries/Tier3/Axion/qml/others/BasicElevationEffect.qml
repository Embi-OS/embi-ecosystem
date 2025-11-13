import QtQuick
import QtQuick.Effects

// TODO: Qt 6.9 RectangularShadow
MultiEffect {
    id: root

    property int elevation: 0

    shadowEnabled: elevation>0
    shadowOpacity: Math.min(elevation, 12)/12
}
