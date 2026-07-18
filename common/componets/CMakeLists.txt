add_library(componets STATIC
    bno055/bno055.c
    cs4270/cs4270.cc
)

target_include_directories(componets PUBLIC
    bmp390
    bno055
    w25q
    cs4270
)

target_link_libraries(componets PUBLIC driver)