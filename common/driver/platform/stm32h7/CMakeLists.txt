add_library(hal_h7 STATIC
    hal_gpio.cc
    hal_i2c.cc
    hal_spi.cc
    hal_delay.cc
    hal_audio_stream.cc
)

target_include_directories(hal_h7 PUBLIC .)

target_link_libraries(hal_h7 PUBLIC
    st_h7_hal
    core
)
