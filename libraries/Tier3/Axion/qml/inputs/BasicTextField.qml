import QtQuick
import QtQuick.VirtualKeyboard
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.TextField {
    id: root

    implicitWidth: implicitBackgroundWidth + leftInset + rightInset
                   || Math.max(contentWidth, placeholder.implicitWidth) + leftPadding + rightPadding
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                    Math.max(contentHeight + topPadding + bottomPadding,
                             placeholder.implicitHeight + topPadding + bottomPadding))

    padding: 0
    verticalAlignment: TextInput.AlignVCenter
    color: !acceptableInput ? Style.colorWarning : ColorUtils.isDarkColor(relativeBackgroundColor) ? Style.colorWhite : Style.colorBlack

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

        text: root.placeholderText
        font: root.font
        color: root.placeholderTextColor
        verticalAlignment: root.verticalAlignment
        visible: !root.length && !root.preeditText && (!root.activeFocus || root.horizontalAlignment !== Qt.AlignHCenter)
        elide: Text.ElideRight
        renderType: root.renderType
    }
}
