import QtQuick
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.TextArea {
    id: root

    implicitWidth: Math.max(contentWidth + leftPadding + rightPadding,
                            implicitBackgroundWidth + leftInset + rightInset)
    implicitHeight: Math.max(contentHeight + topPadding + bottomPadding,
                             implicitBackgroundHeight + topInset + bottomInset)

    padding: 0
    verticalAlignment: TextInput.AlignTop
    color: ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack

    selectionColor: Style.colorAccent
    selectedTextColor: root.color

    property color relativeBackgroundColor: "black"

    focus: true

    Text {
        id: placeholder
        x: root.leftPadding
        y: root.topPadding
        width: root.width - (root.leftPadding + root.rightPadding)
        height: root.height - (root.topPadding + root.bottomPadding)

        // When the TextArea is in a Flickable, the background is reparented to it
        // so that decorations don't move with the content. We need to do the same.
        // Also allow the background to be set to null; in that case we're just not visible.
        parent: root.background?.parent ?? null

        text: root.placeholderText
        font: root.font
        color: root.placeholderTextColor
        verticalAlignment: root.verticalAlignment
        visible: !root.length && !root.preeditText && (!root.activeFocus || root.horizontalAlignment !== Qt.AlignHCenter)
        elide: Text.ElideRight
        renderType: root.renderType
    }

    background: Item {
        implicitWidth: 120
    }
}
