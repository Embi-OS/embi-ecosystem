import QtQuick
import Eco.Tier3.Happy

HappyFullRouter {
    id: root

    path: "api/group"
    socketPath: "ws/group"
    tableName: "api_group"
    lookupField: "uuid"

    PrimaryHappyField { name: "luid" }
    UuidHappyField    { name: "uuid" }
    VarHappyField     { name: "name";           type: SqlColumnTypes.Char;  precision: 128}
    VarHappyField     { name: "enabled";        type: SqlColumnTypes.Boolean}
    VarHappyField     { name: "details";        type: SqlColumnTypes.Json}

    RelatedHappyField {
        name: "alarms"
        lookupField: "uuid"
        relatedName: "api_alarm"
        relatedField: "group"
        relatedKey: "uuid"
    }
}
