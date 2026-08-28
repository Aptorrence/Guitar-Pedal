add_library(hal STATIC
    st_gpio.cc
    st_spi.cc
    st_i2c.cc
)

target_include_directories(hal PUBLIC
    .
)

target_link_libraries(hal PUBLIC
    st_l4_support
    core 
)