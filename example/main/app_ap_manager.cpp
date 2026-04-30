#include "app_ap_manager.h"

void AppApManager::initializeFormFields() {
    BaseAccessPointManager::initializeFormFields();

    auto& cfg = static_cast<AppPreferences&>(_prefs).config;

    _formFields.push_back(FormField{
        "device_name", "Device Name", false, VALIDATION_NONE,
        PREF_STRING, { .str_pref = cfg.deviceName },
        nullptr, 0,
    });
}
