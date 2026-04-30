cmake_minimum_required(VERSION 3.16)

# esp32_wifi must be present as components/esp32_wifi before running idf.py.
# Quick setup:
#   git submodule add https://github.com/v01dma1n/ESP32WiFi2 components/esp32_wifi

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(wifi2-minimal)
