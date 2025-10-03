import QtQuick
import Eco.Tier3.Axion

QtObject {
    id: root

    property int index: -1
    required default property Component navItem
    property BasicNavItem item: null
    property var properties: null

    function setProperties(properties: var): BasicNavObject {
        root.properties = properties;
        return root as BasicNavObject;
    }
}
