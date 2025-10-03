import QtQuick
import Eco.Tier3.Axion

BasicLabel {
    id: root

    font: Style.textTheme.title1
    opacity: (1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)) * (Tumbler.tumbler.enabled ? 1 : 0.5)
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
}
