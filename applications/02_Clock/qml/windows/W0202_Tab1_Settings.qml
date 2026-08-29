import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion
import L02_Clock

BasicPane {
    id: root

    Navigation.guarded: splitView.Navigation.guarded

    padding: 10
    topPadding: 0
    contentItem: PaneSplitView {
        id: splitView
        model: Settings_Clock_Machine {}
    }
}
