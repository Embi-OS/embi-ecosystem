import QtQuick
import Eco.Tier3.Happy
import L02_Clock

HappyServer {
    id: root

    name: "Clock local API"
    port: 32768
    external: true

    HappyRootRouter {
        path: "api"
        socketPath: "ws"
    }

    ClockApiAlarmRouter {}
}
