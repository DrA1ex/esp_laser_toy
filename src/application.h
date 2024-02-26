#pragma once

#include "config.h"
#include "misc/storage.h"
#include "misc/smooth_servo.h"

class Application {
public:
    Storage<Config> &config_storage;

    Config &config;

    unsigned long state_change_time = 0;
    AppState state = AppState::INITIALIZATION;

    SmoothServo &servo_x;
    SmoothServo &servo_y;

    explicit Application(Storage<Config> &config_storage, SmoothServo &servo_x, SmoothServo &servo_y);

    void change_state(AppState s);

    void load();
    void update();

    void set_power(bool on);
    void set_calibration_data(ServoInstance servo, uint8_t value);

    void restart();
};