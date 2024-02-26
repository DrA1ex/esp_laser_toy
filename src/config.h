#pragma once

#include <cctype>

enum class AppState {
    INITIALIZATION,
    STAND_BY,
    TURNING_ON,
    TURNING_OFF,
    CALIBRATION,
};

enum class ServiceState {
    UNINITIALIZED,
    WIFI_CONNECT,
    INITIALIZATION,
    STAND_BY
};

enum class ServoInstance {
    X_AXIS,
    Y_AXIS
};

struct __attribute ((packed)) ServoConfig {
    bool enabled = true;

    uint8_t neutral = 90;
    uint8_t begin = 0;
    uint8_t end = 180;
};

struct __attribute ((packed)) Config {
    bool power = true;

    uint8_t speed = 100;
    uint8_t acceleration = 32;

    uint8_t moves = 5;
    uint16_t waiting = 3000;

    ServoConfig servo_x{};
    ServoConfig servo_y{};
};
