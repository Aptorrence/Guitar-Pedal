if ("${MCU_FAMILY}" STREQUAL "STM32H7")
    add_subdirectory(platform/stm32h7)
endif()

add_library(driver INTERFACE)

target_include_directories(driver INTERFACE
    bus
    io
    time
)

if ("${MCU_FAMILY}" STREQUAL "STM32H7")
    target_link_libraries(driver INTERFACE hal_h7)
endif()