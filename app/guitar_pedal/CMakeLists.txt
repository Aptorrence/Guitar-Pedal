set(EXECUTABLE guitar_pedal)

# Define Firmware project. app
add_executable(${EXECUTABLE}
    main.cpp
    audio_engine.cpp
)

target_link_libraries(${EXECUTABLE} PRIVATE bsp dsp controls)

add_firmware_outputs(${EXECUTABLE})
