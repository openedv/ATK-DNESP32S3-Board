# Create an INTERFACE library for our C module.
add_library(usermod_iic INTERFACE)

# Add our source files to the lib
target_sources(usermod_iic INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/24cxx.c
    ${CMAKE_CURRENT_LIST_DIR}/xl9555.c
    ${CMAKE_CURRENT_LIST_DIR}/ap3216c.c
    ${CMAKE_CURRENT_LIST_DIR}/qma6100p.c
    ${CMAKE_CURRENT_LIST_DIR}/oled.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_iic INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_iic)
