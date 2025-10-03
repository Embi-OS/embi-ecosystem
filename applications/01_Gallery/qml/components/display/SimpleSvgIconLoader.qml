import QtQuick
import Eco.Tier3.Axion

Column {
    id: root

    spacing: 16

    property font font: Style.textTheme.body1

    component SimpleIcon: Column {
        id: simpleIcon

        required property string icon
        required property int size
        required property string text
        property color color: RandomUtils.color()
        property bool dimmed: false

        spacing: 8

        SvgIconLoader {
            anchors.horizontalCenter: parent.horizontalCenter

            icon: simpleIcon.icon
            color: simpleIcon.color
            size: simpleIcon.size
            dimmed: simpleIcon.dimmed

            DebugRectangle {}
        }

        BasicLabel {
            anchors.horizontalCenter: parent.horizontalCenter
            text: simpleIcon.text
            font: Style.textTheme.caption1
        }
    }

    component ScaledIcon: Column {
        id: scaledIcon

        required property string icon
        required property int size
        required property int aspectRatio
        required property string text
        property color color: Style.colorTransparent

        spacing: 8

        SvgIconLoader {
            id: scaledLoader
            anchors.horizontalCenter: parent.horizontalCenter

            icon: scaledIcon.icon
            color: scaledIcon.color
            size: scaledIcon.size
            iconWidth: scaledIcon.size
            iconHeight: scaledIcon.size
            aspectRatio: scaledIcon.aspectRatio

            DebugRectangle {}
        }

        BasicLabel {
            anchors.horizontalCenter: parent.horizontalCenter
            text: scaledIcon.text+ ` (${scaledLoader.implicitWidth}x${scaledLoader.implicitHeight})`
            font: Style.textTheme.caption1
        }
    }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        BasicSlider {
            anchors.horizontalCenter: parent.horizontalCenter
            id: slider
            from: 8
            to: 128
            value: 32
        }

        Row {
            height: 128
            spacing: root.spacing/2
            anchors.horizontalCenter: parent.horizontalCenter
            BasicLabel {
                anchors.verticalCenter: parent.verticalCenter
                text: "Standard icon size: "+Style.roundToIconSize(slider.value)
                font: root.font
            }
            SvgIconLoader {
                anchors.verticalCenter: parent.verticalCenter
                icon: "qrc:/images/logo.svg"
                size: Style.roundToIconSize(slider.value)
                color: Style.colorWhite
            }
        }
    }

    BasicSeparator {
        orientation: Qt.Horizontal
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: root.spacing

        SimpleIcon {
            icon: MaterialIcons.abacus
            size: Style.iconSizesMedium
            color: Style.colorWhite
            text: "normal"
            dimmed: false
        }

        SimpleIcon {
            icon: MaterialIcons.abacus
            size: Style.iconSizesMedium
            color: Style.colorWhite
            text: "dimmed"
            dimmed: true
        }

        SimpleIcon {
            icon: "qrc:/images/others/warning.svg"
            size: Style.iconSizesMedium
            color: "transparent"
            text: "transparent"
            dimmed: false
        }

        SimpleIcon {
            icon: "qrc:/images/others/warning.svg"
            size: Style.iconSizesMedium
            color: "transparent"
            text: "dimmed"
            dimmed: true
        }
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: root.spacing

        SimpleIcon {
            anchors.bottom: parent.bottom
            icon: MaterialIcons.accountHardHat
            size: Style.iconSizesMini
            text: "mini "+size
        }
        SimpleIcon {
            anchors.bottom: parent.bottom
            icon: MaterialIcons.accountHardHat
            size: Style.iconSizesTiny
            text: "tiny "+size
        }
        SimpleIcon {
            anchors.bottom: parent.bottom
            icon: MaterialIcons.accountHardHat
            size: Style.iconSizesSmall
            text: "small "+size
        }
        SimpleIcon {
            anchors.bottom: parent.bottom
            icon: MaterialIcons.accountHardHat
            size: Style.iconSizesMedium
            text: "medium "+size
        }
        SimpleIcon {
            anchors.bottom: parent.bottom
            icon: MaterialIcons.accountHardHat
            size: Style.iconSizesLarge
            text: "large "+size
        }
        SimpleIcon {
            anchors.bottom: parent.bottom
            icon: MaterialIcons.accountHardHat
            size: Style.iconSizesHuge
            text: "huge "+size
        }
        SimpleIcon {
            anchors.bottom: parent.bottom
            icon: MaterialIcons.accountHardHat
            size: Style.iconSizesEnormous
            text: "enormous "+size
        }
    }

    BasicSeparator {
        orientation: Qt.Horizontal
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Row {
        anchors.horizontalCenter: parent.horizontalCenter

        spacing: root.spacing

        ScaledIcon {
            anchors.bottom: parent.bottom
            icon: "qrc:/images/qtLogoGreen.svg"
            size: 40
            aspectRatio: Qt.IgnoreAspectRatio
            text: "IgnoreAspectRatio"
        }
        ScaledIcon {
            anchors.bottom: parent.bottom
            icon: "qrc:/images/qtLogoGreen.svg"
            size: 40
            aspectRatio: Qt.KeepAspectRatio
            text: "KeepAspectRatio"
        }
        ScaledIcon {
            anchors.bottom: parent.bottom
            icon: "qrc:/images/qtLogoGreen.svg"
            size: 40
            aspectRatio: Qt.KeepAspectRatioByExpanding
            text: "KeepAspectRatioByExpanding"
        }
    }
}
