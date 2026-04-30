# ESP32WiFi2 — IDF-native WiFi connectivity and NVS-backed preferences
# configurable via a soft-AP captive portal.
#
# Parallel to the Arduino ESP32WiFi library
# (https://github.com/v01dma1n/ESP32WiFi) but targets esp-idf directly.
# Provides: BasePreferences, BaseAccessPointManager, WiFiConnect,
# BootManager, logging, and the POSIX timezone table.

idf_component_register(
    SRCS
        "src/base_preferences.cpp"
        "src/base_access_point_manager.cpp"
        "src/wifi_connector.cpp"
        "src/boot_manager.cpp"
        "src/logging.cpp"
        "src/tz_data.cpp"
    INCLUDE_DIRS
        "include"
    REQUIRES
        nvs_flash
        esp_wifi
        esp_http_server
        esp_netif
        esp_event
        esp_timer
        esp_system
        lwip
)

target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++17)
