import QtQuick
import Eco.Tier3.Axion

Item {
    id: root
    visible: (icon !== "")

    implicitWidth: (helper.ready || aspectRatio!==Qt.IgnoreAspectRatio) ? img.implicitWidth : iconWidth
    implicitHeight: (helper.ready || aspectRatio!==Qt.IgnoreAspectRatio) ? img.implicitHeight : iconHeight

    property bool dimmed: false

    property int size: 40
    property alias iconWidth: helper.iconWidth
    property alias iconHeight: helper.iconHeight
    property alias aspectRatio: helper.aspectRatio
    property alias asynchronous: helper.asynchronous
    property alias icon: helper.icon
    property alias ready: helper.ready

    smooth: true
    antialiasing: true
    property alias mipmap: img.mipmap
    property alias cache: img.cache
    property alias fillMode: img.fillMode
    property alias mirror: img.mirror

    Image {
        id: img
        // visible: root.ready

        anchors.centerIn: parent
        anchors.alignWhenCentered: true
        width: root.width
        height: root.height

        smooth: root.smooth
        antialiasing: root.antialiasing
        asynchronous: root.asynchronous
        cache: true

        opacity: (root.dimmed ? 0.5 : 1.0)
        fillMode: Image.Pad

        sourceSize: helper.sourceSize
        source: helper.icon

        SvgImageHelper {
            id: helper
            iconWidth: root.size
            iconHeight: root.size
        }
    }
}
