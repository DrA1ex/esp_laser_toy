#include "smooth_servo.h"

#include "debug.h"

SmoothServo::SmoothServo(uint8_t speed, uint8_t acceleration)
        : _speed(speed), _acceleration(acceleration) {}

void SmoothServo::attach(uint8_t pin, uint16_t min_duty, uint16_t max_duty) {
    _min = min_duty;
    _max = max_duty;

    _servo.attach(pin, _min, _max);
    _attached = true;

    _current = _servo.readMicroseconds();
}

void SmoothServo::detach() {
    _servo.detach();
    _attached = false;
}

void SmoothServo::write(uint8_t angle) {
    if (!_attached) return;

    _target = _min + (_max - _min) * min<int32_t>(180, angle) / 180;
}

void SmoothServo::tick() {
    if (!_attached || _current == _target) {
        _current_speed = 0;
        return;
    }

    auto delta = (int16_t) _target - _current;
    auto impulse = delta > 0 ? _acceleration : -_acceleration;
    _current_speed += (int16_t) impulse;

    if (_current_speed > _speed) _current_speed = _speed;
    else if (_current_speed < -(int16_t) _speed) _current_speed = -(int16_t) _speed;

    auto next_position = _current + _current_speed;
    if (abs(delta) > abs(_current_speed)) {
        _update_pos(next_position);
    } else {
        _update_pos(_target);
    }

    VERBOSE(D_PRINTF("Impulse: %i, Speed: %i, Next: %i, Delta: %i\n", impulse, _current_speed, next_position, delta));
}

void SmoothServo::_update_pos(uint16_t us) {
    _servo.writeMicroseconds(us);
    _current = us;
}
