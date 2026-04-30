// ESP32WiFi2.h — umbrella header for the ESP32WiFi2 component.
//
// IDF-native WiFi connectivity and NVS-backed preferences configurable
// via a soft-AP captive portal. Parallel to the Arduino ESP32WiFi library
// (https://github.com/v01dma1n/ESP32WiFi) but targets esp-idf directly.
//
// Apps just:
//   #include "ESP32WiFi2.h"

#pragma once

#include "wifi_types.h"
#include "logging.h"

#include "base_preferences.h"
#include "base_access_point_manager.h"
#include "wifi_connector.h"
#include "boot_manager.h"
#include "tz_data.h"
