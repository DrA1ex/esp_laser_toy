/**
 * @enum {Number}
 */
export const PacketType = {
    POWER_ON: 0x01,
    POWER_OFF: 0x02,

    SPEED: 0x03,
    ACCELERATION: 0x04,

    MOVES: 0x05,
    WAITING: 0x06,

    SERVO_X_ENABLED: 0x10,
    SERVO_X_NEUTRAL: 0x11,
    SERVO_X_MIN: 0x12,
    SERVO_X_MAX: 0x13,

    SERVO_Y_ENABLED: 0x20,
    SERVO_Y_NEUTRAL: 0x21,
    SERVO_Y_MIN: 0x22,
    SERVO_Y_MAX: 0x23,

    GET_CONFIG: 0xa0
};