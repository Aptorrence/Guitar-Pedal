add_library(core INTERFACE)

target_link_libraries(core INTERFACE driver)
add_subdirectory(driver)
add_subdirectory(core)
if(NOT "${BOARD}" STREQUAL "NATIVE")
    add_subdirectory(componets)
endif()

