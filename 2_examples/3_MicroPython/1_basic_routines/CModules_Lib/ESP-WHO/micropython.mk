WHO_CAMERA_MOD_DIR := $(USERMOD_DIR)

SRC_USERMOD_CXX += $(WHO_CAMERA_MOD_DIR)/esp_cat_face_detection.cpp
SRC_USERMOD_CXX += $(WHO_CAMERA_MOD_DIR)/esp_color_detection.cpp
SRC_USERMOD_CXX += $(WHO_CAMERA_MOD_DIR)/esp_face_detection.cpp
SRC_USERMOD_CXX += $(WHO_CAMERA_MOD_DIR)/esp_face_recognition.cpp
SRC_USERMOD_CXX += $(WHO_CAMERA_MOD_DIR)/esp_qr_detection.cpp
SRC_USERMOD += $(WHO_CAMERA_MOD_DIR)/esp_config.c
SRC_USERMOD += $(WHO_CAMERA_MOD_DIR)/lcd.c


# Add our module directory to the include path.
CFLAGS_USERMOD += -I$(WHO_CAMERA_MOD_DIR) -DMODULE_CAMERA_ENABLED=1
CXXFLAGS_USERMOD += -I$(WHO_CAMERA_MOD_DIR) -std=c++11

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++