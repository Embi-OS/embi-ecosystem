import QtQuick
import QtQuick.Controls.impl as QQCI
import QtQuick.Templates as T
import Eco.Tier3.Axion

T.Tumbler {
    id: root

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)

    currentIndex: -1
    delegate: LabelTumbler {
        required property string modelData
        text: modelData
    }

    visibleItemCount: 3
    topPadding: 10
    bottomPadding: 10

    property double preferredWidth: 80
    property double preferredHeight: 130

    contentItem: QQCI.TumblerView {
        id: view
        implicitWidth: root.preferredWidth
        implicitHeight: root.preferredHeight
        model: root.model
        delegate: root.delegate
        clip: true

        path: Path {
            startX: view.width / 2
            startY: -view.delegateHeight / 2
            PathLine {
                x: view.width / 2
                y: (root.visibleItemCount + 1) * view.delegateHeight - view.delegateHeight / 2
            }
        }

        readonly property double delegateHeight: root.availableHeight / root.visibleItemCount
    }
}
