#pragma once

#include "ESP32WiFi2.h"

// Extend BaseConfig with one app-specific field.
// Add more fields here as the app grows; serialize them in app_preferences.cpp.
struct AppConfig : public BaseConfig {
    char deviceName[MAX_PREF_STRING_LEN];
};

class AppPreferences : public BasePreferences {
public:
    AppPreferences();

    void getPreferences() override;
    void putPreferences() override;
    void dumpPreferences() override;

    AppConfig config;
};
