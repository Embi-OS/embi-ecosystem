import QtQuick
import QtQuick.Effects

MultiEffect {
    id: root

    property int elevation: 0

    shadowEnabled: elevation>0
    shadowOpacity: Math.min(elevation, 12)/12
}
