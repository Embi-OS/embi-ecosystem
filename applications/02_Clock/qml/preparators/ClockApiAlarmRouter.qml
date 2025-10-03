import QtQuick
import Eco.Tier3.Happy

HappyFullRouter {
    id: root

    path: "api/alarm"
    socketPath: "ws/alarm"
    tableName: "api_alarm"
    lookupField: "uuid"

    PrimaryHappyField { name: "luid" }
    UuidHappyField    { name: "uuid" }
    VarHappyField     { name: "name";           type: SqlColumnTypes.Char;  precision: 128}
    VarHappyField     { name: "enabled";        type: SqlColumnTypes.Boolean}
    VarHappyField     { name: "hour";           type: SqlColumnTypes.Integer}
    VarHappyField     { name: "minute";         type: SqlColumnTypes.Integer}
    VarHappyField     { name: "date";           type: SqlColumnTypes.Date}
    VarHappyField     { name: "repeat";         type: SqlColumnTypes.Boolean}
    VarHappyField     { name: "weekdays";       type: SqlColumnTypes.Integer}
}
