#include "application.h"

Application::Application(Storage<Config> &config_storage, SmoothServo &servo_x, SmoothServo &servo_y) :
        config_storage(config_storage), config(config_storage.get()),
        servo_x(servo_x), servo_y(servo_y) {}

void Application::load() {
    digitalWrite(LASER_PIN, config.power ? PIN_ENABLED : PIN_DISABLED);

    servo_x.set_speed(config.speed / 2);
    servo_y.set_speed(config.speed / 2);

    servo_x.set_acceleration(config.acceleration / 2);
    servo_y.set_acceleration(config.acceleration / 2);
}

void Application::update() {
    config_storage.save();

    load();
}

void Application::change_state(AppState s) {
    state_change_time = millis();
    state = s;

    D_PRINTF("Change app state: %u\n", (uint8_t) s);
}

void Application::set_power(bool on) {
    config.power = on;

    D_PRINTF("Turning Power: %s\n", config.power ? "ON" : "OFF");

    if (state != AppState::INITIALIZATION) {
        change_state(config.power ? AppState::TURNING_ON : AppState::TURNING_OFF);

        servo_x.write(config.servo_x.neutral);
        servo_y.write(config.servo_y.neutral);
    }

    config_storage.save();
}

void Application::restart() {
    if (config_storage.is_pending_commit()) config_storage.force_save();

    D_PRINT("Restarting");

    ESP.restart();
}

void Application::set_calibration_data(ServoInstance servo, uint8_t value) {
    if (!config.power) return;

    change_state(AppState::CALIBRATION);

    switch (servo) {
        case ServoInstance::X_AXIS:
            servo_x.write(value);
            break;

        case ServoInstance::Y_AXIS:
            servo_y.write(value);
            break;
    }
}
