import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Media

Item {
    id: root

    required property MediaPlayer mediaPlayer
    readonly property mediaMetaData metaData: mediaPlayer.metaData
    readonly property int mediaStatus: mediaPlayer.mediaStatus

    readonly property string title: metaData.stringValue(MediaMetaData.Title)
    property alias titleFont: titleLabel.font
    property alias titleColor: titleLabel.color

    readonly property string album: metaData.stringValue(MediaMetaData.AlbumTitle)
    property alias albumFont: albumLabel.font
    property alias albumColor: albumLabel.color

    readonly property string artist: metaData.stringValue(MediaMetaData.AlbumArtist) || metaData.stringValue(MediaMetaData.ContributingArtist)
    property alias artistFont: artistLabel.font
    property alias artistColor: artistLabel.color

    readonly property double labelsImplicitWidth: Math.max(titleLabel.implicitWidth, Math.max(albumLabel.implicitWidth, artistLabel.implicitWidth))
    readonly property double labelsImplicitHeight: titleLabel.implicitHeight + albumLabel.implicitHeight + artistLabel.implicitHeight

    implicitWidth: labelsImplicitWidth+(root.leftSpacing+root.rightSpacing)
    implicitHeight: labelsImplicitHeight+(root.topSpacing+root.bottomSpacing)+2*spacing

    property int horizontalAlignment: Qt.AlignLeft
    property int verticalAlignment: Qt.AlignVCenter
    property int elide: Text.ElideRight
    property int wrapMode: Text.NoWrap

    property double topSpacing: 0
    property double leftSpacing: 0
    property double rightSpacing: 0
    property double bottomSpacing: 0
    property double spacing: 0

    AnimatedLabel {
        id: titleLabel

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.topMargin: root.topSpacing
        anchors.leftMargin: root.leftSpacing
        anchors.rightMargin: root.rightSpacing

        Binding on text {
            when: root.mediaStatus!==MediaPlayer.LoadingMedia
            delayed: true
            value: root.title || "No song title available"
        }

        horizontalAlignment: root.horizontalAlignment
        font: Style.textTheme.title1
    }

    AnimatedLabel {
        id: albumLabel

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: titleLabel.bottom
        anchors.topMargin: root.spacing
        anchors.leftMargin: root.leftSpacing
        anchors.rightMargin: root.rightSpacing

        Binding on text {
            when: root.mediaStatus!==MediaPlayer.LoadingMedia
            delayed: true
            value: root.album || "No album title available"
        }

        highlighted: true
        horizontalAlignment: root.horizontalAlignment
        font: Style.textTheme.title2
    }

    AnimatedLabel {
        id: artistLabel

        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: albumLabel.bottom
        anchors.topMargin: root.spacing
        anchors.leftMargin: root.leftSpacing
        anchors.rightMargin: root.rightSpacing

        Binding on text {
            when: root.mediaStatus!==MediaPlayer.LoadingMedia
            delayed: true
            value: root.artist || "No artist available"
        }

        highlighted: true
        horizontalAlignment: root.horizontalAlignment
        font: Style.textTheme.title2
    }
}
