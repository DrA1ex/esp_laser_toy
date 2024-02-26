#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>

#include "application.h"
#include "config.h"
#include "debug.h"

#include "misc/storage.h"
#include "misc/smooth_servo.h"
#include "misc/timer.h"

#include "network/web.h"
#include "network/wifi.h"
#include "network/protocol/server/ws.h"

Timer global_timer;

Storage<Config> config_storage(global_timer, "config", STORAGE_CONFIG_VERSION);

SmoothServo servo_x, servo_y;

Application app(config_storage, servo_x, servo_y);

WifiManager wifi_manager;
WebServer web_server(WEB_PORT);

WebSocketServer ws_server(app);

void animation_loop(void *);
void service_loop(void *);

void setup() {
#if defined(DEBUG)
    Serial.begin(115200);
    delay(2000);
#endif

    if (!LittleFS.begin()) {
        D_PRINT("Unable to initialize FS");
    }

    config_storage.begin(&LittleFS);

    servo_x.set_speed(app.config.speed);
    servo_x.set_acceleration(app.config.acceleration);
    servo_x.attach(SERVO_X_PIN);

    servo_y.set_speed(app.config.speed);
    servo_y.set_acceleration(app.config.acceleration);
    servo_y.attach(SERVO_Y_PIN);

    pinMode(LASER_PIN, OUTPUT);
    digitalWrite(LASER_PIN, PIN_DISABLED);

    app.load();

    global_timer.add_interval(animation_loop, 1000 / 60);
    global_timer.add_interval(service_loop, 20);
}

void loop() {
    global_timer.handle_timers();
}

void laser_animation() {
    static uint8_t _move = 0;
    static unsigned long _end_time;

    if (!app.config.power) return;
    if (servo_x.is_moving() || servo_y.is_moving()) return;
    if (millis() - _end_time < app.config.waiting) return;

    if (app.config.servo_x.enabled)
        servo_x.write(random(app.config.servo_x.begin, app.config.servo_x.end));

    if (app.config.servo_y.enabled)
        servo_y.write(random(app.config.servo_y.begin, app.config.servo_y.end));

    if (++_move > app.config.moves) {
        _move = 0;
        _end_time = millis();
    }
}

void animation_loop(void *) {
#if defined(DEBUG) && DEBUG_LEVEL <= __DEBUG_LEVEL_VERBOSE
    static unsigned long t = 0;
    static unsigned long ii = 0;
    if (ii % 10 == 0) D_PRINTF("Animation latency: %lu\n", millis() - t);

    t = millis();
    ++ii;
#endif

    switch (app.state) {
        case AppState::INITIALIZATION: {
            servo_x.write(app.config.servo_x.neutral);
            servo_y.write(app.config.servo_y.neutral);
        }
            break;

        case AppState::STAND_BY:
            laser_animation();
            break;

        case AppState::TURNING_ON:
        case AppState::TURNING_OFF:
            if (!servo_x.is_moving() && !servo_y.is_moving()) {
                app.change_state(AppState::STAND_BY);
            }

            break;

        case AppState::CALIBRATION:
            if (millis() - app.state_change_time > CALIBRATION_TIMEOUT) {
                app.change_state(AppState::STAND_BY);
            }

            break;
    }

    servo_x.tick();
    servo_y.tick();
}

void service_loop(void *) {
#if defined(DEBUG) && DEBUG_LEVEL <= __DEBUG_LEVEL_VERBOSE
    static unsigned long t = 0;
    static unsigned long ii = 0;
    if (ii % 10 == 0) D_PRINTF("Service latency: %lu\n", millis() - t);

    t = millis();
    ++ii;
#endif

    static ServiceState state = ServiceState::UNINITIALIZED;

    switch (state) {
        case ServiceState::UNINITIALIZED:
            wifi_manager.connect(WIFI_MODE, WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL);
            state = ServiceState::WIFI_CONNECT;

            break;

        case ServiceState::WIFI_CONNECT:
            wifi_manager.handle_connection();

            if (wifi_manager.state() == WifiManagerState::CONNECTED) {
                state = ServiceState::INITIALIZATION;
            }

            break;

        case ServiceState::INITIALIZATION:
            ws_server.begin(web_server);

            web_server.on("/debug", HTTP_GET, [](AsyncWebServerRequest *request) {
                char result[64] = {};

                snprintf(result, sizeof(result), "General:\nHeap: %u\nNow: %lu\n",
                         EspClass::getFreeHeap(), millis());

                request->send_P(200, "text/plain", result);
            });

            web_server.begin(&LittleFS);

            ArduinoOTA.setHostname(MDNS_NAME);
            ArduinoOTA.begin();

            app.change_state(AppState::STAND_BY);
            state = ServiceState::STAND_BY;
            break;

        case ServiceState::STAND_BY: {
            ArduinoOTA.handle();
            wifi_manager.handle_connection();

            ws_server.handle_incoming_data();
            break;
        }

        default:;
    }
}