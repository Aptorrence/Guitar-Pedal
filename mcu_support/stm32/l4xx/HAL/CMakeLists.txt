FILE(GLOB SRC_FILES *.c)

add_library(st_l4_hal STATIC
    ${SRC_FILES}
)

target_include_directories(st_l4_hal PUBLIC .)
target_link_libraries(st_l4_hal PUBLIC board_hal_config)
target_compile_options(st_l4_hal PRIVATE -w)