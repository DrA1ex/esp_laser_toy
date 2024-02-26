#pragma once

#include "sys_constants.h"

#define WIFI_MODE                               (WIFI_AP_MODE)
#define WIFI_SSID                               "ESP_TOY"
#define WIFI_PASSWORD                           "12345678"

#define WIFI_CONNECTION_CHECK_INTERVAL          (5000u)                 // Interval (ms) between Wi-Fi connection check
#define WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL    (0u)                    // Max time (ms) to wait for Wi-Fi connection before switch to AP mode
                                                                        // 0 - Newer switch to AP mode

#define MDNS_NAME                               "esp_laser"

#define LASER_PIN                               (13u)

#define SERVO_X_PIN                             (12u)
#define SERVO_Y_PIN                             (14u)

#define CALIBRATION_TIMEOUT                     (5000u)                 // Timeout for calibration mode