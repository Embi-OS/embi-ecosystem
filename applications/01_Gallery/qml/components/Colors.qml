pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    component ColorSquare: Rectangle {
        id: colorSquare
        width: 100
        height: 100
        border.width: 1
        border.color: Style.colorWhite

        required property string name
        color: Style[name]

        LabelWithCaption {
            anchors.margins: 4
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            width: parent.width

            color: ColorUtils.isDarkColor(colorSquare.color) ? Style.colorWhite : Style.colorBlack
            text: colorSquare.name
            textFont: Style.textTheme.caption1
            caption: ColorUtils.name(colorSquare.color, true)
            captionFont: Style.textTheme.caption2
        }
    }

    spacing: 0

    RowLayout {
        spacing: root.spacing
        ColorSquare {
            name: "colorBackground"
        }
        ColorSquare {
            name: "colorPrimaryDarkest"
        }
        ColorSquare {
            name: "colorPrimaryDarker"
        }
        ColorSquare {
            name: "colorPrimaryDark"
        }
        ColorSquare {
            name: "colorPrimary"
        }
        ColorSquare {
            name: "colorPrimaryLight"
        }
        ColorSquare {
            name: "colorPrimaryLighter"
        }
        ColorSquare {
            name: "colorPrimaryLightest"
        }
    }

    RowLayout {
        spacing: root.spacing
        ColorSquare {
            name: "colorAccentDark"
        }
        ColorSquare {
            name: "colorAccent"
        }
        ColorSquare {
            name: "colorAccentLight"
        }
        ColorSquare {
            name: "colorAccentFade"
        }
        ColorSquare {
            name: "colorVariantDark"
        }
        ColorSquare {
            name: "colorVariant"
        }
        ColorSquare {
            name: "colorVariantLight"
        }
        ColorSquare {
            name: "colorVariantFade"
        }
    }

    RowLayout {
        spacing: root.spacing
        ColorSquare {
            name: "colorOptionDark"
        }
        ColorSquare {
            name: "colorOption"
        }
        ColorSquare {
            name: "colorOptionLight"
        }
        ColorSquare {
            name: "colorOptionFade"
        }
        ColorSquare {
            name: "colorBlack"
        }
        ColorSquare {
            name: "colorBlackFade"
        }
        ColorSquare {
            name: "colorWhite"
        }
        ColorSquare {
            name: "colorWhiteFade"
        }
    }

    RowLayout {
        spacing: root.spacing
        ColorSquare {
            name: "colorTrace"
        }
        ColorSquare {
            name: "colorDebug"
        }
        ColorSquare {
            name: "colorInfo"
        }
        ColorSquare {
            name: "colorWarning"
        }
        ColorSquare {
            name: "colorError"
        }
        ColorSquare {
            name: "colorFatal"
        }
        ColorSquare {
            name: "colorSuccess"
        }
        ColorSquare {
            name: "colorTransparent"
        }
    }

    Item {
        height: 5
        Layout.fillWidth: true
    }

    RowLayout {
        spacing: root.spacing
        ColorSquare {
            name: "blueGrey"
        }
        ColorSquare {
            name: "grey"
        }
        ColorSquare {
            name: "brown"
        }
        ColorSquare {
            name: "deepOrange"
        }
        ColorSquare {
            name: "orange"
        }
        ColorSquare {
            name: "amber"
        }
        ColorSquare {
            name: "yellow"
        }
        ColorSquare {
            name: "lime"
        }
    }

    RowLayout {
        spacing: root.spacing
        ColorSquare {
            name: "green"
        }
        ColorSquare {
            name: "teal"
        }
        ColorSquare {
            name: "cyan"
        }
        ColorSquare {
            name: "lightBlue"
        }
        ColorSquare {
            name: "blue"
        }
        ColorSquare {
            name: "indigo"
        }
        ColorSquare {
            name: "deepPurple"
        }
        ColorSquare {
            name: "purple"
        }
    }
}
