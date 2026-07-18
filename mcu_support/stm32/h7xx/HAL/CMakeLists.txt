FILE(GLOB SRC_FILES *.c)

add_library(st_h7_hal STATIC
    ${SRC_FILES}
)

target_include_directories(st_h7_hal PUBLIC .)
target_link_libraries(st_h7_hal PUBLIC board_hal_config)
target_compile_options(st_h7_hal PRIVATE -w)