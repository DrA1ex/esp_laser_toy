import {PacketType} from "./network/cmd.js";
import {BinaryParser} from "./misc/binary_parser.js";
import {EventEmitter} from "./misc/event_emitter.js";
import {Properties} from "./props.js";


export class Config extends EventEmitter {
    static LOADED = "config_loaded";
    static PROPERTY_CHANGED = "config_prop_changed";

    #ws;

    power;
    speed;
    acceleration;
    moves;
    waiting;

    servo_x;
    servo_y;

    constructor(ws) {
        super();

        this.#ws = ws;
    }

    async load() {
        const buffer = await this.#ws.request(PacketType.GET_CONFIG);
        const parser = new BinaryParser(buffer);

        this.power = parser.readBoolean();
        this.speed = parser.readUInt8();
        this.acceleration = parser.readUInt8();
        this.moves = parser.readUInt8();
        this.waiting = parser.readUInt16();

        this.servo_x = this.#loadServoConfig(parser);
        this.servo_y = this.#loadServoConfig(parser);

        this.emitEvent(Config.LOADED);
    }

    #loadServoConfig(parser) {
        return {
            enabled: parser.readBoolean(),
            neutral: parser.readUInt8(),
            begin: parser.readUInt8(),
            end: parser.readUInt8(),
        }
    }

    getProperty(key) {
        const prop = Properties[key];
        if (!prop) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const value = prop.key.split(".").reduce((obj, key) => obj[key], this);
        return (prop.transform ? prop.transform(value) : value) ?? prop.default;
    }

    setProperty(key, value) {
        if (!Properties[key]) {
            console.error(`Unknown property ${key}`);
            return;
        }

        const oldValue = this.getProperty(key);

        this.#setProperty(key, value);
        this.emitEvent(Config.PROPERTY_CHANGED, {key, value, oldValue});
    }

    #setProperty(key, value) {
        let target = this;
        const parts = key.split(".");
        for (let i = 0; i < parts.length - 1; i++) {
            target = target[parts[i]];
        }

        target[parts.at(-1)] = value;
    }
}