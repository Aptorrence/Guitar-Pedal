add_subdirectory(HAL)   # defines st_h7_hal

add_library(st_h7_support STATIC
    system_stm32h7xx.c
)

target_include_directories(st_h7_support SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(st_h7_support PUBLIC
    board_hal_config
    cmsis
)

target_link_libraries(st_h7_hal PUBLIC st_h7_support)
