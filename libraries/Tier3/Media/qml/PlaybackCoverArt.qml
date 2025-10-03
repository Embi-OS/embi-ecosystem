pragma ComponentBehavior: Bound
import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Media

Item {
    id: root

    implicitWidth: height

    required property MediaPlayer mediaPlayer
    readonly property mediaMetaData metaData: mediaPlayer.metaData
    readonly property int mediaStatus: mediaPlayer.mediaStatus

    readonly property var metaDataImage: metaData.value(MediaMetaData.CoverArtImage)
    readonly property alias displayImage: coverArt.source

    property double elevation: Style.flat ? 0 : 8

    Image {
        id: coverArt
        anchors.centerIn: parent
        sourceSize.height: root.height
        sourceSize.width: root.width
        fillMode: Image.PreserveAspectFit
        antialiasing: true
        asynchronous: true

        VariantImageHelper on source {
            id: helper
            enabled: root.mediaStatus!==MediaPlayer.LoadingMedia
            backup: "qrc:/images/media/Album.png"
            source: root.metaDataImage
        }
    }

    ThemeAttached.colorAccent: ColorUtils.isDarkColor(Style.colorBackground) ? imageColorsHelper.dominant : imageColorsHelper.dominantComplementary
    ThemeAttached.colorVariant: ColorUtils.isDarkColor(Style.colorBackground) ? imageColorsHelper.dominantComplementary : imageColorsHelper.dominant

    ImageColorsHelper {
        id: imageColorsHelper
        source: helper.image
        backgroundColor: Style.colorBackground
        textColor: Style.colorWhite
        fallbackDominant: "#BC1142"
        fallbackDominantComplementary: "#00B1FF"
    }

    MetadataInfo {
        id: metadataInfo
        anchors.fill: parent
        metaData: root.metaData
        mediaStatus: root.mediaStatus

        view.footerPositioning: ListView.OverlayFooter
        view.footer: RowLayout{
            z: 2
            width: ListView.view.width
            height: 20
            spacing: 0

            Repeater {
                model: imageColorsHelper.colors
                delegate: Rectangle {
                    id: colorRectangle
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    required property color modelData
                    color: modelData
                    BasicLabel {
                        anchors.centerIn: parent
                        relativeBackgroundColor: colorRectangle.color
                        font: Style.textTheme.caption2
                        text: ColorUtils.name(colorRectangle.color, false)
                    }
                }
            }
        }
    }

    layer.enabled: root.elevation
    layer.effect: BasicElevationEffect {
        elevation: root.elevation
    }
}
