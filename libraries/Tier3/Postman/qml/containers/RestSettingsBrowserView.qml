import QtQuick
import Eco.Tier3.Axion
import Eco.Tier3.Postman

BasicPane {
    id: root

    property bool editable: true

    RestBrowser {
        id: browser

        verb: verbComboBox.currentValue
        path: pathTextField.text

        client: RestHelper.defaultApiClient()
    }

    contentItem: PaneTextView {
        drawFrame: true
        color: borderColor
        borderColor: Style.colorPrimary
        contentColor: Style.colorPrimaryDark
        text: browser.replyJson
        textFormat: TextEdit.AutoText
    }

    header: RowContainer {
        topInset: root.headerPadding
        leftInset: root.headerPadding
        rightInset: root.headerPadding
        spacing: root.spacing
        drawFrame: true

        FormComboBox {
            id: verbComboBox
            Layout.preferredWidth: 150
            borderColorDefault: Style.colorPrimaryLight
            placeholder: "Verb"
            options: ListModel {
                ListElement { display: "GET" }
                ListElement { display: "POST" }
                ListElement { display: "PUT" }
                ListElement { display: "DELETE" }
                ListElement { display: "PATCH" }
                ListElement { display: "HEAD" }
                ListElement { display: "OPTIONS" }
            }
            textRole: "display"
            valueRole: "display"
        }
        FormTextField {
            id: pathTextField
            Layout.fillWidth: true
            borderColorDefault: Style.colorPrimaryLight
            placeholder: "API Path"
            text: "api"
            // onAccepted: browser.execute()
        }
        BasicSeparator {}
        RawButton {
            color: Style.colorPrimaryLight
            icon.source: MaterialIcons.restore
            text: "Exec"
            onClicked: browser.execute()
        }
    }

    // footer: RowContainer {
    //     bottomInset: root.footerPadding
    //     leftInset: root.footerPadding
    //     rightInset: root.footerPadding
    //     spacing: root.spacing
    //     alignment: Qt.AlignVCenter
    //     drawFrame: true

    //     ColumnLayout {
    //         Layout.fillHeight: true
    //         spacing: 30
    //         BasicLabel {
    //             Layout.fillHeight: true
    //             wrapMode: Text.Wrap
    //             font: Style.textTheme.body1
    //             text: ("Reply url: %1").arg(browser.replyUrl)
    //         }
    //         BasicProgressBar {
    //             from: 0
    //             to: 10000
    //             value: browser.upload
    //             font: Style.textTheme.body1
    //             labelPosition: ItemPositions.TopStart
    //             text: ("Upload: %1").arg(defaultLabel)
    //         }
    //         BasicProgressBar {
    //             from: 0
    //             to: 10000
    //             value: browser.download
    //             font: Style.textTheme.body1
    //             labelPosition: ItemPositions.TopStart
    //             text: ("Download: %1").arg(defaultLabel)
    //         }
    //         BasicProgressBar {
    //             from: 0
    //             to: 10000
    //             value: browser.parsing
    //             font: Style.textTheme.body1
    //             labelPosition: ItemPositions.TopStart
    //             text: ("Parsing: %1").arg(defaultLabel)
    //         }
    //     }
    //     LayoutSpring {}
    //     BasicSeparator { color: Style.colorPrimaryLight }
    //     BasicLabel {
    //         Layout.fillHeight: true
    //         Layout.fillWidth: true
    //         wrapMode: Text.Wrap
    //         elide: Text.ElideRight
    //         font: Style.textTheme.body1
    //         text: ("Net. code: %1").arg(browser.networkCode)+"\n"+
    //               ("Net. error: %1").arg(browser.networkError)+"\n"+
    //               ("Net. message: %1").arg(browser.networkMessage)+"\n"+
    //               ("Net. debug: %1").arg(browser.networkDebug)
    //     }
    // }

    footer: ColumnContainer {
        bottomInset: root.footerPadding
        leftInset: root.footerPadding
        rightInset: root.footerPadding
        spacing: root.spacing
        alignment: Qt.AlignVCenter
        drawFrame: true

        BasicLabel {
            Layout.fillWidth: true
            // wrapMode: Text.Wrap
            font: Style.textTheme.body1
            text: ("Reply url: %1").arg(browser.replyUrl)
        }

        RowLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            ColumnLayout {
                Layout.fillHeight: true
                Layout.maximumWidth: 200
                spacing: 30
                LayoutSpring {}
                BasicProgressBar {
                    Layout.fillWidth: true
                    from: 0
                    to: 10000
                    value: browser.upload
                    font: Style.textTheme.body1
                    labelPosition: ItemPositions.TopStart
                    text: ("Upload: %1").arg(defaultLabel)
                }
                BasicProgressBar {
                    Layout.fillWidth: true
                    from: 0
                    to: 10000
                    value: browser.download
                    font: Style.textTheme.body1
                    labelPosition: ItemPositions.TopStart
                    text: ("Download: %1").arg(defaultLabel)
                }
                BasicProgressBar {
                    Layout.fillWidth: true
                    from: 0
                    to: 10000
                    value: browser.parsing
                    font: Style.textTheme.body1
                    labelPosition: ItemPositions.TopStart
                    text: ("Parsing: %1").arg(defaultLabel)
                }
            }
            BasicSeparator { color: Style.colorPrimaryLight }
            BasicLabel {
                Layout.fillHeight: true
                Layout.fillWidth: true
                wrapMode: Text.Wrap
                elide: Text.ElideRight
                font: Style.textTheme.body1
                text: ("Net. code: %1").arg(browser.networkCode)+"\n"+
                      ("Net. error: %1").arg(browser.networkError)+"\n"+
                      ("Net. message: %1").arg(browser.networkMessage)+"\n"+
                      ("Net. debug: %1").arg(browser.networkDebug)
            }
        }
    }
}
