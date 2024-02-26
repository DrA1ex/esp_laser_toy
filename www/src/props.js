import {PacketType} from "./network/cmd.js";

export const PropertyConfig = [{
    key: "general", section: "General", props: [
        {key: "power", title: "Power", type: "trigger", cmd: [PacketType.POWER_ON, PacketType.POWER_OFF]},
        {key: "speed", title: "Speed", type: "wheel", limit: 255, cmd: PacketType.SPEED},
        {key: "acceleration", title: "Acceleration", type: "wheel", limit: 255, cmd: PacketType.ACCELERATION},
        {key: "moves", title: "Moves", type: "wheel", limit: 255, cmd: PacketType.MOVES},
        {key: "waiting", title: "Wait, ms", type: "wheel", kind: "Uint16", size: 2, limit: 60000, cmd: PacketType.WAITING},
    ],
}, {
    key: "servo_x", section: "Horizontal", props: [
        {key: "servo_x.enabled", title: "Enabled", type: "trigger", cmd: PacketType.SERVO_X_ENABLED},
        {key: "servo_x.neutral", title: "Neutral position", type: "wheel", limit: 180, cmd: PacketType.SERVO_X_NEUTRAL},
        {key: "servo_x.begin", title: "Start position", type: "wheel", limit: 180, cmd: PacketType.SERVO_X_MIN},
        {key: "servo_x.end", title: "End position", type: "wheel", limit: 180, cmd: PacketType.SERVO_X_MAX},
    ]
}, {
    key: "servo_y", section: "Vertical", props: [
        {key: "servo_y.enabled", title: "Enabled", type: "trigger", cmd: PacketType.SERVO_Y_ENABLED},
        {key: "servo_y.neutral", title: "Neutral position", type: "wheel", limit: 180, cmd: PacketType.SERVO_Y_NEUTRAL},
        {key: "servo_y.begin", title: "Start position", type: "wheel", limit: 180, cmd: PacketType.SERVO_Y_MIN},
        {key: "servo_y.end", title: "End position", type: "wheel", limit: 180, cmd: PacketType.SERVO_Y_MAX},
    ]
}];

export const Properties = PropertyConfig.reduce((res, section) => {
    for (const prop of section.props) {
        if (res[prop.key]) {
            console.warn(`Key ${prop.key} already exist`);
            continue;
        }

        res[prop.key] = prop;
    }

    return res;
}, {});