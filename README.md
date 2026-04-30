# ESP32WiFi2

IDF-native WiFi connectivity and NVS-backed preferences configurable via
a soft-AP captive portal. IDF port of
[ESP32WiFi](https://github.com/v01dma1n/ESP32WiFi) (Arduino), extracted
from [ESP32NTPClock](https://github.com/v01dma1n/ESP32NTPClock2) to be
reusable as a standalone component.

---

## What it does

On first boot (no credentials) or after a double-reset, the device comes
up as an open access point. Any client that joins gets a captive portal
redirect to `http://192.168.4.1/`, where a minimal dark-theme form lets
the user enter WiFi credentials, timezone, and any app-specific settings.
After save the device reboots, connects to the network, and stays there.

Every DNS query on the AP network is answered with the AP's own IP so
iOS, Android, macOS, and Windows all pop the captive-portal sheet
automatically — no manual navigation required.

---

## Comparison with the Arduino version

| Arduino ESP32WiFi                  | ESP32WiFi2 (this)                              |
|------------------------------------|------------------------------------------------|
| `ESPAsyncWebServer` + `AsyncTCP`   | `esp_http_server` (httpd), built into IDF      |
| Arduino `Preferences`              | `nvs_flash` / `nvs_handle_t` directly          |
| `runBlocking(std::function<> cb)`  | `runBlockingLoop()` — display is caller's job  |
| `TimeManager::begin(tz)`           | not included — lives in ESP32NTPClock          |
| Arduino `String`                   | `std::string`, `const char*`                   |

---

## Component layout

```
ESP32WiFi2/
├── CMakeLists.txt
├── include/
│   ├── ESP32WiFi2.h                  # umbrella — include this one
│   ├── wifi_types.h                  # FormField, AppLogLevel, PrefType, …
│   ├── base_preferences.h            # NVS-backed config base class
│   ├── base_access_point_manager.h   # soft-AP + captive portal
│   ├── wifi_connector.h              # blocking STA connect helper
│   ├── boot_manager.h                # double-reset detection
│   ├── logging.h                     # LOG* macros gated on AppLogLevel
│   └── tz_data.h                     # POSIX timezone dropdown table
└── src/
    ├── base_preferences.cpp
    ├── base_access_point_manager.cpp
    ├── wifi_connector.cpp
    ├── boot_manager.cpp
    ├── logging.cpp
    └── tz_data.cpp
```

---

## Using in an IDF project

Add as a git submodule:

```bash
git submodule add https://github.com/v01dma1n/ESP32WiFi2 components/esp32_wifi
git submodule update --init
```

Declare the dependency in the component that uses it:

```cmake
idf_component_register(
    ...
    REQUIRES esp32_wifi
)
```

Include the umbrella header:

```cpp
#include "ESP32WiFi2.h"
```

---

## Extending — the standard pattern

The library is designed to be subclassed, not forked. The same extension
pattern used by the Arduino version works here unchanged.

### 1. Add config fields

```cpp
// my_preferences.h
#include "ESP32WiFi2.h"

struct MyConfig : public BaseConfig {
    char apiKey[MAX_PREF_STRING_LEN];
    bool featureEnabled;
};

class MyPreferences : public BasePreferences {
public:
    MyPreferences() : BasePreferences(config) {}

    void getPreferences() override {
        BasePreferences::getPreferences();   // loads SSID, password, TZ, logLevel
        if (!openNvs(false)) return;
        readString("api_key",  config.apiKey,        sizeof(config.apiKey));
        readBool  ("feat_on",  false);
        // … store result into config fields …
        closeNvs();
    }

    void putPreferences() override {
        BasePreferences::putPreferences();
        if (!openNvs(true)) return;
        writeString("api_key", config.apiKey);
        writeBool  ("feat_on", config.featureEnabled);
        closeNvs();
    }

    MyConfig config;
};
```

### 2. Add portal rows

```cpp
// my_ap_manager.h
#include "ESP32WiFi2.h"
#include "my_preferences.h"

class MyApManager : public BaseAccessPointManager {
public:
    explicit MyApManager(MyPreferences& prefs)
        : BaseAccessPointManager(prefs) {}

protected:
    void initializeFormFields() override {
        BaseAccessPointManager::initializeFormFields();  // WiFi/TZ/logLevel first

        auto& cfg = static_cast<MyPreferences&>(_prefs).config;

        _formFields.push_back(FormField{
            "api_key", "API Key", true, VALIDATION_NONE,
            PREF_STRING, { .str_pref = cfg.apiKey }, nullptr, 0,
        });
        _formFields.push_back(FormField{
            "feat_on", "Enable feature", false, VALIDATION_NONE,
            PREF_BOOL, { .bool_pref = &cfg.featureEnabled }, nullptr, 0,
        });
    }
};
```

### 3. Wire into app_main

```cpp
#include "ESP32WiFi2.h"
#include "my_preferences.h"
#include "my_ap_manager.h"

MyPreferences prefs;
MyApManager   apManager(prefs);

extern "C" void app_main() {
    prefs.setup();
    prefs.getPreferences();

    if (!WiFiConnect("my-device",
                     prefs.config.ssid,
                     prefs.config.password,
                     /*attempts=*/3)) {
        apManager.setup("my-device");
        apManager.runBlockingLoop();   // blocks until user saves + device reboots
    }

    // WiFi is up; proceed with application logic.
}
```

---

## Double-reset AP trigger

`BootManager` detects two resets within ~5 seconds and returns `true`
from `checkForForceAPMode()`. Call it early in setup, before the FSM
starts, so a stuck device can always reach the portal without a flash
erase.

```cpp
BootManager bootMgr;
if (bootMgr.checkForForceAPMode()) {
    apManager.setup("my-device");
    apManager.runBlockingLoop();
}
// Later, once running normally:
bootMgr.markBootStable();
```

---

## API summary

### `wifi_connector.h`

| Function | Description |
|---|---|
| `WiFiInit()` | Initialize netif, event loop, WiFi. Idempotent. |
| `WiFiConnect(host, ssid, pass, attempts)` | Blocking STA connect, ~10 s per attempt. Returns true on success. |
| `WiFiStop()` | Disconnect and stop WiFi. |

### `BasePreferences`

| Method | Description |
|---|---|
| `setup()` | One-time NVS init (call before getPreferences). |
| `getPreferences()` | Load config from NVS. |
| `putPreferences()` | Save config to NVS and commit. |
| `dumpPreferences()` | Log current values at DEBUG level. |

Protected helpers for subclasses: `readString`, `writeString`, `readBool`,
`writeBool`, `readInt`, `writeInt`, `openNvs`, `closeNvs`.

### `BaseAccessPointManager`

| Method | Description |
|---|---|
| `setup(hostName)` | Start soft-AP, httpd, DNS responder. |
| `runBlockingLoop()` | Spin until user saves config and device reboots. |
| `shutdown()` | Tear down AP, httpd, DNS task. |
| `isClientConnected()` | True once any client has loaded the form. |
| `initializeFormFields()` | Override to add rows; call base first. |

### `BootManager`

| Method | Description |
|---|---|
| `checkForForceAPMode()` | Returns true if a double reset was detected. |
| `markBootStable()` | Clear the flag after running normally for a few seconds. |

### `logging.h`

Runtime-gated macros: `LOGINF(fmt, ...)`, `LOGDBG(fmt, ...)`,
`LOGERR(fmt, ...)`. Gate controlled by `AppLogLevel g_appLogLevel`
(default `APP_LOG_INFO`). The captive portal's log-level dropdown updates
this at runtime without reflashing.

---

## IDF component dependencies

```
esp32_wifi REQUIRES:
    nvs_flash       — BasePreferences, BootManager
    esp_wifi        — WiFiConnect, BaseAccessPointManager (soft-AP)
    esp_http_server — BaseAccessPointManager (httpd)
    esp_netif       — WiFiInit, BaseAccessPointManager
    esp_event       — WiFiInit
    esp_timer       — reboot-on-save timer
    esp_system      — esp_restart()
    lwip            — DNS task (socket API)
```

`esp_http_client` is intentionally absent — HTTP fetching (weather,
geo-TZ lookup) belongs at the application layer.

---

## License

MIT (matches the upstream Arduino project).
