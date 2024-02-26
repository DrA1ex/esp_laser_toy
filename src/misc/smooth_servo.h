#pragma once

#include <Servo.h>

class SmoothServo {
    Servo _servo;

    bool _attached = false;

    uint8_t _speed;
    uint8_t _acceleration;

    uint16_t _min = 0;
    uint16_t _max = 0;

    int16_t _current_speed = 0;

    uint16_t _current = 0;
    uint16_t _target = 0;

public:
    explicit SmoothServo(uint8_t speed = 100, uint8_t acceleration = 32);

    void attach(uint8_t pin, uint16_t min_duty = DEFAULT_MIN_PULSE_WIDTH, uint16_t max_duty = DEFAULT_MAX_PULSE_WIDTH);
    void detach();

    void tick();

    void write(uint8_t angle);

    inline void set_speed(uint8_t speed) { _speed = max<uint8_t>(1, speed); }
    inline void set_acceleration(uint8_t value) { _acceleration = max<uint8_t>(1, value); }

    [[nodiscard]] bool is_moving() const { return _current != _target; }

private:
    void _update_pos(uint16_t us);
};
