pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Control {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    topInset: Style.snackbarInset
    bottomInset: Style.snackbarInset
    leftInset: Style.snackbarInset
    rightInset: Style.snackbarInset

    spacing: Style.snackbarSpacing
    padding: Style.snackbarPadding + Style.snackbarInset
    font: Style.textTheme.body1

    hoverEnabled: tapHandler.enabled
    property alias pressed: tapHandler.pressed
    property alias tapCount: tapHandler.tapCount
    property alias timeHeld: tapHandler.timeHeld
    property alias gesturePolicy: tapHandler.gesturePolicy

    signal clicked()
    signal doubleClicked()
    signal longPressed()

    signal closed()
    signal accepted()

    property int severity: SnackbarSeverities.None
    property string title: ""
    property string caption: ""
    property string details: ""
    property string button: ""
    property bool closable: false
    property int timeout: 0
    property int progress: 0

    property bool filled: false
    property bool outlined: false

    readonly property color color: severity===SnackbarSeverities.Info ? Style.colorInfo :
                                   severity===SnackbarSeverities.Warning ? Style.colorWarning :
                                   severity===SnackbarSeverities.Error ? Style.colorError :
                                   severity===SnackbarSeverities.Fatal ? Style.colorFatal :
                                   severity===SnackbarSeverities.Success ? Style.colorSuccess : ColorUtils.blend(Style.colorPrimary, Style.colorWhite, 0.8)

    readonly property string icon: severity===SnackbarSeverities.Info ? MaterialIcons.informationOutline :
                                   severity===SnackbarSeverities.Warning ? MaterialIcons.alertOutline :
                                   severity===SnackbarSeverities.Error ? MaterialIcons.alertCircleOutline :
                                   severity===SnackbarSeverities.Fatal ? MaterialIcons.alertOctagonOutline :
                                   severity===SnackbarSeverities.Success ? MaterialIcons.checkCircleOutline : MaterialIcons.messageOutline

    readonly property color foregroundColor: {
        var color = ColorUtils.isDarkColor(root.backgroundColor) ? Style.colorWhite : Style.colorBlack
        if(filled)
            return color
        if(outlined)
            return root.color
        return ColorUtils.blend(color, root.color, 0.1)
    }

    readonly property color backgroundColor: {
        if(filled)
            return root.color
        if(outlined)
            return Style.colorPrimaryDark
        return ColorUtils.blend(Style.colorBackground, root.color, 0.3)
    }

    contentItem: RowLayout {
        spacing: root.spacing

        SvgColorImage {
            Layout.alignment: Qt.AlignTop
            size: 24
            icon: root.icon
            color: root.filled ? root.foregroundColor : root.color
        }

        ColumnLayout {
            spacing: 0

            RowLayout {
                spacing: root.spacing

                BasicLabel {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter
                    visible: text!=="" || root.button!=="" || root.closable
                    color: root.foregroundColor
                    text: root.title
                    font: Style.textTheme.subtitle1
                    maximumLineCount: 0
                }

                ClickableLabel {
                    Layout.alignment: Qt.AlignTop
                    visible: root.button!==""
                    font: Style.textTheme.capital
                    text: root.button
                    color: root.foregroundColor
                    onClicked: root.accepted()
                }

                ClickableIcon {
                    Layout.alignment: Qt.AlignTop
                    size: 24
                    visible: root.closable
                    icon: MaterialIcons.close
                    color: root.foregroundColor
                    onClicked: root.closed()
                }
            }

            BasicLabel {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: !(text === "")
                color: root.foregroundColor
                text: root.caption
                font: root.font
                elide: Text.ElideRight
                wrapMode: Text.Wrap
                maximumLineCount: 7
            }
        }
    }

    property double elevation: Style.snackbarElevation

    background: Rectangle {
        radius: Style.snackbarRadius
        color: root.backgroundColor

        layer.enabled: root.elevation>0
        layer.effect: BasicElevationEffect {
            elevation: root.elevation
        }

        Rectangle {
            visible: root.progress>0
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            radius: Style.snackbarRadius
            height: parent.height
            width: parent.width * root.progress/10000.0
            color: ColorUtils.blend(root.backgroundColor, root.foregroundColor, 0.1)
        }

        BasicVeil {
            z: 100
            anchors.fill: parent
            radius: 4

            relativeBackgroundColor: root.backgroundColor
            focussed: root.pressed || root.hovered
            pressed: root.pressed
        }

        TapHandler {
            id: tapHandler
            enabled: root.details!==""
            gesturePolicy: TapHandler.ReleaseWithinBounds
            onTapped: root.clicked()
            onDoubleTapped: root.doubleClicked()
            onLongPressed: root.longPressed()
        }
    }
}
