#include "ESP32WiFi2.h"
#include "app_preferences.h"
#include "app_ap_manager.h"

#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static constexpr const char* HOST_NAME     = "wifi2-demo";
static constexpr int         WIFI_ATTEMPTS = 3;

extern "C" void app_main() {
    // NVS must be ready before anything touches preferences.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    AppPreferences prefs;
    prefs.setup();
    prefs.getPreferences();
    prefs.dumpPreferences();

    AppApManager apManager(prefs);
    BootManager  bootManager;

    bool forceAp = bootManager.checkForForceAPMode();

    if (!forceAp) {
        WiFiInit();
        bool connected = WiFiConnect(HOST_NAME,
                                     prefs.config.ssid,
                                     prefs.config.password,
                                     WIFI_ATTEMPTS);
        if (connected) {
            bootManager.markBootStable();
            LOGINF("Connected — running application");

            while (true) {
                // Application work goes here.
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
        }
        WiFiStop();
        LOGINF("WiFi connect failed — entering AP mode");
    }

    // Reaches here on double-reset or connection failure.
    apManager.setup(HOST_NAME);
    apManager.runBlockingLoop();   // blocks until user saves config + device reboots
}
