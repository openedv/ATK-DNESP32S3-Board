# Create an INTERFACE library for our C module.
add_library(usermod_ltdc INTERFACE)

# Add our source files to the lib
target_sources(usermod_ltdc INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ltdc.c
    ${CMAKE_CURRENT_LIST_DIR}/touch.c
    ${CMAKE_CURRENT_LIST_DIR}/gt9xxx.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_ltdc INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Link our INTERFACE library to the usermod target.
target_link_libraries(usermod INTERFACE usermod_ltdc)