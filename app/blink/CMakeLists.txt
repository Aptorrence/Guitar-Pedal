set(EXECUTABLE blink)

# Define Firmware project. app
add_executable(${EXECUTABLE}
    main.cpp
)

target_link_libraries(${EXECUTABLE} PRIVATE bsp)

add_firmware_outputs(${EXECUTABLE})
