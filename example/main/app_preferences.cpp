#include "app_preferences.h"

#include <cstring>

AppPreferences::AppPreferences()
    : BasePreferences(config) {
    std::memset(&config, 0, sizeof(config));
    std::strncpy(config.deviceName, "my-device", sizeof(config.deviceName) - 1);
}

void AppPreferences::getPreferences() {
    BasePreferences::getPreferences();
    readString("device_name", config.deviceName, sizeof(config.deviceName));
    if (config.deviceName[0] == '\0')
        std::strncpy(config.deviceName, "my-device", sizeof(config.deviceName) - 1);
}

void AppPreferences::putPreferences() {
    BasePreferences::putPreferences();
    writeString("device_name", config.deviceName);
}

void AppPreferences::dumpPreferences() {
    BasePreferences::dumpPreferences();
    LOGINF("device_name: %s", config.deviceName);
}
