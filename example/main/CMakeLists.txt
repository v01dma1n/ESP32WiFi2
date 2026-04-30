idf_component_register(
    SRCS
        "main.cpp"
        "app_preferences.cpp"
        "app_ap_manager.cpp"
    INCLUDE_DIRS
        "."
    REQUIRES
        esp32_wifi
        nvs_flash
)

target_compile_options(${COMPONENT_LIB} PUBLIC -std=gnu++17)
