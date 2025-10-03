import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    property double inset: 0
    topInset: inset
    bottomInset: inset
    leftInset: inset
    rightInset: inset

    padding: inset
    topPadding: padding + topInset
    bottomPadding: padding + bottomInset
    leftPadding: padding + leftInset
    rightPadding: padding + rightInset

    spacing: 5

    property string text: qsTr("Sélectionner un réglage")
    property font textFont: Style.textTheme.headline3
    property color textColor: Style.colorPrimary

    property string caption: ""
    property font captionFont: Style.textTheme.headline5
    property color captionColor: Style.colorPrimary

    property IconObject icon: IconObject {
        source: MaterialIcons.cogs
        width: Style.roundToIconSize(root.width)
        height: Style.roundToIconSize(root.width)
        color: Style.colorPrimary
        cache: true
    }

    property int horizontalAlignment: Qt.AlignHCenter
    property int verticalAlignment: Qt.AlignVCenter

    contentItem: ColumnLayout {
        spacing: root.spacing

        LayoutSpring {
            visible: root.verticalAlignment===Qt.AlignBottom || root.verticalAlignment===Qt.AlignVCenter
        }

        SvgColorImage {
            Layout.alignment: root.horizontalAlignment
            icon: root.icon.source
            iconWidth: root.icon.width
            iconHeight: root.icon.height
            color: root.icon.color
            cache: root.icon.cache
        }

        LabelWithCaption {
            Layout.alignment: root.horizontalAlignment
            text: root.text
            textFont: root.textFont
            textColor: root.textColor
            caption: root.caption
            captionFont: root.captionFont
            captionColor: root.captionColor
        }

        LayoutSpring {
            visible: root.verticalAlignment===Qt.AlignTop || root.verticalAlignment===Qt.AlignVCenter
        }
    }
}
