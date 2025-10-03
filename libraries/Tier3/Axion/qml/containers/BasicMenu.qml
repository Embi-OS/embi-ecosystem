pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Menu {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            contentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             contentHeight + topPadding + bottomPadding)

    ExtraPosition.position: ItemPositions.BottomStart

    cascade: true
    margins: 0
    padding: Style.menuPadding

    topInset: Style.menuInset
    bottomInset: Style.menuInset
    leftInset: Style.menuInset
    rightInset: Style.menuInset

    transformOrigin: !cascade ? Item.Top : Item.TopLeft

    delegate: BasicMenuItem {}

    property double elevation: Style.menuElevation
    modal: true // Back darker
    focus: true

    T.Overlay.modal: BasicOverlay {}
    T.Overlay.modeless: BasicOverlay {}

    enter: Transition {
        NumberAnimation { property: "scale"; from: 0.9; to: 1.0; easing.type: Easing.OutQuint; duration: 220 }
        NumberAnimation { property: "opacity"; from: 0.0; to: 1.0; easing.type: Easing.OutCubic; duration: 150 }
    }

    exit: Transition {
        NumberAnimation { property: "scale"; from: 1.0; to: 0.9; easing.type: Easing.OutQuint; duration: 220 }
        NumberAnimation { property: "opacity"; from: 1.0; to: 0.0; easing.type: Easing.OutCubic; duration: 150 }
    }

    property alias footer: listView.footer
    property alias footerItem: listView.footerItem
    property alias footerPositioning: listView.footerPositioning

    property alias header: listView.header
    property alias headerItem: listView.headerItem
    property alias headerPositioning: listView.headerPositioning

    property alias section: listView.section

    contentItem: ListView {
        id: listView
        implicitHeight: contentHeight
        implicitWidth: contentWidth
        spacing: root.spacing
        reuseItems: true

        model: root.contentModel
        interactive: contentHeight > height
        clip: true
        currentIndex: root.currentIndex

        T.ScrollIndicator.vertical: BasicScrollIndicator {
            color: Style.colorPrimaryLight
        }
    }

    property color backgroundColor: Style.colorPrimary
    property color borderColor: Style.colorPrimaryLight
    property double backgroundImplicitWidth: Style.menuImplicitWidth
    property double backgroundImplicitHeight: Style.menuImplicitHeight

    background: Rectangle {
        implicitWidth: root.backgroundImplicitWidth
        implicitHeight: root.backgroundImplicitHeight

        radius: Style.menuRadius
        color: ColorUtils.transparent(root.backgroundColor, 1.0)
        border.width: Style.menuBorderWidth
        border.color: ColorUtils.transparent(root.borderColor, 1.0)

        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }
    }
}
