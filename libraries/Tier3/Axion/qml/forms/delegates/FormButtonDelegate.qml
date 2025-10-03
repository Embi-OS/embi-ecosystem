pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.None
    defaultValue: false

    highlighted: false
    icon: ""

    signal clicked()

    delegate: FabButton {
        id: button

        enabled: root.enabled // && root.editable

        highlighted: root.highlighted
        text: root.label
        icon.source: root.icon
        elevation: 0
        radius: Style.buttonRadius

        onClicked: root.clicked()
    }
}
