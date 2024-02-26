#include "base.h"

#include "application.h"

ServerBase::ServerBase(Application &app) : _app(app), _protocol() {}


Response ServerBase::handle_packet_data(const uint8_t *buffer, uint16_t length) {
    const auto parseResponse = _protocol.parse_packet(buffer, length);
    if (!parseResponse.success) return parseResponse.response;

    const auto [header, data] = parseResponse.packet;

    if (header->type == PacketType::GET_CONFIG) {
        return _protocol.serialize(app().config);
    } else if (header->type >= PacketType::SERVO_X_ENABLED) {
        auto response = _handle_servo_command(header, data);
        if (response.is_ok()) app().update();

        return response;
    } else {
        auto response = _handle_parameter_update(header, data);
        if (response.is_ok()) app().update();

        return response;
    }
}

Response ServerBase::_handle_parameter_update(PacketHeader *header, const void *data) {
    switch (header->type) {
        case PacketType::POWER_ON:
            app().set_power(true);
            return Response::ok();

        case PacketType::POWER_OFF:
            app().set_power(false);
            return Response::ok();

        case PacketType::SPEED:
            return _protocol.update_parameter_value(&app().config.speed, *header, data);

        case PacketType::ACCELERATION:
            return _protocol.update_parameter_value(&app().config.acceleration, *header, data);

        case PacketType::MOVES:
            return _protocol.update_parameter_value(&app().config.moves, *header, data);

        case PacketType::WAITING:
            return _protocol.update_parameter_value(&app().config.waiting, *header, data);

        default:
            return Response::code(ResponseCode::BAD_COMMAND);
    }
}

Response ServerBase::_handle_servo_command(PacketHeader *header, const void *data) {
    ServoConfig *config;
    ServoInstance servo;

    if (header->type >= PacketType::SERVO_Y_ENABLED) {
        config = &app().config.servo_y;
        servo = ServoInstance::Y_AXIS;
    } else {
        config = &app().config.servo_x;
        servo = ServoInstance::X_AXIS;
    }

    switch (header->type) {
        case PacketType::SERVO_X_ENABLED:
        case PacketType::SERVO_Y_ENABLED:
            return _protocol.update_parameter_value(&config->enabled, *header, data);

        case PacketType::SERVO_X_NEUTRAL:
        case PacketType::SERVO_Y_NEUTRAL:
            return _handle_servo_calibrated_parameter(servo, &config->neutral, header, data);

        case PacketType::SERVO_X_MIN:
        case PacketType::SERVO_Y_MIN:
            return _handle_servo_calibrated_parameter(servo, &config->begin, header, data);

        case PacketType::SERVO_X_MAX:
        case PacketType::SERVO_Y_MAX:
            return _handle_servo_calibrated_parameter(servo, &config->end, header, data);

        default:
            return Response::code(ResponseCode::BAD_COMMAND);
    }
}

Response ServerBase::_handle_servo_calibrated_parameter(ServoInstance servo, uint8_t *prop, PacketHeader *header, const void *data) {
    auto response = _protocol.update_parameter_value(prop, *header, data);
    if (response.is_ok()) {
        app().set_calibration_data(servo, *prop);
    }

    return response;
}
