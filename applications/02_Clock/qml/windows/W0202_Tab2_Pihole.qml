import QtQuick
import Eco.Tier1.Utils
import Eco.Tier1.Models
import Eco.Tier2.Rest
import Eco.Tier2.Solid
import Eco.Tier3.Axion
import L02_Clock

BasicPane {
    id: root

    property string ip_address: "192.168.1.21"
    property string password: "raspberry"
    property string sid: ""

    readonly property string lastGravityUpdate: DateTimeUtils.formatSecsSinceEpoch(statsMapper?.gravity?.last_update)
    readonly property bool loading: statsMapper.loading || dnsMapper.loading || piholeClient.rootClass.loading

    Component.onCompleted: Qt.callLater(root.authenticate)
    onPasswordChanged: Qt.callLater(root.authenticate)

    SettingsMapper {
        id: piholeSettings
        settingsCategory: "pihole"
        property alias ip_address: root.ip_address
        property alias password: root.password
    }

    property Component formModel: FormObjectModel {
        FormTextFieldDelegate {
            label: qsTr("Adresse IPv4")
            validator: Ipv4Validator {}
            targetProperty: "ip_address"
        }
        FormTextFieldPasswordDelegate {
            label: qsTr("Mot de passe")
            targetProperty: "password"
        }
    }

    RestClient {
        id: piholeClient
        name: "pihole"
        baseUrl: ("https://%1").arg(root.ip_address)
        port: 443
        trailingSlash: false
        globalHeaders: ({"X-FTL-SID": root.sid})
    }

    Timer {
        interval: 1000
        repeat: true
        running: root.sid!==""
        onTriggered: root.reload()
    }

    RestMapper {
        id: statsMapper
        connection: "pihole"
        baseName: "api/stats"
        method: "summary"
        selectWhen: root.sid!==""
        selectPolicy: QVariantMapperPolicies.Delayed
        submitPolicy: QVariantMapperPolicies.Disabled

        property var queries
        property var clients
        property var gravity
        property double took
    }

    RestMapper {
        id: dnsMapper
        connection: "pihole"
        baseName: "api/dns"
        method: "blocking"
        selectWhen: root.sid!==""
        selectPolicy: QVariantMapperPolicies.Delayed
        submitPolicy: QVariantMapperPolicies.Disabled

        property string blocking
        property int timer
        property double took
    }

    RestMapper {
        id: paddMapper
        connection: "pihole"
        baseName: "api"
        method: "padd"
        selectWhen: root.sid!==""
        selectPolicy: QVariantMapperPolicies.Delayed
        submitPolicy: QVariantMapperPolicies.Disabled

        property string recent_blocked
        property int active_clients
        property int gravity_size
        property string top_domain
        property string top_blocked
        property string top_client
        property string blocking
        property var queries
        property var cache
        property var system
        property string node_name
        property string host_model
        property var iface
        property var version
        property var config
        property var sensors
        property double took

        readonly property string localCoreVersion: version?.core?.local?.version??""
        readonly property string remoteCoreVersion: version?.core?.remote?.version??""
        readonly property string localFtlVersion: version?.ftl?.local?.version??""
        readonly property string remoteFtlVersion: version?.ftl?.remote?.version??""
        readonly property string localWebVersion: version?.web?.local?.version??""
        readonly property string remoteWebVersion: version?.web?.remote?.version??""
    }

    function reload() {
        statsMapper.queueSelect()
        dnsMapper.queueSelect()
        paddMapper.queueSelect()
    }

    function authenticate() {
        var reply = piholeClient.rootClass.post("api/auth", ({"password": "raspberry"}))
        reply.succeeded.connect(root.onReplySucceeded)
        reply.failed.connect(root.onReplyFailed)
    }

    function onReplySucceeded(httpStatus: int, reply: var) {
        if(reply?.session?.valid)
            root.sid = reply.session.sid

        statsMapper.select()
        dnsMapper.select()
    }
    function onReplyFailed(httpStatus: int, reply: var) {
        var settings = {
            "caption": ("Http status: %1").arg(httpStatus),
            "details": RestHelper.parseBody(reply, false),
            "diagnose": false
        }
        SnackbarManager.showError(settings)
    }

    function enable() {
        var reply = piholeClient.rootClass.post("api/dns/blocking", ({"blocking": true, "timer": 0}))
        reply.succeeded.connect(root.onReplySucceeded)
        reply.failed.connect(root.onReplyFailed)
    }

    function disable(time: int) {
        if(time>=0) {
            var reply = piholeClient.rootClass.post("api/dns/blocking", ({"blocking": false, "timer": time}))
            reply.succeeded.connect(root.onReplySucceeded)
            reply.failed.connect(root.onReplyFailed)
        }
        else {
            var settings = {
                "title": qsTr("Custom disable timeout"),
                "label": qsTr("Minutes"),
                "textType": FormTextTypes.Number,
                "text": 60,
                "onInputAccepted": function(value) {
                    root.disable(Math.max(0,value*60))
                }
            }
            DialogManager.showInput(settings);
        }
    }

    function versionString(verionLocale: string, versionRemote: string): string {
        const parts1 = verionLocale.replace('v', '').split('.').map(Number);
        const parts2 = versionRemote.replace('v', '').split('.').map(Number);

        var diff = 0
        for (let i = 0; i < Math.max(parts1.length, parts2.length); i++) {
            const num1 = parts1[i] || 0;
            const num2 = parts2[i] || 0;
            if (num1 > num2) diff = 1;
            if (num1 < num2) diff = -1;
        }

        return ("<font color=\"%1\">%2</font>").arg(diff<0 ? Style.colorError : Style.colorSuccess).arg(verionLocale)
    }

//──────────────────────────────────────────────────────────────────────
// Déclaration des composants de la page
//──────────────────────────────────────────────────────────────────────

    padding: 20
    topPadding: 0
    contentItem: PaneGridView {
        rowSpacing: 20
        columnSpacing: 20
        itemWidth: 200
        itemHeight: 100

        model: piholeModel

        header: RowLayout {
            FabButton {
                icon.source: MaterialIcons.play
                visible: dnsMapper.blocking==="disabled"
                highlighted: true
                text: qsTr("Activer")
                onClicked: root.enable()
            }
            FabButton {
                icon.source: MaterialIcons.stop
                visible: dnsMapper.blocking==="enabled"
                highlighted: true
                text: qsTr("Désactiver")
                onClicked: disableMenu.open()

                BasicMenu {
                    id: disableMenu
                    ExtraPosition.position: ItemPositions.BottomEnd

                    BasicMenuItem {text: qsTr("Indéfiniment"); onClicked: root.disable(0); icon.source: MaterialIcons.infinity}
                    BasicMenuSeparator {}
                    BasicMenuItem {text: qsTr("Pour 1h"); onClicked:root.disable(3600); icon.source: MaterialIcons.clockOutline}
                    BasicMenuItem {text: qsTr("Pour 2h"); onClicked: root.disable(7200); icon.source: MaterialIcons.clockOutline}
                    BasicMenuItem {text: qsTr("Pour 4h"); onClicked: root.disable(14400); icon.source: MaterialIcons.clockOutline}
                    BasicMenuSeparator {}
                    BasicMenuItem {text: qsTr("Manuel"); onClicked: root.disable(-1); icon.source: MaterialIcons.accountClockOutline}
                }
            }
            FabButton {
                icon.source: MaterialIcons.sync
                hint: qsTr("Reload")
                onClicked: root.reload()
            }
            BasicBusyIndicator {
                visible: root.loading
                backgroundColor: "transparent"
                size: 40
            }
            LayoutSpring {}
            LabelWithCaption {
                horizontalAlignment: Qt.AlignRight
                textFont: Style.textTheme.title1
                text: "Pi-hole"
                captionFont: Style.textTheme.subtitle2
                caption: ("Core: %1 - FTL: %2 - Web: %3").arg(root.versionString(paddMapper.localCoreVersion, paddMapper.remoteCoreVersion))
                                                         .arg(root.versionString(paddMapper.localFtlVersion, paddMapper.remoteFtlVersion))
                                                         .arg(root.versionString(paddMapper.localWebVersion, paddMapper.remoteWebVersion))
            }
            FabButton {
                icon.source: MaterialIcons.pen
                hint: qsTr("Edit")
                onClicked: {
                    var model = root.formModel.createObject(root) as FormObjectModel
                    var settings = {
                        "formModel": model,
                        "formObject": piholeSettings,
                        "onClosed": function () {
                            model.destroy()
                        },
                        "onFormValidated": function (formValues) {

                        }
                    }
                    DialogManager.showForm(settings)
                }
            }
        }

        itemDelegate: Rectangle {
            id: card
            implicitHeight: 100
            implicitWidth: 200
            radius: 10
            color: card.qtObject?.color ?? Style.colorPrimary

            property QtObject model
            readonly property StandardObject qtObject: model?.qtObject as StandardObject ?? null

            SvgColorImage {
                anchors.top: parent.top
                anchors.topMargin: 0
                anchors.right: parent.right
                anchors.rightMargin: 0
                size: 80
                icon: card.qtObject?.icon ?? MaterialIcons.informationOutline
                color: Style.colorBlack
                opacity: 0.3
            }

            LabelWithCaption {
                anchors.top: parent.top
                anchors.topMargin: 10
                anchors.left: parent.left
                anchors.leftMargin: 10
                spacing: 10

                text: card.qtObject.text
                textFont: Style.textTheme.subtitle2
                caption: card.qtObject.value
                captionFont: Style.textTheme.title1
            }
        }
    }

    StandardObjectModel {
        id: piholeModel

        StandardObject { text: "Total queries";         value: paddMapper.queries?.total;          icon: MaterialIcons.earth;                   color: "#007997" }
        StandardObject { text: "Queries blocked";       value: paddMapper.queries?.blocked;        icon: MaterialIcons.alertOctagonOutline;     color: "#913225" }
        StandardObject { text: "Percentage blocked";    value: FormatUtils.realToString(paddMapper.queries?.percent_blocked,1);        icon: MaterialIcons.chartArc; color: "#B1720C" }
        StandardObject { text: "Domain on Adlists";     value: paddMapper.gravity_size;            icon: MaterialIcons.listBox;          color: "#005C32" }

        StandardObject { text: "Status";                value: dnsMapper.blocking==="disabled"&&dnsMapper.timer!=0 ? DateTimeUtils.formatDuration(dnsMapper.timer*1000) : dnsMapper.blocking; icon: dnsMapper.blocking==="enabled"?MaterialIcons.check:MaterialIcons.close; color: dnsMapper.blocking==="enabled"?"#5CA314":"#BD2C19" }
        StandardObject { text: "Nb. of queries per second";    value: FormatUtils.realToString(statsMapper.queries?.frequency,1); icon: MaterialIcons.sync; color: Style.colorPrimary }
        StandardObject { text: "Nb. of active clients"; value: paddMapper.active_clients;          icon: MaterialIcons.accountOutline;           color: Style.colorPrimary }
        StandardObject { text: "Nb. of FTL clients";    value: statsMapper.clients?.total;          icon: MaterialIcons.domain;                  color: Style.colorPrimary }

        StandardObject { text: "Type UNKNOWN replies";  value: statsMapper.queries?.replies?.UNKNOWN;     icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type NODATA replies";   value: statsMapper.queries?.replies?.NODATA;      icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type NXDOMAIN replies"; value: statsMapper.queries?.replies?.NXDOMAIN;    icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type CNAME replies";    value: statsMapper.queries?.replies?.CNAME;       icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type IP replies";       value: statsMapper.queries?.replies?.IP;          icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type DOMAIN replies";   value: statsMapper.queries?.replies?.DOMAIN;      icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type RRNAME replies";   value: statsMapper.queries?.replies?.RRNAME;      icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type SERVFAIL replies"; value: statsMapper.queries?.replies?.SERVFAIL;    icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type REFUSED replies";  value: statsMapper.queries?.replies?.REFUSED;     icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type NOTIMP replies";   value: statsMapper.queries?.replies?.NOTIMP;      icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type OTHER replies";    value: statsMapper.queries?.replies?.OTHER;       icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type DNSSEC replies";   value: statsMapper.queries?.replies?.DNSSEC;      icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type NONE replies";     value: statsMapper.queries?.replies?.NONE;        icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
        StandardObject { text: "Type BLOB replies";     value: statsMapper.queries?.replies?.BLOB;        icon: MaterialIcons.informationOutline;     color: Style.colorPrimary }
    }
}
