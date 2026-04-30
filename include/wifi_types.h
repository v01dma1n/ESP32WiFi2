// wifi_types.h — shared enums and structs for WiFi connectivity and preferences.
//
// Extracted from ESP32NTPClock's enc_types.h so this layer can be used
// independently of the clock/display stack. enc_types.h re-includes this
// file for backward compatibility with existing clock apps.

#pragma once

#include <cstdint>
#include <cstddef>

// Upper bound for any single string preference stored in NVS.
static constexpr size_t MAX_PREF_STRING_LEN = 64;

// Application-level log verbosity. The LOG* macros in logging.h consult
// a global g_appLogLevel and suppress lower-severity messages.
enum AppLogLevel {
    APP_LOG_ERROR,
    APP_LOG_INFO,
    APP_LOG_DEBUG
};

// Which kind of form control to render for a given preference.
enum PrefType {
    PREF_NONE,
    PREF_STRING,
    PREF_BOOL,
    PREF_INT,
    PREF_ENUM,
    PREF_SELECT   // dropdown backed by PrefSelectOption[]
};

enum FieldValidation {
    VALIDATION_NONE,
    VALIDATION_IP_ADDRESS,
    VALIDATION_INTEGER,
};

// Option entry for dropdown-style preference fields in the captive portal.
struct PrefSelectOption {
    const char* name;   // user-facing label
    const char* value;  // value persisted to NVS
};

// Description of one preference the captive portal should render.
// The AP manager walks an array of these, renders HTML, and writes back
// through the `pref` union.
struct FormField {
    const char* id;          // HTML name="" — must match NVS key slug
    const char* name;        // human-readable label
    bool isMasked;           // render as type="password"
    FieldValidation validation;
    PrefType prefType;
    union {
        char*    str_pref;   // PREF_STRING / PREF_SELECT
        bool*    bool_pref;  // PREF_BOOL
        int32_t* int_pref;   // PREF_INT
    } pref;
    const PrefSelectOption* select_options;  // only for PREF_SELECT
    int num_select_options;
};
