#pragma once

#define DISABLED                                0
#define ENABLED                                 1

#define WIFI_AP_MODE                            0
#define WIFI_STA_MODE                           1

#define WS_MAX_PACKET_SIZE                      (260u)
#define WS_MAX_PACKET_QUEUE                     (10u)

#define PACKET_SIGNATURE                        ((uint16_t) 0xCADA)

#define WEB_PORT                                (80)

#define STORAGE_PATH                            ("/__storage/")
#define STORAGE_HEADER                          ((uint32_t) 0xfacadaba)
#define STORAGE_CONFIG_VERSION                  ((uint8_t) 1)
#define STORAGE_SAVE_INTERVAL                   (60000u)                // Wait before commit settings to FLASH

#define TIMER_GROW_AMOUNT                       (8u)

#define PIN_DISABLED                            (HIGH)
#define PIN_ENABLED                             (LOW)
