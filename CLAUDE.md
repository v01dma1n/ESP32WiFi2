# ESP32WiFi2 — agent instructions

## Project
IDF-native WiFi connectivity and NVS-backed preferences configurable via
a soft-AP captive portal. Extracted from ESP32NTPClock; parallel to the
Arduino ESP32WiFi library (https://github.com/v01dma1n/ESP32WiFi).

This is a component library, not an application — there is no `main.cpp`
and no standalone `idf.py build` target. Build and test it inside a project
that has an IDF build system (e.g. MoodWhisperer at
https://github.com/v01dma1n/MoodWhisperer).

## Using in an IDF project

Add as a git submodule under `components/esp32_wifi`, then declare
`REQUIRES esp32_wifi` in the dependent component's CMakeLists.txt.
Include the umbrella header:

    #include "ESP32WiFi2.h"

The dependent component's CMakeLists.txt also needs `esp_wifi` and
`nvs_flash` in its own REQUIRES if it calls WiFi or NVS APIs directly;
otherwise the transitive REQUIRES from esp32_wifi are sufficient for
include paths.

## Conventions

- Class names and file layout mirror the Arduino ESP32WiFi library so
  porting between the two feels familiar. Do not rename the base classes
  (BaseConfig, BasePreferences, BaseAccessPointManager, BootManager).

- Extension pattern — apps subclass, not modify:

      struct AppConfig : public BaseConfig { ... app fields ... };
      class  AppPreferences : public BasePreferences { ... overrides ... };
      class  AppApManager  : public BaseAccessPointManager {
          void initializeFormFields() override;  // call base first
      };

- `initializeFormFields()` must call `BaseAccessPointManager::initializeFormFields()`
  first so WiFi/TZ/logLevel fields are always at the top of the portal form.

- `BaseConfig` carries only SSID, password, POSIX timezone, and log level.
  Everything else goes in the app's derived struct.

- `runBlockingLoop()` deliberately has no display dependency. If the app
  wants to push banners to a display while waiting, do it in the concrete
  AP manager subclass or in the caller before invoking `runBlockingLoop()`.

- WiFi/prefs types (FormField, PrefSelectOption, AppLogLevel, PrefType,
  FieldValidation) live in `wifi_types.h`. Clock/display types live in
  ESP32NTPClock's `enc_types.h`, which re-includes `wifi_types.h`.

- Comments and logs match the existing casual, specific voice. No emoji,
  no marketing words, no "comprehensive solution" phrasing.

## Do not
- Don't add display, animation, or NTP/clock types to this component.
  Those belong in ESP32NTPClock.
- Don't add `esp_http_client` to REQUIRES. That is for HTTP fetching
  (weather client, geo-TZ) which lives at the clock layer. This component
  only needs `esp_http_server` (captive portal) and `lwip` (DNS task).
- Don't change `MAX_PREF_STRING_LEN` without coordinating across every
  app that has existing NVS entries — old strings would be silently
  truncated on the next read.
- Don't touch `.git/` or rewrite history.
