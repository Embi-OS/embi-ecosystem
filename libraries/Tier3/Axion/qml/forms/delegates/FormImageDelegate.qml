pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier1.Models
import Eco.Tier3.Axion

FormObject {
    id: root

    formType: FormTypes.None
    defaultValue: ""

    property string image: ""

    delegate: SvgImage {
        asynchronous: false
        icon: root.image
        iconWidth: width
        iconHeight: height
        aspectRatio: Qt.KeepAspectRatio
    }
}
