import QtQuick
import Eco.Tier3.Axion

ColumnLayout {
    id: root

    RowLayout {
        Layout.fillWidth: true
        BasicLabel {
            Layout.alignment: Qt.AlignVCenter
            text: "Spacing"
        }
        BasicSlider {
            id: slider
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true

            from: 0
            to: 16
            stepSize: 1
        }
    }

    RowLayout {
        Layout.fillWidth: true

        LabelWithCaption {
            Layout.preferredWidth: 100
            spacing: slider.value

            text: "Title"
            caption: "Caption"
            captionMaximumLineCount: 1
            captionWrapMode: Text.Wrap

            DebugRectangle {}
        }

        LabelWithCaption {
            Layout.preferredWidth: 100
            spacing: slider.value

            text: "Long title that should be elided"
            caption: "Caption that should be elided"
            captionMaximumLineCount: 1
            captionWrapMode: Text.Wrap

            DebugRectangle {}
        }

        LabelWithCaption {
            Layout.preferredWidth: 100
            spacing: slider.value

            text: "Long title that should be elided"
            caption: "Caption with 2 line too long so elided"
            captionMaximumLineCount: 2
            captionWrapMode: Text.Wrap

            DebugRectangle {}
        }
    }

    LabelWithCaption {
        spacing: slider.value
        Layout.fillWidth: true

        text: "Text only"
        captionMaximumLineCount: 1
        captionWrapMode: Text.Wrap

        DebugRectangle {}
    }

    LabelWithCaption {
        spacing: slider.value
        Layout.fillWidth: true

        caption: "Caption only"
        captionMaximumLineCount: 1
        captionWrapMode: Text.Wrap

        DebugRectangle {}
    }

    GridLayout {
        Layout.fillWidth: true
        columns: 3

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignLeft"
            caption: "AlignLeft"
            horizontalAlignment: Text.AlignLeft

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignLeft"
            horizontalAlignment: Text.AlignLeft

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            caption: "AlignLeft"
            horizontalAlignment: Text.AlignLeft

            DebugRectangle {}
        }
    // }

    // RowLayout {
    //     Layout.fillWidth: true

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignHCenter"
            caption: "AlignHCenter"
            horizontalAlignment: Text.AlignHCenter

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignHCenter"
            horizontalAlignment: Text.AlignHCenter

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            caption: "AlignHCenter"
            horizontalAlignment: Text.AlignHCenter

            DebugRectangle {}
        }
    // }

    // RowLayout {
    //     Layout.fillWidth: true

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignRight"
            caption: "AlignRight"
            horizontalAlignment: Text.AlignRight

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignRight"
            horizontalAlignment: Text.AlignRight

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            caption: "AlignRight"
            horizontalAlignment: Text.AlignRight

            DebugRectangle {}
        }
    // }

    // RowLayout {
    //     Layout.fillWidth: true

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignTop"
            caption: "AlignTop"
            verticalAlignment: Text.AlignTop

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignTop"
            verticalAlignment: Text.AlignTop

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            caption: "AlignTop"
            verticalAlignment: Text.AlignTop

            DebugRectangle {}
        }
    // }

    // RowLayout {
    //     Layout.fillWidth: true

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignVCenter"
            caption: "AlignVCenter"
            verticalAlignment: Text.AlignVCenter

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignVCenter"
            verticalAlignment: Text.AlignVCenter

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            caption: "AlignVCenter"
            verticalAlignment: Text.AlignVCenter

            DebugRectangle {}
        }
    // }

    // RowLayout {
    //     Layout.fillWidth: true

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignBottom"
            caption: "AlignBottom"
            verticalAlignment: Text.AlignBottom

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            text: "AlignBottom"
            verticalAlignment: Text.AlignBottom

            DebugRectangle {}
        }

        LabelWithCaption {
            spacing: slider.value
            implicitWidth: 150
            Layout.preferredHeight: 56

            caption: "AlignBottom"
            verticalAlignment: Text.AlignBottom

            DebugRectangle {}
        }
    }
}
