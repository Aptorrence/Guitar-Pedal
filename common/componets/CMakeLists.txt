add_library(componets STATIC
    cs4270/cs4270.cc
)

target_include_directories(componets PUBLIC
    cs4270
)

target_link_libraries(componets PUBLIC driver)