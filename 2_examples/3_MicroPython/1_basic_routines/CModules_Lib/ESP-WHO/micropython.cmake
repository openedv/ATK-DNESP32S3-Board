# Create an INTERFACE library for our C module.
add_library(usermod_esp32_who INTERFACE)

# Add our source files to the lib
target_sources(usermod_esp32_who INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/esp_cat_face_detection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/esp_color_detection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/esp_face_detection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/esp_face_recognition.cpp
    ${CMAKE_CURRENT_LIST_DIR}/esp_qr_detection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/esp_config.c
    ${CMAKE_CURRENT_LIST_DIR}/lcd.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_esp32_who INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
#	${IDF_PATH}/components/bus/include
    ${IDF_PATH}/components/esp-dl/include
    ${IDF_PATH}/components/esp-dl/include/detect
    ${IDF_PATH}/components/esp-dl/include/image
    ${IDF_PATH}/components/esp-dl/include/layer
    ${IDF_PATH}/components/esp-dl/include/math
    ${IDF_PATH}/components/esp-dl/include/nn
    ${IDF_PATH}/components/esp-dl/include/tool
    ${IDF_PATH}/components/esp-dl/include/typedef
    ${IDF_PATH}/components/esp-dl/include/model_zoo
    ${IDF_PATH}/components/fb_gfx
    ${IDF_PATH}/components/fb_gfx/include
    ${IDF_PATH}/components/esp32-camera/driver/include
    ${IDF_PATH}/components/esp32-camera/driver/private_include
    ${IDF_PATH}/components/esp32-camera/conversions/include
    ${IDF_PATH}/components/esp32-camera/conversions/private_include
    ${IDF_PATH}/components/esp32-camera/sensors/private_include
    ${IDF_PATH}/components/modules/ai
    ${IDF_PATH}/components/esp-code-scanner/include
)

target_compile_definitions(usermod_esp32_who INTERFACE)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_esp32_who)