if ("${MCU_FAMILY}" STREQUAL "STM32L4")
    add_subdirectory(platform/stm32l4)
elseif ("${MCU_FAMILY}" STREQUAL "STM32H7")
    add_subdirectory(platform/stm32h7)
endif()

add_library(driver INTERFACE)

target_include_directories(driver INTERFACE
    bus
    io
)

if(NOT "${BOARD}" STREQUAL "NATIVE")
    if ("${MCU_FAMILY}" STREQUAL "STM32H7")
        target_link_libraries(driver INTERFACE hal_h7 utils)
    else()
        target_link_libraries(driver INTERFACE hal utils)
    endif()
endif()