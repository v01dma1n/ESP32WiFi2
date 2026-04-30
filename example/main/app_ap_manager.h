#pragma once

#include "ESP32WiFi2.h"
#include "app_preferences.h"

class AppApManager : public BaseAccessPointManager {
public:
    explicit AppApManager(AppPreferences& prefs)
        : BaseAccessPointManager(prefs) {}

protected:
    void initializeFormFields() override;
};
